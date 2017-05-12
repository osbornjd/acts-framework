template <class T>
FW::ProcessCode
FWRoot::RootExCellWriter<T>::write(const Acts::ExtrapolationCell<T>& eCell)
{
  {
    // lock the mutex
    std::lock_guard<std::mutex> lock(m_write_mutex);
    
    ACTS_VERBOSE("Writing an eCell object with "
                 << eCell.extrapolationSteps.size()
                 << " steps.");

    // the event paramters
    auto sMomentum = eCell.startParameters.momentum();
    m_eta          = sMomentum.eta();
    m_phi          = sMomentum.phi();
    m_materialX0   = eCell.materialX0;
    m_materialL0   = eCell.materialL0;

    // clear the vectors & reserve
    // - for main step information
    m_s_positionX.clear();
    m_s_positionY.clear();
    m_s_positionZ.clear();
    m_s_positionR.clear();
    m_s_volumeID.clear();
    m_s_layerID.clear();
    m_s_surfaceID.clear();
    m_s_positionX.reserve(MAXSTEPS);
    m_s_positionY.reserve(MAXSTEPS);
    m_s_positionZ.reserve(MAXSTEPS);
    m_s_positionR.reserve(MAXSTEPS);
    m_s_volumeID.reserve(MAXSTEPS);
    m_s_layerID.reserve(MAXSTEPS);
    m_s_surfaceID.reserve(MAXSTEPS);

    // - for the sensitive
    if (m_cfg.writeSensitive) {
      m_s_sensitive.clear();
      m_s_localposition0.clear();
      m_s_localposition1.clear();
      m_s_sensitive.reserve(MAXSTEPS);
      m_s_localposition0.reserve(MAXSTEPS);
      m_s_localposition1.reserve(MAXSTEPS);
    }
    // - for the material
    if (m_cfg.writeMaterial) {
      m_s_material.clear();
      m_s_materialX0.clear();
      m_s_materialL0.clear();
      m_s_material.reserve(MAXSTEPS);
      m_s_materialX0.reserve(MAXSTEPS);
      m_s_materialL0.reserve(MAXSTEPS);
    }
    // - for the boundary
    if (m_cfg.writeBoundary) {
      m_s_boundary.clear();
      m_s_boundary.reserve(MAXSTEPS);
    }
    // the number of sensitive hits per event
    m_hits = 0;
    // loop over extrapolation steps
    for (auto& es : eCell.extrapolationSteps) {
      if (es.parameters) {
        /// step parameters
        const T& pars = (*es.parameters);
        /// type information
        int material = es.configuration.checkMode(
            Acts::ExtrapolationMode::CollectMaterial);
        int boundary = es.configuration.checkMode(
            Acts::ExtrapolationMode::CollectBoundary);
        int sensitive = es.configuration.checkMode(
            Acts::ExtrapolationMode::CollectSensitive);
        int passive = es.configuration.checkMode(
            Acts::ExtrapolationMode::CollectPassive);
        /// check the layer, surface, volume ID
        geo_id_value volumeID = pars.referenceSurface().geoID().value(
            Acts::GeometryID::volume_mask);
        geo_id_value layerID = pars.referenceSurface().geoID().value(
            Acts::GeometryID::layer_mask);
        geo_id_value surfaceID = pars.referenceSurface().geoID().value(
            Acts::GeometryID::sensitive_mask);
        ///
        if ((m_cfg.writeSensitive && sensitive)
            || (m_cfg.writeBoundary && boundary)
            || (m_cfg.writeMaterial && material)
            || (m_cfg.writePassive && passive)) {

          // the material steps
          if (m_cfg.writeMaterial) {
            // the material is being written out
            double materialStepX0 = 0.;
            double materialStepL0 = 0.;
            if (es.material) {
              // assign the material
              materialStepX0
                  = es.materialScaling * es.material->thicknessInX0();
              materialStepX0
                  = es.materialScaling * es.material->thicknessInL0();
            }
            m_s_materialX0.push_back(materialStepX0);
            m_s_materialX0.push_back(materialStepL0);
          }

          /// goblal position information
          m_s_positionX.push_back(pars.position().x());
          m_s_positionY.push_back(pars.position().y());
          m_s_positionZ.push_back(pars.position().z());
          m_s_positionR.push_back(pars.position().perp());

          /// local position information - only makes sense for sensitive really
          if (m_cfg.writeSensitive) {
            m_s_localposition0.push_back(pars.parameters()[Acts::eLOC_X]);
            m_s_localposition1.push_back(pars.parameters()[Acts::eLOC_Y]);
          }

          ///
          m_s_volumeID.push_back(volumeID);
          m_s_layerID.push_back(layerID);
          m_s_surfaceID.push_back(surfaceID);

          /// indicate what hit you have
          m_s_material.push_back(material);
          m_s_boundary.push_back(boundary);
          m_s_sensitive.push_back(sensitive);
        }
        if (sensitive) m_hits++;
      }
    }
    // write to
    m_outputTree->Fill();
  }

  // return scuess
  return FW::ProcessCode::SUCCESS;
}

template <class T> 
FWRoot::RootExCellWriter<T>::RootExCellWriter(
  const FWRoot::RootExCellWriter<T>::Config& cfg)
  : FW::IWriterT< const Acts::ExtrapolationCell<T> >()
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
}

template <class T> 
FWRoot::RootExCellWriter<T>::~RootExCellWriter()
{
}

template <class T> 
FW::ProcessCode
FWRoot::RootExCellWriter<T>::initialize()
{
  ACTS_INFO("Registering new ROOT output File : " << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), m_cfg.fileMode.c_str());
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());

  // initial parameters
  m_outputTree->Branch("Eta", &m_eta);
  m_outputTree->Branch("Phi", &m_phi);
  
  // output the step information
  m_outputTree->Branch("StepX", &m_s_positionX);
  m_outputTree->Branch("StepY", &m_s_positionY);
  m_outputTree->Branch("StepZ", &m_s_positionZ);
  m_outputTree->Branch("StepR", &m_s_positionR);

  // identification
  m_outputTree->Branch("StepVolumeID",  &m_s_volumeID);
  m_outputTree->Branch("StepLayerID",   &m_s_layerID);
  m_outputTree->Branch("StepSurfaceID", &m_s_surfaceID);
  // material section
  if (m_cfg.writeMaterial){
    m_outputTree->Branch("MaterialX0",     &m_materialX0);
    m_outputTree->Branch("MaterialL0",     &m_materialL0);
    m_outputTree->Branch("StepMaterialX0", &m_s_materialX0);
    m_outputTree->Branch("StepMaterialL0", &m_s_materialL0);
    m_outputTree->Branch("MaterialStep",   &m_s_material);
  }
  // sensitive section
  if (m_cfg.writeSensitive){
    m_outputTree->Branch("SensitiveStep", &m_s_sensitive);
    m_outputTree->Branch("StepLocal0",    &m_s_localposition0);
    m_outputTree->Branch("StepLocal1",    &m_s_localposition1);
  }
  // boundary section
  if (m_cfg.writeBoundary)
    m_outputTree->Branch("BoundaryStep", &m_s_boundary);
  // numer of sensitive hits
  m_outputTree->Branch("nHits", &m_hits);

  return FW::ProcessCode::SUCCESS;
}

template <class T> 
FW::ProcessCode
FWRoot::RootExCellWriter<T>::finalize()
{
  ACTS_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();
  return FW::ProcessCode::SUCCESS;
}

template <class T>
FW::ProcessCode
FWRoot::RootExCellWriter<T>::write(const std::string&)
{
  return FW::ProcessCode::SUCCESS;
}


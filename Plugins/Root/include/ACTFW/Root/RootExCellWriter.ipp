
template <class T> FW::ProcessCode RootExCellWriter::writeT(const Acts::ExtrapolationCell<T>& eCell)
{
    ACTS_VERBOSE("Writing an eCell object with " << eCell.extrapolationSteps.size() << " steps.");
    
    // the event paramters
    auto sMomentum = eCell.startParameters.momentum();
    m_eta        =  sMomentum.eta();
    m_phi        =  sMomentum.phi();
    m_materialX0 = eCell.materialX0;
    m_materialL0 = eCell.materialL0;
    
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
    if (m_cfg.writeSensitive){
      m_s_sensitive.clear();
      m_s_localposition0.clear();
      m_s_localposition1.clear();
      m_s_sensitive.reserve(MAXSTEPS);
      m_s_localposition0.reserve(MAXSTEPS);
      m_s_localposition1.reserve(MAXSTEPS);
    }
    // - for the material
    if (m_cfg.writeMaterial){
      m_s_material.clear();
      m_s_materialX0.clear();
      m_s_materialL0.clear();
      m_s_material.reserve(MAXSTEPS);
      m_s_materialX0.reserve(MAXSTEPS);
      m_s_materialL0.reserve(MAXSTEPS);
    }
    // - for the boundary
    if (m_cfg.writeBoundary){
      m_s_boundary.clear();
      m_s_boundary.reserve(MAXSTEPS);
    }
    // loop over extrapolation steps
    for (auto& es : eCell.extrapolationSteps){
        if (es.parameters){
          /// step parameters
            const T& pars = (*es.parameters);
            /// type information
            int material  = es.stepConfiguration.checkMode(Acts::ExtrapolationMode::CollectMaterial);
            int boundary  = es.stepConfiguration.checkMode(Acts::ExtrapolationMode::CollectBoundary);
            int sensitive = es.stepConfiguration.checkMode(Acts::ExtrapolationMode::CollectSensitive);
            int passive   = es.stepConfiguration.checkMode(Acts::ExtrapolationMode::CollectPassive);
            /// check the layer, surface, volume ID
            Acts::geo_id_value volumeID = pars.associatedSurface().geoID().value(
                                                                Acts::GeometryID::volume_mask,
                                                                Acts::GeometryID::volume_shift);
            Acts::geo_id_value layerID = pars.associatedSurface().geoID().value(
                                                                Acts::GeometryID::layer_mask,
                                                                Acts::GeometryID::layer_shift);
            Acts::geo_id_value surfaceID = pars.associatedSurface().geoID().value(
                                                                  Acts::GeometryID::sensitive_mask,
                                                                  Acts::GeometryID::sensitive_shift);   
            /// 
            if ((m_cfg.writeSensitive && sensitive) || 
                (m_cfg.writeBoundary && boundary) || 
                (m_cfg.writeMaterial && material) ||
                (m_cfg.writePassive && passive)) {
                
                // the material steps
                if (m_cfg.writeMaterial){
                  // the material is being written out
                  double materialStepX0 = 0.;
                  double materialStepL0 = 0.;
                  if (es.material){
                    // assign the material
                    materialStepX0 = es.materialScaling*es.material->thicknessInX0();
                    materialStepX0 = es.materialScaling*es.material->thicknessInL0();
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
                if (m_cfg.writeSensitive){
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
        }
    }
    // write to 
    m_outputTree->Fill();
    
    // return scuess
    return FW::ProcessCode::SUCCESS;
}


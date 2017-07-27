#include "ACTFW/Fatras/FatrasAlgorithm.hpp"
#include <iostream>
#include <random>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Units.hpp"
#include <deque>

template <class MaterialEngine>
FWA::FatrasAlgorithm<MaterialEngine>::FatrasAlgorithm(
    const FWA::FatrasAlgorithm<MaterialEngine>::Config& cfg,
    std::unique_ptr<const Acts::Logger>                 logger)
  : FW::Algorithm(cfg, std::move(logger)), m_cfg(cfg)
{
}

/** Framework finalize mehtod */
template <class MaterialEngine>
FW::ProcessCode
FWA::FatrasAlgorithm<MaterialEngine>::initialize(
    std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::ABORT;
  }

  if (!m_cfg.randomNumbers) {
    ACTS_FATAL("No random number service handed over, can not proceed with "
               "extrapolation!");
    return FW::ProcessCode::ABORT;
  }

  if (!m_cfg.materialInteractionEngine) {
    ACTS_FATAL(
        "No material interaction engine handed over, can not proceed with "
        "extrapolation!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

template <class MaterialEngine>
FW::ProcessCode
FWA::FatrasAlgorithm<MaterialEngine>::execute(
    const FW::AlgorithmContext context) const
{
  // Create an algorithm local random number generator
  FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(context);
  // Set the random generator of the material interaction
  m_cfg.materialInteractionEngine->setRandomGenerator(rng);
  // we read from a collection
  // -> will be outsourced into a simulation algorithm
  if (m_cfg.evgenParticlesCollection != "") {
    // Retrieve relevant information from the execution context
    auto eventStore = context.eventContext->eventStore;

    // prepare the input vector
    std::vector<Acts::ParticleProperties>* eventParticles = nullptr;
    // read and go
    if (eventStore
        && eventStore->readT(eventParticles, m_cfg.evgenParticlesCollection)
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
    // run over it
    ACTS_INFO("Successfully read in collection with " << eventParticles->size()
                                                      << " particles");

    // create a new detector data hit container
    typedef std::pair<std::unique_ptr<const Acts::TrackParameters>,
                      barcode_type>
        FatrasHit;
    FW::DetectorData<geo_id_value, FatrasHit>* simulatedHits
        = m_cfg.simulatedHitsCollection != ""
        ? new FW::DetectorData<geo_id_value, FatrasHit>
        : nullptr;
    // prepare the output vector
    std::vector<Acts::ParticleProperties>* simulatedParticles
        = m_cfg.simulatedParticlesCollection != ""
        ? new std::vector<Acts::ParticleProperties>
        : nullptr;

    // counters
    size_t pCounter = 0;
    size_t sCounter = 0;
      
    // the collection of extrapolation cells to be written to the event store
    std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>* exCells
      = m_cfg.simulatedExCell != ""
      ? new std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>() : nullptr;
      
    for (auto& eParticle : (*eventParticles)) {
      // process the particle // TODO make configuraable
      if (eParticle.charge() != 0. && eParticle.vertex().perp() < 1.
          && eParticle.momentum().perp() > m_cfg.minPt
          && fabs(eParticle.momentum().eta()) < m_cfg.maxEta) {
        // count simulated particles
        ++pCounter;
        // record the simulated particles
        if (simulatedParticles) simulatedParticles->push_back(eParticle);
        // TODO update to better structure with Vertex-Particle tree
        Acts::PerigeeSurface surface(eParticle.vertex());
        double               d0    = 0.;
        double               z0    = 0.;
        double               phi   = eParticle.momentum().phi();
        double               theta = eParticle.momentum().theta();
        double qop = eParticle.charge() / eParticle.momentum().mag();
        // parameters
        Acts::ActsVectorD<5> pars;
        pars << d0, z0, phi, theta, qop;
        // some screen output
        std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;
        // create the bound parameters
        Acts::BoundParameters startParameters(
            std::move(cov), std::move(pars), surface);
          
        //-------------------Do the extrapolation-------------------
        // setup the extrapolation how you'd like it
          Acts::ExtrapolationCell<Acts::TrackParameters> ecc(startParameters);
        // ecc.setParticleHypothesis(m_cfg.particleType);
        ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
        ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
        ecc.searchMode = m_cfg.searchMode;
        ecc.particleType = eParticle.particleType();
        // now set the behavioral bits
        if (m_cfg.collectSensitive)
            ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectSensitive);
        if (m_cfg.collectPassive)
            ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectPassive);
        if (m_cfg.collectBoundary)
            ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectBoundary);
        if (m_cfg.collectMaterial)
            ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);
        if (m_cfg.sensitiveCurvilinear) ecc.sensitiveCurvilinear = true;
          
        // force a stop in the extrapoaltion mode
        if (m_cfg.pathLimit > 0.) {
            ecc.pathLimit = m_cfg.pathLimit;
            ecc.addConfigurationMode(Acts::ExtrapolationMode::StopWithPathLimit);
        }
        // screen output
        ACTS_DEBUG("===> forward extrapolation - collecting information <<===");
          
        // call the extrapolation engine
        Acts::ExtrapolationCode eCode = m_cfg.extrapolationEngine->extrapolate(ecc);
        if (eCode.isFailure()) {
            ACTS_WARNING("Extrapolation failed.");
            return FW::ProcessCode::ABORT;
        }
        // store the extrapolation cell
          if (exCells) exCells->push_back(std::move(ecc));

        //-------------------Fill the hits & material of each particle-------------------
          // theta / phi
          auto  sPosition = startParameters.position();
          double sTheta   = startParameters.momentum().theta();
          double sPhi     = startParameters.momentum().theta();
          double tX0      = 0.;
          double tL0      = 0.;
          
          // call the writer
       //   if (m_cfg.ecChargedWriter && m_cfg.ecChargedWriter->write(ecc) == FW::ProcessCode::ABORT){
       //       ACTS_ERROR("Could not write ExtrapolationCell to writer. Aborting.");
       ///       return FW::ProcessCode::ABORT;
       ///   }
          
          // material steps to be  filled
          std::vector< Acts::MaterialStep > materialSteps;
          
          // fill the detectorData container if you have one
          /// loop over steps and get the sensitive
          for (auto& es : ecc.extrapolationSteps ){
              // check if you have parameters
              if (es.parameters){
                  // get the surface
                  const Acts::Surface& sf = es.parameters->referenceSurface();
                  // check if you have material
                  if (es.configuration.checkMode(Acts::ExtrapolationMode::CollectSensitive) && simulatedHits) {
                      // fill the appropriate vector
                      geo_id_value volumeID = sf.geoID().value(Acts::GeometryID::volume_mask);
                      geo_id_value layerID  = sf.geoID().value(Acts::GeometryID::layer_mask);
                      geo_id_value moduleID = sf.geoID().value(Acts::GeometryID::sensitive_mask);
                      // search and/or insert
                      std::pair< std::unique_ptr<const Acts::TrackParameters>, barcode_type > eHit(std::move(es.parameters), eParticle.barcode());
                      FW::Data::insert(*simulatedHits, volumeID, layerID, moduleID, std::move(eHit) );
                  }
              }
              if (es.material && m_cfg.materialWriter && es.surface){
                  tX0 += es.material->thicknessInX0();
                  tL0 += es.material->thicknessInL0();
                  // collect the material steps
                  materialSteps.push_back(Acts::MaterialStep(*es.material,
                                                             es.position,
                                                             es.surface->geoID().value()));
                  
              }
          }
          // also check the end parameters
          if (ecc.endParameters){
              // get the endSurface
              const Acts::Surface& esf = ecc.endParameters->referenceSurface();
              // check if sensitive
              if (esf.associatedDetectorElement()){
                  // fill the appropriate vector
                  geo_id_value volumeID = esf.geoID().value(Acts::GeometryID::volume_mask);
                  geo_id_value layerID  = esf.geoID().value(Acts::GeometryID::layer_mask);
                  geo_id_value moduleID = esf.geoID().value(Acts::GeometryID::sensitive_mask);
                  // search and/or insert
                  std::pair< std::unique_ptr<const Acts::TrackParameters>, barcode_type > eHit(std::move(ecc.endParameters), eParticle.barcode());
                  FW::Data::insert(*simulatedHits, volumeID, layerID, moduleID, std::move(eHit) );
              }
          }
          
          // write the material if configured for it
          if (m_cfg.materialWriter) {
              ACTS_VERBOSE("Writing out the material track to a file");
              Acts::MaterialTrack mTrack(sPosition, sTheta, sPhi, materialSteps, tX0, tL0);
              // call the writer
              if (m_cfg.materialWriter->write(mTrack) == FW::ProcessCode::ABORT){
                  ACTS_ERROR("Could not write MaterialTrack to writer. Aborting.");
                  return FW::ProcessCode::ABORT;
              }
          }

          
        
      } else
        ++sCounter;
    }

    //-------------------Write out-------------------
    ACTS_INFO("Number of simulated particles : " << pCounter);
    ACTS_INFO("Number of skipped   particles : " << sCounter);
    if (eventStore && simulatedParticles
        && eventStore->writeT(simulatedParticles,
                              m_cfg.simulatedParticlesCollection)
            == FW::ProcessCode::ABORT) {
      ACTS_WARNING(
          "Could not write colleciton of simulated particles to event store.");
      return FW::ProcessCode::ABORT;
    } else if (simulatedParticles)
      ACTS_INFO("Truth information for "
                << simulatedParticles->size()
                << " particles written to EventStore.");

    // write to the EventStore
    if (eventStore && simulatedHits
        && eventStore->writeT(simulatedHits, m_cfg.simulatedHitsCollection)
            == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not write colleciton of hits to event store.");
      return FW::ProcessCode::ABORT;
    } else if (simulatedHits)
      ACTS_INFO("Hit information for " << simulatedHits->size()
                                       << " volumes written to EventStore.");
      
    if (eventStore
        && exCells && eventStore->writeT(exCells,
                                        m_cfg.simulatedExCell)
        == FW::ProcessCode::ABORT) {
        ACTS_WARNING(
                     "Could not write collection of simulated extrapolation cells to event store.");
        return FW::ProcessCode::ABORT;
    }
  }
    
  // return SUCCESS to the frameword
  return FW::ProcessCode::SUCCESS;
}

template <class MaterialEngine>
FW::ProcessCode
FWA::FatrasAlgorithm<MaterialEngine>::finalize()
{
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

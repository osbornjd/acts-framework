//
//  ExtrapolationAlgorithm.ipp
//  ACTS-Development
//
//  Created by Andreas Salzburger on 19/05/16.
//
//
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

template <class T>
FW::ProcessCode
FWA::ExtrapolationAlgorithm::executeTestT(
    const T& startParameters,
    barcode_type barcode,
    FW::DetectorData<geo_id_value, 
      std::pair< std::unique_ptr<const T>, barcode_type >  >* dData,
    std::shared_ptr<FW::IWriterT< Acts::ExtrapolationCell<T> > > writer) const
{

  // setup the extrapolation how you'd like it
  Acts::ExtrapolationCell<T> ecc(startParameters);
  // ecc.setParticleHypothesis(m_cfg.particleType);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
  ecc.searchMode = m_cfg.searchMode;
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
  
  // theta / phi
  auto  sPosition = startParameters.position();
  double sTheta   = startParameters.momentum().theta();
  double sPhi     = startParameters.momentum().theta();
  double tX0      = 0.;
  double tL0      = 0.;
  // material steps to be  filled
  std::vector< Acts::MaterialStep > materialSteps;
  
  // call the extrapolation engine
  Acts::ExtrapolationCode eCode = m_cfg.extrapolationEngine->extrapolate(ecc);
  if (eCode.isFailure()) {
    ACTS_WARNING("Extrapolation failed.");
    return FW::ProcessCode::ABORT;
  }
  
  // call the writer
  if (writer && writer->write(ecc) == FW::ProcessCode::ABORT){
    ACTS_ERROR("Could not write ExtrapolationCell to writer. Aborting.");
    return FW::ProcessCode::ABORT;
  }
  
  // fill the detectorData container if you have one
  /// loop over steps and get the sensitive
  for (auto& es : ecc.extrapolationSteps ){
    // check if you have parameters
    if (es.parameters){
        // get the surface
        const Acts::Surface& sf = es.parameters->referenceSurface();
        // check if you have material
        if (es.configuration.checkMode(Acts::ExtrapolationMode::CollectSensitive) && dData) {
          // fill the appropriate vector
          geo_id_value volumeID = sf.geoID().value(Acts::GeometryID::volume_mask);
          geo_id_value layerID  = sf.geoID().value(Acts::GeometryID::layer_mask);
          geo_id_value moduleID = sf.geoID().value(Acts::GeometryID::sensitive_mask);
          // search and/or insert
          std::pair< std::unique_ptr<const T>, barcode_type > eHit(std::move(es.parameters), barcode);
          FW::Data::insert(*dData, volumeID, layerID, moduleID, std::move(eHit) );
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
      std::pair< std::unique_ptr<const T>, barcode_type > eHit(std::move(ecc.endParameters), barcode);
      FW::Data::insert(*dData, volumeID, layerID, moduleID, std::move(eHit) );
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
  
  // return success
  return FW::ProcessCode::SUCCESS;
}


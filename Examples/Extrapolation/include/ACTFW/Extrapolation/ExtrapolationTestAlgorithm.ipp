//
//  ExtrapolationTestAlgorithm.ipp
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
#include "ACTFW/Writers/IExtrapolationCellWriter.hpp"

template <class T>
FW::ProcessCode
FWE::ExtrapolationTestAlgorithm::executeTestT(
    const T& startParameters,
    barcode_type barcode,
    FW::DetectorData<geo_id_value, std::pair< std::unique_ptr<const T>, barcode_type > >* dData) const
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
  // call the extrapolation engine
  Acts::ExtrapolationCode eCode = m_cfg.extrapolationEngine->extrapolate(ecc);
  if (eCode.isFailure()) {
    ACTS_WARNING("Extrapolation failed.");
    return FW::ProcessCode::ABORT;
  }
  // write out if configured
  if (m_cfg.extrapolationCellWriter) m_cfg.extrapolationCellWriter->write(ecc);
  // fill the detectorData container if you have one
  // - sensitive only
  if (dData){
     /// loop over steps and get the sensitive
    for (auto& es : ecc.extrapolationSteps ){
      if (es.stepConfiguration.checkMode(Acts::ExtrapolationMode::CollectSensitive)) {
        // get the surface
        const Acts::Surface& sf = es.parameters->referenceSurface();
        // fill the appropriate vector
        geo_id_value volumeID = sf.geoID().value(Acts::GeometryID::volume_mask);
        geo_id_value layerID  = sf.geoID().value(Acts::GeometryID::layer_mask);
        geo_id_value moduleID = sf.geoID().value(Acts::GeometryID::sensitive_mask);
        // search and/or insert
        std::pair< std::unique_ptr<const T>, barcode_type > eHit(std::move(es.parameters), barcode);
        FW::Data::insert(*dData, volumeID, layerID, moduleID, std::move(eHit) );
      }
    }
    
    
  }
  // return success
  return FW::ProcessCode::SUCCESS;
}


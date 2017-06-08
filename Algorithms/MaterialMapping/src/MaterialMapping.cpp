///////////////////////////////////////////////////////////////////
// MaterialMapping.cpp
///////////////////////////////////////////////////////////////////

#include <iostream>
#include "ACTFW/MaterialMapping/MaterialMapping.hpp"
#include "ACTS/Plugins/MaterialPlugins/SurfaceMaterialRecord.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "TTree.h"

FWA::MaterialMapping::MaterialMapping(
    const FWA::MaterialMapping::Config& cnf,
    std::unique_ptr<const Acts::Logger> log)
  : FW::Algorithm(cnf, std::move(log)), m_cfg(cnf)
{
}

FWA::MaterialMapping::~MaterialMapping()
{
}

FW::ProcessCode
FWA::MaterialMapping::initialize(
    std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }

  // set up the reader
  if (!m_cfg.materialTrackReader) {
    ACTS_ERROR("Algorithm::materialTrackReader not set!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");

  // set up the material mapper
  if (!m_cfg.materialMapper) {
    ACTS_ERROR("Algorithm::MaterialMapping not set!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::MaterialMapping::execute(const FW::AlgorithmContext /*context*/) const
{
  
  if (!m_cfg.materialMapper || !m_cfg.trackingGeometry){
    ACTS_INFO("MaterialMapper or TrackingGeometry not available. Aborting.");
    return FW::ProcessCode::ABORT;
  }
    
  // retrive a cache object 
  Acts::MaterialMapper::Cache mCache
    = m_cfg.materialMapper->materialMappingCache(*m_cfg.trackingGeometry);
    
  // access the tree and read the records
  Acts::MaterialTrack inputTrack;
  
  for (size_t itc = 0 ; 
       m_cfg.materialTrackReader->read(inputTrack) != FW::ProcessCode::ABORT; ++itc){

     
    ACTS_VERBOSE("Read MaterialTrack " << itc << " from file, it has " 
      << inputTrack.materialSteps().size() << " steps.");
    
    // some screen output to know what is going on 
    ACTS_VERBOSE("These will be mapped onto "
               << mCache.surfaceMaterialRecords.size() << " surfaces.");
    
    // perform the mapping
    auto mappedTrack = m_cfg.materialMapper->mapMaterialTrack(mCache,inputTrack);
    if (m_cfg.materialTrackWriter){
      // write out the material for validation purpose
      m_cfg.materialTrackWriter->write(mappedTrack);
    }
    
    // break if configured
    if (m_cfg.maximumTrackRecords > 0 && itc > m_cfg.maximumTrackRecords){
      ACTS_VERBOSE("Maximum track records reached. Stopping.");
      break;
    }
  }
  /// get the maps back 
  std::map<Acts::GeometryID, Acts::SurfaceMaterial*> sMaterialMaps 
    = m_cfg.materialMapper->createSurfaceMaterial(mCache);

  //// write the maps out to a file 
  if (m_cfg.indexedMaterialWriter){
    // screen output
    ACTS_INFO("Writing out the material maps for " 
      << sMaterialMaps.size() << " material surfaces");
    // loop over the material maps
    for (auto& sMap : sMaterialMaps) {
        // write out map by map
        m_cfg.indexedMaterialWriter->write(sMap);
    }
  }
  
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::MaterialMapping::finalize()
{

  ACTS_VERBOSE("finalize successful.");
  return FW::ProcessCode::SUCCESS;
}

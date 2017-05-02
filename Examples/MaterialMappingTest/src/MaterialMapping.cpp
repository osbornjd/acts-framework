///////////////////////////////////////////////////////////////////
// MaterialMapping.cpp
///////////////////////////////////////////////////////////////////

#include "MaterialMapping.hpp"
#include <iostream>
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecReader.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Plugins/MaterialPlugins/SurfaceMaterialRecord.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapper.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"
#include "TTree.h"

FWE::MaterialMapping::MaterialMapping(
    const FWE::MaterialMapping::Config& cnf,
    std::unique_ptr<Acts::Logger>                log)
  : FW::Algorithm(cnf, std::move(log)), m_cfg(cnf)
{
}

FWE::MaterialMapping::~MaterialMapping()
{
}

FW::ProcessCode
FWE::MaterialMapping::initialize(
    std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }

  // set up the reader
  if (!m_cfg.materialTrackRecReader) {
    ACTS_ERROR("Algorithm::MaterialTrackRecReader not set!");
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
FWE::MaterialMapping::execute(const FW::AlgorithmContext context) const
{
  
  if (!m_cfg.materialMapper || !m_cfg.trackingGeometry){
    ACTS_INFO("MaterialMapper or TrackingGeometry not available. Aborting.");
    return FW::ProcessCode::ABORT;
  }
  
  // access the tree and read the records
  std::vector<Acts::MaterialTrackRecord> mRecords
      = m_cfg.materialTrackRecReader->materialTrackRecords();

  ACTS_INFO("Read " << mRecords.size()
                    << " MaterialTrackRecords from file.");
  
  // retrive a cache object 
  Acts::MaterialMapper::Cache mCache
    = m_cfg.materialMapper->materialMappingCache(*m_cfg.trackingGeometry);
  
  // some screen output to know what is going on 
  ACTS_INFO("These will be mapped onto "
             << mCache.surfaceMaterialRecords.size() << " surfaces.");

  /// eventual stop counter - initialised
  size_t stopCounter = 0;
  // go through the records and map them
  for (auto& record : mRecords) {
    // perform the mapping
    if (!m_cfg.materialMapper->mapMaterialTrackRecord(mCache,record)){
      ACTS_ERROR("Problem in the mapping. Aborting.");
      return FW::ProcessCode::ABORT;
    }
    if (++stopCounter > m_cfg.maximumTrackRecords) {
      ACTS_INFO("External break condition of maximal " 
          << m_cfg.maximumTrackRecords << " reached.");
      break;
    }
  }
  
  /// get the maps back 
  std::map<Acts::GeometryID, Acts::SurfaceMaterial*> sMaterialMaps 
  = m_cfg.materialMapper->createSurfaceMaterial(mCache);

  // write the maps out to a file 
  if (m_cfg.materialWriter){
    // screen output
    ACTS_INFO("Writing out the material maps for " << sMaterialMaps.size() << " material surfaces");
    // loop over the material maps
    for (auto& sMap : sMaterialMaps) {
        // write out map by map
        m_cfg.materialWriter->write(*sMap.second,sMap.first,"");
    }
  }
  
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::MaterialMapping::finalize()
{

  ACTS_VERBOSE("finalize successful.");
  return FW::ProcessCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// MaterialMapping.cpp
///////////////////////////////////////////////////////////////////

#include <iostream>

#include <TTree.h>

#include "ACTFW/MaterialMapping/MaterialMapping.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Plugins/MaterialPlugins/SurfaceMaterialRecord.hpp"

FWA::MaterialMapping::MaterialMapping(const FWA::MaterialMapping::Config& cnf,
                                      Acts::Logging::Level                level)
  : FW::BareAlgorithm("MaterialMapping", level), m_cfg(cnf)
{
}

FWA::MaterialMapping::~MaterialMapping()
{
}

FW::ProcessCode
    FWA::MaterialMapping::execute(FW::AlgorithmContext /*context*/) const
{
  if (!m_cfg.materialMapper || !m_cfg.trackingGeometry) {
    ACTS_INFO("MaterialMapper or TrackingGeometry not available. Aborting.");
    return FW::ProcessCode::ABORT;
  }

  // retrive a cache object
  Acts::MaterialMapper::Cache mCache
      = m_cfg.materialMapper->materialMappingCache(*m_cfg.trackingGeometry);

  // access the tree and read the records
  Acts::MaterialTrack inputTrack;

  for (size_t itc = 0;
       m_cfg.materialTrackReader->read(inputTrack) != FW::ProcessCode::ABORT;
       ++itc) {
    ACTS_VERBOSE("Read MaterialTrack " << itc << " from file, it has "
                                       << inputTrack.materialSteps().size()
                                       << " steps.");

    // some screen output to know what is going on
    ACTS_VERBOSE("These will be mapped onto "
                 << mCache.surfaceMaterialRecords.size() << " surfaces.");

    // perform the mapping
    auto mappedTrack
        = m_cfg.materialMapper->mapMaterialTrack(mCache, inputTrack);
    if (m_cfg.materialTrackWriter) {
      // write out the material for validation purpose
      m_cfg.materialTrackWriter->write(mappedTrack);
    }

    // break if configured
    if (m_cfg.maximumTrackRecords > 0 && itc > m_cfg.maximumTrackRecords) {
      ACTS_VERBOSE("Maximum track records reached. Stopping.");
      break;
    }
  }
  /// get the maps back
  std::map<Acts::GeometryID, Acts::SurfaceMaterial*> sMaterialMaps
      = m_cfg.materialMapper->createSurfaceMaterial(mCache);

  //// write the maps out to a file
  if (m_cfg.indexedMaterialWriter) {
    // screen output
    ACTS_INFO("Writing out the material maps for " << sMaterialMaps.size()
                                                   << " material surfaces");
    // loop over the material maps
    for (auto& sMap : sMaterialMaps) {
      // write out map by map
      m_cfg.indexedMaterialWriter->write(sMap);
    }
  }

  return FW::ProcessCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// MaterialMappingAlgorithm.cpp
///////////////////////////////////////////////////////////////////

#include "MaterialMappingAlgorithm.hpp"
#include <iostream>
#include "ACTFW/Root/RootMaterialTrackRecReader.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Plugins/MaterialPlugins/LayerMaterialRecord.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapping.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"
#include "TTree.h"

FWE::MaterialMappingAlgorithm::MaterialMappingAlgorithm(
    const FWE::MaterialMappingAlgorithm::Config& cnf,
    std::unique_ptr<Acts::Logger>                log)
  : FW::Algorithm(cnf, std::move(log)), m_cnf(cnf)
{
}

FWE::MaterialMappingAlgorithm::~MaterialMappingAlgorithm()
{
}

FW::ProcessCode
FWE::MaterialMappingAlgorithm::initialize(
    std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }

  // set up the reader
  if (!m_cnf.materialTrackRecReader) {
    ACTS_ERROR("Algorithm::MaterialTrackRecReader not set!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");

  // set up the material mapper
  if (!m_cnf.materialMapper) {
    ACTS_ERROR("Algorithm::MaterialMapping not set!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::MaterialMappingAlgorithm::execute(const FW::AlgorithmContext context) const
{
  // average after every event - if entries = 0 no averaging will be done
  m_cnf.materialMapper->averageLayerMaterial();
  // access the tree
  std::vector<Acts::MaterialTrackRecord> mRecords
      = m_cnf.materialTrackRecReader->materialTrackRecords();
  ACTS_INFO("Collected " << mRecords.size()
                         << " MaterialTrackRecords for this event.");
  // go through the records and map them
  for (auto& record : mRecords) {
    double         theta = record.theta();
    double         phi   = record.phi();
    Acts::Vector3D direction(
        cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
    ACTS_INFO("direction: "
              << "("
              << direction.x()
              << ","
              << direction.y()
              << ","
              << direction.z()
              << ")");
    m_cnf.materialMapper->mapMaterial(record);
  }
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::MaterialMappingAlgorithm::finalize()
{
  // average material and hand over to layer
  m_cnf.materialMapper->averageLayerMaterial();
  ACTS_INFO("finalize layer material");
  m_cnf.materialMapper->finalizeLayerMaterial();
  ACTS_INFO("finalize successful.");
  /*   ACTS_INFO("Now get the material maps of the layers and print them");
     const std::map<const Acts::Layer*, Acts::LayerMaterialRecord> layerRecords
   = m_cnf.materialMapper->layerRecords();
     ACTS_INFO("finalize1");
     for (auto& layerRecord : layerRecords) {
         ACTS_INFO("layerRecord");
   //      std::shared_ptr<const Acts::BinnedSurfaceMaterial> layerMaterial =
   layerRecord.second.layerMaterial();
         m_cnf.materialWriter->write(layerRecord.second.layerMaterial(),layerRecord.first->geoID());
     }
     ACTS_INFO("finalize successful2.");*/
  return FW::ProcessCode::SUCCESS;
}

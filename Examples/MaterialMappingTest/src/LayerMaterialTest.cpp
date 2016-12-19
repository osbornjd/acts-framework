///////////////////////////////////////////////////////////////////
// LayerMaterialTest.cpp
///////////////////////////////////////////////////////////////////

#include "LayerMaterialTest.hpp"
#include <iostream>
#include "ACTFW/Root/RootMaterialTrackRecReader.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Plugins/MaterialPlugins/LayerMaterialRecord.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapping.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"
#include "TTree.h"

FWE::LayerMaterialTest::LayerMaterialTest(
    const FWE::LayerMaterialTest::Config& cnf,
    std::unique_ptr<Acts::Logger>         log)
  : FW::Algorithm(cnf, std::move(log)), m_cnf(cnf)
{
}

FWE::LayerMaterialTest::~LayerMaterialTest()
{
}

FW::ProcessCode
FWE::LayerMaterialTest::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }

  // set up the writer
  if (!m_cnf.materialWriter) {
    ACTS_ERROR("Algorithm::MaterialWriter not set!");
    return FW::ProcessCode::ABORT;
  }

  // set up the writer
  if (!m_cnf.materialStepWriter) {
    ACTS_ERROR("Algorithm::MaterialStepWriter not set!");
    return FW::ProcessCode::ABORT;
  }

  // set up the material mapper
  if (!m_cnf.materialMapper) {
    ACTS_ERROR("Algorithm::MaterialMapping not set!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::LayerMaterialTest::execute(const FW::AlgorithmContext context) const
{
  ACTS_INFO("Now access the material maps of the layers and print them");
  // average after every event - if entries = 0 no
  // averaging will be done
  // access the layer records
  const std::map<const Acts::Layer*, Acts::LayerMaterialRecord> layerRecords
      = m_cnf.materialMapper->layerRecords();
  size_t nLayer = 0;
  // loop through the layer records and print out the Binned Surface material
  for (auto& layerRecord : layerRecords) {
    std::string                  lname = "layer" + std::to_string(nLayer);
    const Acts::SurfaceMaterial* surfMat
        = layerRecord.first->materialSurface()->associatedMaterial();
    const Acts::BinnedSurfaceMaterial* binnedSurfMat
        = dynamic_cast<const Acts::BinnedSurfaceMaterial*>(surfMat);
    std::shared_ptr<const Acts::BinnedSurfaceMaterial> bsMat(
        binnedSurfMat->clone());
    m_cnf.materialWriter->write(bsMat, layerRecord.first->geoID(), lname);
    nLayer++;
  }
  // access the material steps assigned per layer and write out the material at
  // a specific position for each layer
  ACTS_INFO("Access the geant4 material steps per layer and print them");
  const std::map<const Acts::Layer*,
                 std::vector<std::pair<const Acts::MaterialStep,
                                       const Acts::Vector3D>>>
      layersAndSteps = m_cnf.materialMapper->layerMaterialSteps();

  for (auto& step : layersAndSteps) {
    std::string histName = std::to_string(step.first->geoID().value());
    m_cnf.materialStepWriter->write(
        histName, step.first->materialSurface(), step.second);
  }
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::LayerMaterialTest::finalize()
{
  ACTS_INFO("finalize successful.");
  return FW::ProcessCode::SUCCESS;
}

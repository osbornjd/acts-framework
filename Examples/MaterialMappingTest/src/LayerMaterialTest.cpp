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
  // loop through the layer records and print out the material
  for (auto& layerRecord : layerRecords) {
    std::string lname = "layer" + std::to_string(nLayer);
    /*    m_cnf.materialWriter->write(
            layerRecord.second.layerMaterial(), layerRecord.first->geoID(),
       lname);*/

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
  const std::multimap<const Acts::Layer*, const Acts::MaterialStep>
      layersAndSteps = m_cnf.materialMapper->layerMaterialSteps();

  // Now compare with original material -> write out all the material steps
  // which are assigned to one layer
  // access the layers of which the material should be printed out
  std::vector<std::pair<const Acts::Layer*, const Acts::MaterialStep>> layers;
  unique_copy(
      begin(layersAndSteps),
      end(layersAndSteps),
      back_inserter(layers),
      [](const std::pair<const Acts::Layer*, const Acts::MaterialStep>& entry1,
         const std::pair<const Acts::Layer*, const Acts::MaterialStep>&
             entry2) { return (entry1.first == entry2.first); });
  int nLayers = 0;
  // go through layers and write out all the material which was assigned to them
  for (auto& layer : layers) {
    // now access all the material steps assigned to one layer
    std::pair<std::multimap<const Acts::Layer*,
                            const Acts::MaterialStep>::const_iterator,
              std::multimap<const Acts::Layer*,
                            const Acts::MaterialStep>::const_iterator>
        layerRange;
    layerRange = layersAndSteps.equal_range(layer.first);
    // write out the steps per layer
    std::vector<Acts::MaterialStep> steps;

    for (auto step = layerRange.first; step != layerRange.second; ++step) {
      steps.push_back((*step).second);
    }
    std::string histName = "G4layer" + std::to_string(nLayers) + "_"
        + std::to_string(layer.first->geoID().value());
    ACTS_INFO("before MaterialStepWriter write()");
    m_cnf.materialStepWriter->write(
        histName, layer.first->materialSurface(), steps);
    nLayers++;
  }  // go through layers
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::LayerMaterialTest::finalize()
{
  ACTS_INFO("finalize successful.");
  return FW::ProcessCode::SUCCESS;
}

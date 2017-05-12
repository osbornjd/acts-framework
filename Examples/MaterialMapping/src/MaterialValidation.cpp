///////////////////////////////////////////////////////////////////
// MaterialValidation.cpp
///////////////////////////////////////////////////////////////////

#include "MaterialValidation.hpp"
#include <iostream>
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecReader.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Plugins/MaterialPlugins/SurfaceMaterialRecord.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapper.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"
#include "TTree.h"

FWE::MaterialValidation::MaterialValidation(
    const FWE::MaterialValidation::Config& cnf,
    std::unique_ptr<Acts::Logger>         log)
  : FW::Algorithm(cnf, std::move(log)), m_cfg(cnf)
{
}

FWE::MaterialValidation::~MaterialValidation()
{
}

FW::ProcessCode
FWE::MaterialValidation::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }

  // set up the writer
  if (!m_cfg.materialWriter) {
    ACTS_ERROR("Algorithm::MaterialWriter not set!");
    return FW::ProcessCode::ABORT;
  }

  // set up the writer
  if (!m_cfg.materialStepWriter) {
    ACTS_ERROR("Algorithm::MaterialStepWriter not set!");
    return FW::ProcessCode::ABORT;
  }

  // set up the material mapper
  if (!m_cfg.materialMapper) {
    ACTS_ERROR("Algorithm::MaterialMapping not set!");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("initialize successful.");

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::MaterialValidation::execute(const FW::AlgorithmContext context) const
{
  ACTS_INFO("Now access the material maps of the layers and print them");
// // average after every event - if entries = 0 no
// // averaging will be done
// // access the layer records
// const std::map<const Acts::Layer*, Acts::LayerMaterialRecord> layerRecords
//     = m_cfg.materialMapper->layerRecords();
// size_t nLayer = 0;
// // loop through the layer records and print out the Binned Surface material
// for (auto& layerRecord : layerRecords) {
//   std::string                  lname = "Layer" + std::to_string(nLayer);
//   const Acts::SurfaceMaterial* surfMat
//       = layerRecord.first->materialSurface()->associatedMaterial();
//   const Acts::BinnedSurfaceMaterial* binnedSurfMat
//       = dynamic_cast<const Acts::BinnedSurfaceMaterial*>(surfMat);
//   std::shared_ptr<const Acts::BinnedSurfaceMaterial> bsMat(
//       binnedSurfMat->clone());
//   m_cfg.materialWriter->write(bsMat, layerRecord.first->geoID(), lname);
//   nLayer++;
//
//   // access the material steps assigned per layer and write out the material
//   // at
//   // a specific position for each layer
//   ACTS_INFO("Access the geant4 material steps per layer and print them");
//
//   std::vector<std::pair<const std::vector<Acts::MaterialStep>,
//                         const Acts::Vector3D>>
//       layerMaterialSteps = layerRecord.second.layerMaterialSteps();
//
//   std::vector<Acts::MaterialStep> steps;
//   std::vector<std::pair<const Acts::Vector3D, const Acts::Vector3D>>
//       realAndAssignedPos;
//
//   for (auto& layerMaterialStep : layerMaterialSteps) {
//     std::vector<Acts::MaterialStep> materialSteps = layerMaterialStep.first;
//
//     // sum up all the material of the layer at this point
//     float thickness = 0.;
//     float rho       = 0.;
//     float x0        = 0.;
//     float l0        = 0.;
//     float A         = 0.;
//     float Z         = 0.;
//     float x         = 0.;
//     float y         = 0.;
//     float z         = 0.;
//     for (auto& layerStep : materialSteps) {
//       float t       = layerStep.material().thickness();
//       float density = layerStep.material().averageRho();
//       thickness += t;
//       rho += density * t;
//       x0 += layerStep.material().x0() * t;
//       l0 += layerStep.material().l0() * t;
//       A += layerStep.material().averageA() * density * t;
//       Z += layerStep.material().averageZ() * density * t;
//       x += layerStep.position().x;
//       y += layerStep.position().y;
//       z += layerStep.position().z;
//       realAndAssignedPos.push_back(
//           std::make_pair(Acts::Vector3D(layerStep.position().x,
//                                         layerStep.position().y,
//                                         layerStep.position().z),
//                          layerMaterialStep.second));
//     }
//     if (rho != 0.) {
//       A /= rho;
//       Z /= rho;
//     }
//     if (thickness != 0.) {
//       x0 /= thickness;
//       l0 /= thickness;
//       rho /= thickness;
//     }
//     if (materialSteps.size()) {
//       x /= materialSteps.size();
//       y /= materialSteps.size();
//       z /= materialSteps.size();
//     }
//
//     const Acts::MaterialStep averagedStep(
//         Acts::MaterialProperties(x0, l0, A, Z, rho, thickness),
//         Acts::MaterialStep::Position(x, y, z));
//
//     steps.push_back(averagedStep);
//   }
//   std::string histName = "G4Layer"
//       + std::to_string(std::distance(layerRecords.begin(),
//                                      layerRecords.find(layerRecord.first)));
//
//   m_cfg.materialStepWriter->write(histName,
//                                   layerRecord.first->materialSurface(),
//                                   steps,
//                                   realAndAssignedPos);
// }

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::MaterialValidation::finalize()
{
  ACTS_INFO("finalize successful.");
  return FW::ProcessCode::SUCCESS;
}

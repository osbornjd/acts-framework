// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"

#include <iostream>
#include <stdexcept>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Detector/DetectorElementBase.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/PlanarModuleStepper.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/ParameterDefinitions.hpp"

FW::DigitizationAlgorithm::DigitizationAlgorithm(
    const FW::DigitizationAlgorithm::Config& cfg,
    Acts::Logging::Level                     level)
  : FW::BareAlgorithm("DigitizationAlgorithm", level), m_cfg(cfg)
{
  if (m_cfg.simulatedHitsCollection.empty()) {
    throw std::invalid_argument("Missing input hits collection");
  } else if (m_cfg.spacePointsCollection.empty()) {
    throw std::invalid_argument("Missing output space points collection");
  } else if (m_cfg.clustersCollection.empty()) {
    throw std::invalid_argument("Missing output clusters collection");
  } else if (!m_cfg.randomNumbers) {
    throw std::invalid_argument("Missing random numbers service");
  } else if (!m_cfg.planarModuleStepper) {
    throw std::invalid_argument("Missing planar module stepper");
  }
}

FW::ProcessCode
FW::DigitizationAlgorithm::execute(FW::AlgorithmContext ctx) const
{
  // prepare the input data
  const FW::DetectorData<geo_id_value,
                         std::pair<std::unique_ptr<const Acts::TrackParameters>,
                                   barcode_type>>* hitData
      = nullptr;
  // read and go
  if (ctx.eventStore.get(m_cfg.simulatedHitsCollection, hitData)
      == FW::ProcessCode::ABORT)
    return FW::ProcessCode::ABORT;

  ACTS_DEBUG("Retrieved hit data '" << m_cfg.simulatedHitsCollection
                                    << "' from event store.");

  // the particle mass table
  Acts::ParticleMasses pMasses;

  // prepare the output data: Clusters
  FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster> planarClusters;
  // perpare the second output data : SpacePoints
  FW::DetectorData<geo_id_value, Acts::Vector3D> spacePoints;

  // now digitise
  for (auto& vData : (*hitData)) {
    auto volumeKey = vData.first;
    ACTS_DEBUG("- Processing Volume Data collection for volume with ID "
               << volumeKey);
    for (auto& lData : vData.second) {
      auto layerKey = lData.first;
      ACTS_DEBUG("-- Processing Layer Data collection for layer with ID "
                 << layerKey);
      for (auto& sData : lData.second) {
        auto moduleKey = sData.first;
        ACTS_DEBUG("-- Processing Module Data collection for module with ID "
                   << moduleKey);
        // get the hit parameters
        for (auto& hit : sData.second) {
          auto hitParameters   = hit.first.get();
          auto particleBarcode = hit.second;
          // get the surface
          const Acts::Surface& hitSurface = hitParameters->referenceSurface();
          // get the DetectorElement
          auto hitDetElement = hitSurface.associatedDetectorElement();
          if (hitDetElement) {
            // get the digitization module
            auto hitDigitizationModule = hitDetElement->digitizationModule();
            if (hitDigitizationModule) {
              // get the lorentz angle
              double lorentzAngle = hitDigitizationModule->lorentzAngle();
              double thickness    = hitDetElement->thickness();
              double lorentzShift = thickness * tan(lorentzAngle);
              lorentzShift *= -(hitDigitizationModule->readoutDirection());
              // parameters
              auto           pars     = hitParameters->parameters();
              auto           position = hitParameters->position();
              auto           momentum = hitParameters->momentum();
              Acts::Vector2D localIntersection(pars[Acts::ParDef::eLOC_0],
                                               pars[Acts::ParDef::eLOC_1]);
              Acts::Vector3D localDirection(
                  hitSurface.transform().inverse().linear() * momentum);
              // position
              std::vector<Acts::DigitizationStep> dSteps
                  = m_cfg.planarModuleStepper->cellSteps(*hitDigitizationModule,
                                                         localIntersection,
                                                         localDirection.unit());
              // everything under threshold or edge effects
              if (!dSteps.size()) continue;
              /// let' create a cluster - centroid method
              double localX    = 0.;
              double localY    = 0.;
              double totalPath = 0.;
              // the cells to be used
              std::vector<Acts::DigitizationCell> usedCells;
              usedCells.reserve(dSteps.size());
              // loop over the steps
              for (auto dStep : dSteps) {
                // @todo implement smearing
                localX += dStep.stepLength * dStep.stepCellCenter.x();
                localY += dStep.stepLength * dStep.stepCellCenter.y();
                totalPath += dStep.stepLength;
                usedCells.push_back(
                    std::move(Acts::DigitizationCell(dStep.stepCell.channel0,
                                                     dStep.stepCell.channel1,
                                                     dStep.stepLength)));
              }
              // divide by the total path
              localX /= totalPath;
              localX += lorentzShift;
              localY /= totalPath;

              // get the segmentation & find the corresponding cell id
              const Acts::Segmentation& segmentation
                  = hitDigitizationModule->segmentation();
              auto           binUtility = segmentation.binUtility();
              Acts::Vector2D localPosition(localX, localY);
              // @todo remove unneccesary conversion
              size_t bin0          = binUtility.bin(localPosition, 0);
              size_t bin1          = binUtility.bin(localPosition, 1);
              size_t binSerialized = binUtility.serialize({bin0, bin1, 0});

              // the covariance is currently set to 0.
              Acts::ActsSymMatrixD<2> cov;
              cov << 0., 0., 0., 0.;

              // create the indetifier
              Acts::GeometryID geoID(0);
              geoID.add(volumeKey, Acts::GeometryID::volume_mask);
              geoID.add(layerKey, Acts::GeometryID::layer_mask);
              geoID.add(moduleKey, Acts::GeometryID::sensitive_mask);
              geoID.add(binSerialized, Acts::GeometryID::channel_mask);
              // create the truth for this - assume here muons
              Acts::ParticleProperties pProperties(
                  momentum, pMasses.mass[Acts::muon], 1., 13, particleBarcode);
              // the associated process vertex
              Acts::ProcessVertex pVertex(position, 0., 0, {pProperties}, {});

              // create the planar cluster
              Acts::PlanarModuleCluster pCluster(hitSurface,
                                                 Identifier(geoID.value()),
                                                 std::move(cov),
                                                 localX,
                                                 localY,
                                                 std::move(usedCells),
                                                 {pVertex});

              // insert into the space point map
              FW::Data::insert(spacePoints,
                               volumeKey,
                               layerKey,
                               moduleKey,
                               hitParameters->position());

              // insert into the cluster map
              FW::Data::insert(planarClusters,
                               volumeKey,
                               layerKey,
                               moduleKey,
                               std::move(pCluster));

            }  // hit moulde proection
          }    // hit element protection
        }      // hit loop
      }        // moudle loop
    }          // layer loop
  }            // volume loop

  // write the SpacePoints to the EventStore
  if (ctx.eventStore.add(m_cfg.spacePointsCollection, std::move(spacePoints))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  // write the clusters to the EventStore
  if (ctx.eventStore.add(m_cfg.clustersCollection, std::move(planarClusters))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  return FW::ProcessCode::SUCCESS;
}

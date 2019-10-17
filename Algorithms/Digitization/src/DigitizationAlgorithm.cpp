// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"

#include <iostream>
#include <stdexcept>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/EventData/SimHit.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/DetectorElementBase.hpp"
#include "Acts/Geometry/GeometryID.hpp"
#include "Acts/Plugins/Digitization/DigitizationModule.hpp"
#include "Acts/Plugins/Digitization/PlanarModuleCluster.hpp"
#include "Acts/Plugins/Digitization/PlanarModuleStepper.hpp"
#include "Acts/Plugins/Digitization/Segmentation.hpp"
#include "Acts/Plugins/Identification/IdentifiedDetectorElement.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/ParameterDefinitions.hpp"

FW::DigitizationAlgorithm::DigitizationAlgorithm(
    const FW::DigitizationAlgorithm::Config& cfg,
    Acts::Logging::Level                     level)
  : FW::BareAlgorithm("DigitizationAlgorithm", level), m_cfg(cfg)
{
  if (m_cfg.simulatedHitCollection.empty()) {
    throw std::invalid_argument("Missing input hits collection");
  }
  if (m_cfg.clusterCollection.empty()) {
    throw std::invalid_argument("Missing output clusters collection");
  }
  if (!m_cfg.randomNumberSvc) {
    throw std::invalid_argument("Missing random numbers service");
  }
  if (!m_cfg.planarModuleStepper) {
    throw std::invalid_argument("Missing planar module stepper");
  }
}

FW::ProcessCode
FW::DigitizationAlgorithm::execute(const AlgorithmContext& context) const
{
  // Read the input data collection from the event store
  const auto& simHits
      = context.eventStore.get<SimHits>(m_cfg.simulatedHitCollection);

  // Prepare the output data: Clusters
  FW::GeometryIdMultimap<Acts::PlanarModuleCluster> clusters;

  // now digitise
  for (const auto& hit : simHits) {
    const Acts::Surface&                  hitSurface  = *hit.surface;
    const Data::SimParticle*              hitParticle = &hit.particle;
    std::vector<const Data::SimParticle*> hitParticles{hitParticle};
    // get the DetectorElement
    auto hitDetElement = dynamic_cast<const Acts::IdentifiedDetectorElement*>(
        hitSurface.associatedDetectorElement());
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
        auto invTransfrom = hitSurface.transform(context.geoContext).inverse();
        // local intersection / direction
        Acts::Vector3D localIntersect3D(invTransfrom * hit.position);
        Acts::Vector2D localIntersection(localIntersect3D.x(),
                                         localIntersect3D.y());
        Acts::Vector3D localDirection(invTransfrom.linear()
                                      * hit.direction.normalized());
        // now calculate the steps through the silicon
        std::vector<Acts::DigitizationStep> dSteps
            = m_cfg.planarModuleStepper->cellSteps(context.geoContext,
                                                   *hitDigitizationModule,
                                                   localIntersection,
                                                   localDirection);
        // everything under threshold or edge effects
        if (!dSteps.size()) {
          ACTS_VERBOSE("No steps returned from stepper.");
          continue;
        }
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
          usedCells.push_back(Acts::DigitizationCell(dStep.stepCell.channel0,
                                                     dStep.stepCell.channel1,
                                                     dStep.stepLength));
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
        size_t binSerialized = binUtility.serialize({{bin0, bin1, 0}});

              // the covariance is currently set to 0.
              Acts::ActsSymMatrixD<3> cov;
              cov << 0.05, 0., 0.,
					 0., 0.05, 0.,
					 0., 0., 30. * Acts::UnitConstants::ps;

		  // create the planar cluster
		  Acts::PlanarModuleCluster pCluster(
			  hitSurface.getSharedPtr(),
			  Identifier(Identifier::identifier_type(geoID.value()),
						 hitParticles),
			  std::move(cov),
			  localX,
			  localY,
			  hit.timeStamp,
			  std::move(usedCells));

        // insert into the cluster container. since the input data is already
        // sorted by geoId, we should always be able to add at the end.
        clusters.emplace_hint(clusters.end(), hit.geoId(), std::move(cluster));
      }
    }
  }
  // write the clusters to the EventStore
  context.eventStore.add(m_cfg.clusterCollection, std::move(clusters));
  return FW::ProcessCode::SUCCESS;
}

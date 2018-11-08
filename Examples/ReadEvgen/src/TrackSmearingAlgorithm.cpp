// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "TrackSmearingAlgorithm.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include <iostream>


FWE::TrackSmearingAlgorithm::TrackSmearingAlgorithm(const Config& cfg, Acts::Logging::Level level)
  : FW::BareAlgorithm("TrackSmearing", level), m_cfg(cfg)
{
}

FW::ProcessCode
FWE::TrackSmearingAlgorithm::execute(FW::AlgorithmContext context) const
{
	const std::vector<FW::Data::SimVertex<>>* inputEvent = nullptr;
	if (context.eventStore.get(m_cfg.collection, inputEvent) == FW::ProcessCode::ABORT)
	{
    	return FW::ProcessCode::ABORT;
	}

	// Get first vertex of event
	FW::Data::SimVertex<> vtx = (*inputEvent)[0];

	// Define initial position and momentum for a toy particle
	Acts::Vector3D pos(0.,0.,0.);
	Acts::Vector3D mom(10.,0.,0.);

	Acts::CurvilinearParameters start(nullptr, pos, mom, 0);

	// Create PlaneSurface w/o bounds in direction of particle momentum
	Acts::PlaneSurface endSurface(Acts::Vector3D(10.,0.,0.), Acts::Vector3D(1.,0.,0.));

	// Set up b-field and stepper
	Acts::ConstantBField bField(Acts::Vector3D(0.,0.,0.));
	Acts::EigenStepper<Acts::ConstantBField> stepper(bField);

	// Set up propagator with void navigator
	Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>> propagator(stepper);

	// Set up propagator options
	propagator.PropagatorOptions<> options;

	const auto result = propagator.propagate(start, endSurface, options);

	// Iterate over all particle emerging from current vertex // TODO: use iterator?
	for (auto const& particle : vtx.out){
		// Only charged particles
		if (particle.q() !=0) 
		{
			
			//do stuff
		}
		std::cout << particle.q() << std::endl;
	}

	return FW::ProcessCode::SUCCESS;
}
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
#include "Acts/Surfaces/PerigeeSurface.hpp"
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
	Acts::Vector3D pos(10.,10.,0.);
	Acts::Vector3D mom(10.,0.,0.);

	Acts::CurvilinearParameters start(nullptr, pos, mom, 0);

	/*
	// Create PlaneSurface w/o bounds in direction of particle momentum
	Acts::PlaneSurface endSurface(Acts::Vector3D(15.,0.,0.), Acts::Vector3D(1.,0.,0.));
	*/

	// Define perigee surface center coordinates
	double pgSrfX = 0.;
	double pgSrfY = 0.;
	double pgSrfZ = 0.;

	Acts::PerigeeSurface endSurface(Acts::Vector3D(pgSrfX, pgSrfY, pgSrfZ));

	// Set up b-field and stepper
	Acts::ConstantBField bField(Acts::Vector3D(0.,0.,0.));
	Acts::EigenStepper<Acts::ConstantBField> stepper(bField);

	// Set up propagator with void navigator
	Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>> propagator(stepper);

	// Set up propagator options
	Acts::PropagatorOptions<> options;
	// Run propagator
	const auto result = propagator.propagate(start, endSurface, options);

	// Obtain position of closest approach
	const auto& tp     = result.endParameters;
	const Acts::Vector3D& closestApp = tp->position();
	
	double x = closestApp[Acts::eX];
	double y = closestApp[Acts::eY];
	double z = closestApp[Acts::eZ];

	// Calculate corresponding Perigee params
	double d0 = std::sqrt( std::pow(x - pgSrfX,2) + std::pow(y - pgSrfY,2));
	double z0 = std::abs(z - pgSrfZ);

	std::cout << d0 << " " << z0 << std::endl;

	// Iterate over all particle emerging from current vertex // TODO: use iterator?
	for (auto const& particle : vtx.out){
		// Only charged particles
		if (particle.q() !=0) 
		{
			
			//do stuff
		}
		//std::cout << particle.q() << std::endl;
	}

	return FW::ProcessCode::SUCCESS;
}












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
#include "Acts/Utilities/Units.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include <iostream>

struct Config
  {
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;

    std::array<double, 2> gaussParameters   = {{0., 1.}};
    std::array<double, 2> uniformParameters = {{0., 1.}};
    std::array<double, 2> landauParameters  = {{0., 1.}};
    std::array<double, 2> gammaParameters   = {{0., 1.}};
    int poissonParameter = 40;

    size_t drawsPerEvent = 0;
  };


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

	// Define perigee surface center coordinates
	double pgSrfX = 0.;
	double pgSrfY = 0.;
	double pgSrfZ = 0.;

	//Acts::PlaneSurface   endSurface(Acts::Vector3D(10.,0.,0.), Acts::Vector3D(1.,0.,0.));
	Acts::PerigeeSurface endSurface(Acts::Vector3D(pgSrfX, pgSrfY, pgSrfZ));

	// Set up b-field and stepper
	Acts::ConstantBField bField(Acts::Vector3D(0.,1.5,0.)*Acts::units::_T);
	Acts::EigenStepper<Acts::ConstantBField> stepper(bField);
	
	// Set up propagator with void navigator
	Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>> propagator(stepper);

	// Set up propagator options
	Acts::PropagatorOptions<> options;

	// Create random number generator and spawn gaussian distribution
	FW::RandomEngine rng = m_cfg.randomNumberSvc->spawnGenerator(context);
	FW::GaussDist	 gaussDist(0.,1.);

	// Get first vertex of event
	FW::Data::SimVertex<> vtx = (*inputEvent)[0];

	// Iterate over all particle emerging from current vertex // TODO: use iterator?
	std::cout << vtx.out.size() << " particle emerging from vertex" << std::endl;
	int i = 0;
	for (auto const& particle : vtx.out){
		// Only charged particles
		if (particle.q() !=0) 
		{
			// Define start track params
			//Acts::CurvilinearParameters 
			//	start(nullptr, particle.position(), particle.momentum(), particle.q());

			Acts::CurvilinearParameters 
				start(nullptr, Acts::Vector3D(-100.,10.,0.), Acts::Vector3D(10.,0.,0.), 0.);

			// Run propagator
			const auto result = propagator.propagate(start, endSurface, options);
			// Obtain position of closest approach
			const auto& tp     = result.endParameters;
			std::cout << "HERE 4 " << std::endl;
			const Acts::Vector3D& closestApp = tp->position();
			std::cout << "HERE 5 " << std::endl;
			
			double x = closestApp[Acts::eX];
			double y = closestApp[Acts::eY];
			double z = closestApp[Acts::eZ];

			std::cout << "closestApproach: " << x << " " << y << " " << z << std::endl;

			// Calculate corresponding Perigee params
			double d0 = std::sqrt(std::pow(x - pgSrfX,2) + std::pow(y - pgSrfY,2));
			double z0 = std::abs(z - pgSrfZ);

			double rnGauss = gaussDist(rng);
			std::cout << "RN: " << rnGauss << std::endl;

			i++;
			std::cout << "particle " << i << ", d0=" << d0 << ", z0=" << z0 << std::endl;
			
		}
		//std::cout << particle.q() << std::endl;
	}

	return FW::ProcessCode::SUCCESS;
}












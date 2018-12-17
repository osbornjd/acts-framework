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
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include <iostream>

#include "Vertex.hpp"
#include "LinearizedTrack.hpp"
#include "LinearizedTrackFactory.hpp"
#include "FullVertexFitter.hpp"

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
	// Define parameter for pt-dependent IP resolution
	// of the form sigma_d/z(p_t[GeV]) = A*exp(-B*p_t[GeV]) + C
	static const double ipResA = 100.7439 * Acts::units::_um;
	static const double ipResB = 0.23055;
	static const double ipResC = 20. * Acts::units::_um;

	// Create and fill input event
	const std::vector<FW::Data::SimVertex<>>* inputEvent = nullptr;
	if (context.eventStore.get(m_cfg.collection, inputEvent) == FW::ProcessCode::ABORT)
	{
    	return FW::ProcessCode::ABORT;
	}

	// Define perigee surface center coordinates
	const double pgSrfX = 0.;
	const double pgSrfY = 0.;
	const double pgSrfZ = 0.;

	Acts::PerigeeSurface perigeeSurface(Acts::Vector3D(pgSrfX, pgSrfY, pgSrfZ));

	// Set up b-field and stepper
	Acts::ConstantBField bField(Acts::Vector3D(0.,0.,1.)*Acts::units::_T);
	Acts::EigenStepper<Acts::ConstantBField> stepper(bField);
	
	// Set up propagator with void navigator
	Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>> propagator(stepper);

	// Set up propagator options
	Acts::PropagatorOptions<> options;

	// Create random number generator and spawn gaussian distribution
	FW::RandomEngine rng = m_cfg.randomNumberSvc->spawnGenerator(context);

	// Get first vertex of event
	//FW::Data::SimVertex<> vtx = (*inputEvent)[0];

	// Initialize vector to be filled with smeared tracks
	std::vector<Acts::BoundParameters> smrdTrksVec;

	Acts::Vector3D tmpVtx;
	int count_v = 0; // count vertices
	int count_t = 0; // count tracks at vertex
	for (auto& vtx: (*inputEvent)){
		count_v++;
		if (count_v != 1) continue; // take only first vertex now

		//std::cout << "true vertex position: " << vtx.position << std::endl << std::endl;
		tmpVtx = vtx.position; //store tmp true position for debugging only

		//std::cout << count_v <<  " ntracks: " << vtx.out.size() << std::endl;
		// Iterate over all particle emerging from current vertex
		for (auto const& particle : vtx.out){

			const Acts::Vector3D& ptclMom = particle.momentum();
			// Calculate pseudo-rapidity
			const double eta = Acts::VectorHelpers::eta(ptclMom);
			// Only charged particles for |eta| < 2.5
			if (particle.q() !=0 && std::abs(eta) < 2.5) 
			{
				// Define start track params
				Acts::CurvilinearParameters 
					start(nullptr, particle.position(), ptclMom, particle.q());

				// Run propagator
				const auto result = propagator.propagate(start, perigeeSurface, options);

				if (result.status == Acts::Status::SUCCESS){

					const auto& perigeeParameters = result.endParameters->parameters(); // d0, z0, phi, theta,q/p

					if (std::abs(perigeeParameters[0]) > 30 || std::abs(perigeeParameters[1]) > 200){
						std::cout << "d0 = " << perigeeParameters[0] << std::endl;
						std::cout << "z0 = " << perigeeParameters[1] << std::endl;
						std::cout << "eta = " << eta << std::endl;
						std::cout << "#########" << std::endl;
						continue;
					}

					// Calculate pt-dependent IP resolution
					const double pclPt = 
							Acts::VectorHelpers::perp(ptclMom)/Acts::units::_GeV;
					const double ipRes = ipResA * std::exp(-ipResB*pclPt) + ipResC;

					// except for IP resolution, following variances are rough guesses
					// Gaussian distribution for IP resolution
					FW::GaussDist gaussDist_IP(0., ipRes);
					// Gaussian distribution for angular resolution
					FW::GaussDist gaussDist_angular(0., 0.1);
					// Gaussian distribution for q/p (momentum) resolution
					FW::GaussDist gaussDist_qp(0., 0.1*perigeeParameters[4]);

					double rn_d0 = gaussDist_IP(rng);
					double rn_z0 = gaussDist_IP(rng);
					double rn_ph = gaussDist_angular(rng);
					double rn_th = gaussDist_angular(rng);
					double rn_qp = gaussDist_qp(rng);

					double smrd_d0 	= perigeeParameters[0] + rn_d0;
					double smrd_z0	= perigeeParameters[1] + rn_z0;
					double smrd_phi 	= perigeeParameters[2] + rn_ph;
					double smrd_theta	= perigeeParameters[3] + rn_th;
					double srmd_qp	= perigeeParameters[4] + rn_qp;

					// smearing can bring theta out of range ->close to beam line -> discard
					if(smrd_theta < 0 || smrd_theta > M_PI){
						continue;
					}

					double new_eta = -log(tan(smrd_theta/2));
					if(std::abs(new_eta) > 2.5) continue;
				
					Acts::TrackParametersBase::ParVector_t paramVec;
					paramVec << smrd_d0, smrd_z0, smrd_phi, smrd_theta, srmd_qp;

					// Fill vector of smeared tracks
					std::unique_ptr<Acts::ActsSymMatrixD<5>> covMat = std::make_unique<Acts::ActsSymMatrixD<5>>();
					covMat->setZero();
					(*covMat)(0,0) = rn_d0*rn_d0;
					(*covMat)(1,1) = rn_z0*rn_z0;
					(*covMat)(2,2) = rn_ph*rn_ph;
					(*covMat)(3,3) = rn_th*rn_th;
					(*covMat)(4,4) = rn_qp*rn_qp;

					smrdTrksVec.push_back(Acts::BoundParameters(std::move(covMat), paramVec, perigeeSurface));

					count_t ++;
				}
			}
		}
	}

	FullVertexFitter<Acts::ConstantBField>::Config vf_config(bField);

	FullVertexFitter<Acts::ConstantBField> vf(vf_config);

	if (smrdTrksVec.size() > 1){
		Vertex v1 = vf.fit(smrdTrksVec);

		Acts::Vector3D diffVtx = tmpVtx - v1.position();

		std::cout << "true vertex:   "
			 << "(" << tmpVtx[0] << "," <<  tmpVtx[1] << ","<<   tmpVtx[2] << ")" << std::endl;
		std::cout << "fitted vertex: " 
			<< "(" << v1.position()[0] << "," <<  v1.position()[1] << ","<<   v1.position()[2] << ")" << std::endl;
	}
	

	if(context.eventStore.add(m_cfg.collectionOut, std::move(smrdTrksVec))
		!= FW::ProcessCode::SUCCESS)
	{
		return FW::ProcessCode::ABORT;
	}

	
	return FW::ProcessCode::SUCCESS;
}












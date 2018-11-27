
#include "LinearizedTrackFactory.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include <iostream>

LinearizedTrackFactory::LinearizedTrackFactory(){};
LinearizedTrackFactory::~LinearizedTrackFactory(){};

LinearizedTrack* LinearizedTrackFactory::linearizeTrack(const Acts::BoundParameters* params,
                                    const Acts::Vector3D& linPoint) const
{
	if (!params) return nullptr;

	const Acts::PerigeeSurface perigeeSurface(linPoint);

	// Set up b-field and stepper
	Acts::ConstantBField bField(Acts::Vector3D(0.,0.,1.)*Acts::units::_T);
	Acts::EigenStepper<Acts::ConstantBField> stepper(bField);
	
	// Set up propagator with void navigator
	Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>> propagator(stepper);

	// Set up propagator options
	Acts::PropagatorOptions<> options;

	const auto result = propagator.propagate(*params, perigeeSurface, options);
	if (result.status == Acts::Status::SUCCESS){
		const auto& perigeeParameters = result.endParameters->parameters();
		std::cout << perigeeParameters << std::endl;
	}
	else{
		std::cout << "propagation not successful" << std::endl;
	}


	return new LinearizedTrack();
}
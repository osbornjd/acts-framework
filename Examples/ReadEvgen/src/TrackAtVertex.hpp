
#pragma once

#include "Acts/EventData/TrackParameters.hpp"

class TrackAtVertex{
public:

	// Default constructor
	TrackAtVertex();

	TrackAtVertex(double chi2perTrack, 
				  const Acts::BoundParameters* paramsAtVertex,
				  const Acts::BoundParameters* originalParams);

private:

	double m_chi2Track;

	const Acts::BoundParameters* m_paramsAtVertex;

	const Acts::BoundParameters* m_originalParams;

};

#pragma once

#include "Acts/EventData/TrackParameters.hpp"

class TrackAtVertex{
public:

	// Default constructor
	//TrackAtVertex();

	TrackAtVertex(const double chi2perTrack, 
				  Acts::BoundParameters paramsAtVertex,
				  Acts::BoundParameters originalParams);

private:

	const double m_chi2Track;

	Acts::BoundParameters m_paramsAtVertex;

	Acts::BoundParameters m_originalParams;

};

#include "TrackAtVertex.hpp"

#include <iostream>

TrackAtVertex::TrackAtVertex(){};

TrackAtVertex::TrackAtVertex(double chi2perTrack, 	
				  const Acts::BoundParameters* paramsAtVertex,
				  const Acts::BoundParameters* originalParams) :
m_chi2Track(chi2perTrack),
m_paramsAtVertex(paramsAtVertex),
m_originalParams(originalParams){}


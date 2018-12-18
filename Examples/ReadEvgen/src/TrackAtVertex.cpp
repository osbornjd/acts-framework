
#include "TrackAtVertex.hpp"

#include <iostream>

//TrackAtVertex::TrackAtVertex(){};

TrackAtVertex::TrackAtVertex(const double chi2perTrack, 	
				  Acts::BoundParameters paramsAtVertex,
				  Acts::BoundParameters originalParams) :
m_chi2Track(chi2perTrack),
m_paramsAtVertex(paramsAtVertex),
m_originalParams(originalParams){}


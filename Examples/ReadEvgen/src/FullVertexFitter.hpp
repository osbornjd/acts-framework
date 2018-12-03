
#pragma once
#include "Vertex.hpp"
#include "Acts/EventData/TrackParameters.hpp"



class FullVertexFitter
{
public:
	FullVertexFitter();

	Vertex fit(const std::vector<Acts::BoundParameters>& paramVector, 
								 const Acts::Vector3D& startingPoint);

private:
	unsigned int m_maxIterations;
};

/// Class describing a vertex
///

#pragma once

#include "Acts/Utilities/Definitions.hpp"
#include "TrackAtVertex.hpp"

class Vertex{

public:
	/// Default constructor
	Vertex();

	/// Copy constructor
	Vertex(const Vertex& other);

	Vertex(const Acts::Vector3D& position,
		   const Acts::ActsSymMatrixD<3>& covariance,
		   std::vector<std::unique_ptr<TrackAtVertex>>& tracks);


	/// Return 3-position
	const Acts::Vector3D& position() const;
	/// Return covariance
	const Acts::ActsSymMatrixD<3>& covariance() const;

	/// Set 3-position
	void setPosition(const Acts::Vector3D& position);
	/// Set covariance
	void setCovariance(const Acts::ActsSymMatrixD<3>& covariance);
	/// Set tracks at vertex
	void setTracksAtVertex(
		std::vector<std::unique_ptr<TrackAtVertex>>& tracks);

private:
	Acts::Vector3D 			m_position;
	Acts::ActsSymMatrixD<3> m_covariance;
	std::vector<std::unique_ptr<TrackAtVertex>> m_tracksAtVertex;
};



/// Class describing a vertex
///

#ifndef VERTEX_H
#define VERTEX_H

#include "Acts/Utilities/Definitions.hpp"

class Vertex{

public:
	/// Default constructor
	Vertex();

	/// Copy constructor
	Vertex(const Vertex& other);


	/// Return 3-position
	const Acts::Vector3D& position() const;
	/// Return covariance
	const Acts::ActsSymMatrixD<3>& covariance() const;

	/// Set 3-position
	void setPosition(const Acts::Vector3D& position);
	/// Set covariance
	void setCovariance(const Acts::ActsSymMatrixD<3>& covariance);

private:
	Acts::Vector3D 			m_position;
	Acts::ActsSymMatrixD<3> m_covariance;

};

#endif
#include "Vertex.hpp"


Vertex::Vertex(){};

Vertex::Vertex(const Vertex& other) :
	m_position( other.m_position ) {};

const Acts::Vector3D& Vertex::position() const{
	return m_position;
}

const Acts::ActsSymMatrixD<3>& Vertex::covariance() const{
	return m_covariance;
}

void Vertex::setPosition(const Acts::Vector3D& position){
	m_position = position;
}

void Vertex::setCovariance(const Acts::ActsSymMatrixD<3>& covariance){
	m_covariance = covariance;
}

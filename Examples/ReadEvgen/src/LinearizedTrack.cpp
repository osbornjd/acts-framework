
#include "LinearizedTrack.hpp"
#include <iostream>

LinearizedTrack::LinearizedTrack()
{
	m_ParamsAtPCA.setZero();
	m_ParCovarianceAtPCA.setZero();
	m_LinPoint.setZero();
	m_PositionJacobian.setZero();
	m_MomentumJacobian.setZero();
	m_PositionAtPCA.setZero();
	m_MomentumAtPCA.setZero();
	m_ConstTerm.setZero();
}

LinearizedTrack::LinearizedTrack(const LinearizedTrack& other) :
m_ParamsAtPCA(other.m_ParamsAtPCA),
m_ParCovarianceAtPCA(other.m_ParCovarianceAtPCA),
m_LinPoint(other.m_LinPoint),
m_PositionJacobian(other.m_PositionJacobian),
m_MomentumJacobian(other.m_MomentumJacobian),
m_PositionAtPCA(other.m_PositionAtPCA),
m_MomentumAtPCA(other.m_MomentumAtPCA),
m_ConstTerm(other.m_ConstTerm) {}
 
LinearizedTrack::LinearizedTrack(const Acts::ActsVectorD<5>& paramsAtPCA,
								const Acts::ActsSymMatrixD<5>& parCovarianceAtPCA,
								const Acts::Vector3D& linPoint,
								const Acts::ActsMatrixD<5,3>& positionJacobian,
								const Acts::ActsMatrixD<5,3>& momentumJacobian,
								const Acts::Vector3D& positionAtPCA,
								const Acts::Vector3D& momentumAtPCA,
								const Acts::ActsVectorD<5>& constTerm) :
m_ParamsAtPCA(paramsAtPCA),
m_ParCovarianceAtPCA(parCovarianceAtPCA),
m_LinPoint(linPoint),
m_PositionJacobian(positionJacobian),
m_MomentumJacobian(momentumJacobian),
m_PositionAtPCA(positionAtPCA),
m_MomentumAtPCA(momentumAtPCA),
m_ConstTerm(constTerm) {}

LinearizedTrack& LinearizedTrack::operator= (const LinearizedTrack& other)
{
	if(this!=&other)
	{
		m_ParamsAtPCA 			= 		other.m_ParamsAtPCA;
		m_ParCovarianceAtPCA 	= 		other.m_ParCovarianceAtPCA;
		m_LinPoint 				= 		other.m_LinPoint;
		m_PositionJacobian		= 		other.m_PositionJacobian;
		m_MomentumJacobian		= 		other.m_MomentumJacobian;
		m_PositionAtPCA 		= 		other.m_PositionAtPCA;
		m_MomentumAtPCA 		= 		other.m_MomentumAtPCA;
		m_ConstTerm 			= 		other.m_ConstTerm;
	}
	return *this;
}


LinearizedTrack::~LinearizedTrack() {}
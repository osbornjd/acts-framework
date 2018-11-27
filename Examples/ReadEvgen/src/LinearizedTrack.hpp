/**
 * @class LinearizedTrack.h
 *
 * Class for linear expansion of track parameters in vicinity of vertex
 *
 * Drom ATHENA code: The measurement equation is linearized in the following way: 
 *
 * q_k= A_k (x_k - x_0k) + B_k (p_k - p_0k) + c_k
 *
 * where q_k are the parameters at perigee nearest to the lin point, 
 * x_k is the position of the vertex, p_k the track momentum at the vertex,
 * and c_k is the constant term of expansion. A_k and B_k are matrices
 * of derivatives, denoted hereafter as "positionJacobian" and "momentumJacobian"
 * respectively.

 */

#ifndef LINEARIZEDTRACK_H
#define LINEARIZEDTRACK_H

#include "Acts/Utilities/Definitions.hpp"

class LinearizedTrack
{
public:

	/// Default constructor
	LinearizedTrack();

	/// Copy constructor
	LinearizedTrack(const LinearizedTrack& other);

	/**
	 * Constructor taking perigee parameters and covariance matrix
	 * of track propagated to closest approach (PCA) of linearization point, 
	 * position and momentum Jacobian and const term.
	 */
	LinearizedTrack(const Acts::ActsVectorD<5>& paramsAtPCA,
					const Acts::ActsSymMatrixD<5>& parCovarianceAtPCA,
					const Acts::Vector3D& linPoint,
					const Acts::ActsMatrixD<5,3>& positionJacobian,
					const Acts::ActsMatrixD<5,3>& momentumJacobian,
					const Acts::Vector3D& positionAtPCA,
					const Acts::Vector3D& momentumAtPCA,
					const Acts::ActsVectorD<5>& constTerm);

	/// Assignment operator
	LinearizedTrack& operator= (const LinearizedTrack&);

	/// Clone method
	LinearizedTrack* clone() const;

	/// Default constructor
	virtual ~LinearizedTrack();

	const Acts::ActsVectorD<5>& parametersAtPCA() const
	{
		return m_ParamsAtPCA;
	}

	const Acts::ActsSymMatrixD<5>& covarianceAtPCA() const
	{
		return m_ParCovarianceAtPCA;
	}

	const Acts::Vector3D& linearizationPoint() const
	{
		return m_LinPoint;
	}

	const Acts::ActsMatrixD<5,3>& positionJacobian() const
	{
		return m_PositionJacobian;
	}

	const Acts::ActsMatrixD<5,3>& momentumJacobian() const
	{
		return m_MomentumJacobian;
	}

	const Acts::Vector3D& positionAtPCA() const
	{
		return m_PositionAtPCA;
	}

	const Acts::Vector3D& momentumAtPCA() const
	{
		return m_MomentumAtPCA;
	}

	const Acts::ActsVectorD<5>& constantTerm() const
	{
		return m_ConstTerm;
	}


private:

	 Acts::ActsVectorD<5> 		m_ParamsAtPCA;
	 Acts::ActsSymMatrixD<5> 	m_ParCovarianceAtPCA;
	 Acts::Vector3D 			m_LinPoint;
	 Acts::ActsMatrixD<5,3>		m_PositionJacobian;
	 Acts::ActsMatrixD<5,3>		m_MomentumJacobian;
	 Acts::Vector3D				m_PositionAtPCA;
	 Acts::Vector3D				m_MomentumAtPCA;
	 Acts::ActsVectorD<5>		m_ConstTerm;
};

#endif
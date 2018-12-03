
#include "FullVertexFitter.hpp"
#include "LinearizedTrackFactory.hpp"

namespace {

	struct BilloirTrack
	{
		BilloirTrack(const Acts::BoundParameters& params, LinearizedTrack* lTrack) :
			originalParams(params),
			linTrack(lTrack) 
		{}
                
        BilloirTrack(const BilloirTrack& arg) = default;
       
		const Acts::BoundParameters originalParams;
		LinearizedTrack* linTrack;
		double chi2;
		Acts::ActsMatrixD<5,3> Di_mat;
		Acts::ActsMatrixD<5,3> Ei_mat;
		Acts::ActsSymMatrixD<3> Gi_mat;
		Acts::ActsSymMatrixD<3> Bi_mat;     // Bi = Di.T * Wi * Ei
		Acts::ActsSymMatrixD<3> Ci_inv;     // Ci = (Ei.T * Wi * Ei)^-1
		Acts::Vector3D Ui_vec;     // Ui = Ei.T * Wi * dqi
		Acts::ActsSymMatrixD<3> BCi_mat;       // BCi = Bi * Ci^-1
		Acts::ActsVectorD<5> Dper;
	};

	struct BilloirVertex
	{
                BilloirVertex()
                {
                  A_mat.setZero();
                  T_vec.setZero();
                  BCB_mat.setZero();
                  BCU_vec.setZero();
                };
		double chi2;
		unsigned int ndf;
		Acts::ActsSymMatrixD<3> A_mat;              // T  = sum{Di.T * Wi * Di}
		Acts::Vector3D T_vec;              // A  = sum{Di.T * Wi * dqi}
		Acts::ActsSymMatrixD<3> BCB_mat;       // BCB = sum{Bi * Ci^-1 * Bi.T}
		Acts::Vector3D BCU_vec;       // BCU = sum{Bi * Ci^-1 * Ui}
	};



} // end anonymous namespace


FullVertexFitter::FullVertexFitter(): m_maxIterations(1){};

Vertex FullVertexFitter::fit(const std::vector<Acts::BoundParameters>& paramVector, 
								 const Acts::Vector3D& startingPoint)
{

	double chi2 = 1E10;
	double newChi2;
	unsigned int nTracks = paramVector.size();

	// Factory for linearizing tracks
	LinearizedTrackFactory linFactory;

	std::vector<BilloirTrack> billoirTracks;

	std::vector<Acts::Vector3D> trackMomenta;

	Acts::Vector3D linPoint(startingPoint);

	for(int n_iter = 0; n_iter < m_maxIterations; ++n_iter)
	{
		//billoirTracks.clear();

		BilloirVertex billoirVertex;

		int i_track = 0;
		// iterate over all tracks
		for(const auto& trackParams : paramVector)
		{
			if (i_track > 0) continue;

			if (n_iter == 0){

				double phi 		= trackParams.parameters()[Acts::ParID_t::ePHI];
				double theta 	= trackParams.parameters()[Acts::ParID_t::eTHETA];
				double qop 		= trackParams.parameters()[Acts::ParID_t::eQOP];
				trackMomenta.push_back(Acts::Vector3D(phi, theta, qop));
				std::cout << "starting momentum: phi, theta, qop:  " << phi << " " << theta << " " << qop << std::endl;
			}

			std::cout << "####### start linearization ########" << std::endl;
			LinearizedTrack* linTrack = linFactory.linearizeTrack(&trackParams, linPoint);
			std::cout << "####### end linearization ########" << std::endl;

			double d0 = linTrack->parametersAtPCA()[Acts::ParID_t::eLOC_D0];
			double z0 = linTrack->parametersAtPCA()[Acts::ParID_t::eLOC_Z0];
			double phi = linTrack->parametersAtPCA()[Acts::ParID_t::ePHI];
			double theta = linTrack->parametersAtPCA()[Acts::ParID_t::eTHETA];
			double qOverP = linTrack->parametersAtPCA()[Acts::ParID_t::eQOP];
			
			std::cout << "at PCA: d0, z0, phi, theta, qOverP = " << d0 << " " << z0 << " " << phi << " " << theta << " " <<qOverP << std::endl;

			// calculate f(V_0,p_0)  f_d0 = f_z0 = 0
			double f_phi = trackMomenta[i_track] ( 0 );
			double f_theta =trackMomenta[i_track] ( 1 );
			double f_qOverP = trackMomenta[i_track] ( 2 );

			BilloirTrack currentBilloirTrack(trackParams, linTrack);

			//calculate Dper[i]
			currentBilloirTrack.Dper[0] = d0;
			currentBilloirTrack.Dper[1] = z0;
			currentBilloirTrack.Dper[2] = phi - f_phi;
			currentBilloirTrack.Dper[3] = theta - f_theta;
			currentBilloirTrack.Dper[4] = qOverP -f_qOverP;
			
			// position jacobian (D matrix)
			Acts::ActsMatrixD<5,3> D_mat;
			D_mat = linTrack->positionJacobian();

			// momentum jacobian (E matrix)
			Acts::ActsMatrixD<5,3> E_mat;
			E_mat = linTrack->momentumJacobian();

			// cache some matrix multiplications
			Acts::ActsMatrixD<3,5> Dt_W_mat;
			Dt_W_mat.setZero();
			Acts::ActsMatrixD<3,5> Et_W_mat;
			Et_W_mat.setZero();
			Dt_W_mat = D_mat.transpose() * ( linTrack->covarianceAtPCA() );
			Et_W_mat = E_mat.transpose() * ( linTrack->covarianceAtPCA() );

			// compute billoir tracks
			currentBilloirTrack.Di_mat = D_mat;
			currentBilloirTrack.Ei_mat = E_mat;
			currentBilloirTrack.Gi_mat = Et_W_mat*E_mat;
			currentBilloirTrack.Bi_mat = Dt_W_mat * E_mat; // Di.T * Wi * Ei
			currentBilloirTrack.Ui_vec = Et_W_mat * currentBilloirTrack.Dper; // Ei.T * Wi * dqi
			currentBilloirTrack.Ci_inv = Et_W_mat * E_mat ; // (Ei.T * Wi * Ei)^-1

			// we need the inverse matrix here
			currentBilloirTrack.Ci_inv.inverse().eval();
			
			// sum up over all tracks
			billoirVertex.T_vec       += Dt_W_mat * currentBilloirTrack.Dper; // sum{Di.T * Wi * dqi}
			billoirVertex.A_mat = billoirVertex.A_mat + Dt_W_mat * D_mat ; // sum{Di.T * Wi * Di}

			// remember those results for all tracks
			currentBilloirTrack.BCi_mat = currentBilloirTrack.Bi_mat * currentBilloirTrack.Ci_inv; // BCi = Bi * Ci^-1

			// and some summed results
			billoirVertex.BCU_vec += currentBilloirTrack.BCi_mat * currentBilloirTrack.Ui_vec; // sum{Bi * Ci^-1 * Ui}
			billoirVertex.BCB_mat =  billoirVertex.BCB_mat + currentBilloirTrack.BCi_mat * currentBilloirTrack.Bi_mat.transpose() ;// sum{Bi * Ci^-1 * Bi.T}
			
			billoirTracks.push_back(currentBilloirTrack);
			i_track ++;
			

		} // end loop tracks

		// calculate delta (billoirFrameOrigin-position), might be changed by the beam-const
		Acts::Vector3D    V_del     = billoirVertex.T_vec - billoirVertex.BCU_vec;      // V_del = T-sum{Bi*Ci^-1*Ui}
		Acts::ActsSymMatrixD<3> V_wgt_mat = billoirVertex.A_mat - billoirVertex.BCB_mat;      // V_wgt = A-sum{Bi*Ci^-1*Bi.T}

		// TODO: if constraint: change accordingly

		// cov(delta_V) = V_wgt^-1
		Acts::ActsSymMatrixD<3> cov_delta_V_mat = V_wgt_mat.inverse().eval() ;

		// delta_V = cov_(delta_V) * V_del;
		Acts::Vector3D delta_V = cov_delta_V_mat * V_del;

		linPoint += delta_V;


	} // end loop iterations

	Vertex v;
	v.setPosition(linPoint);

	return v;



}
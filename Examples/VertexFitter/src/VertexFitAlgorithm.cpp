// This file is part of the Acts project.
//
// Copyright (C) 2016-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "VertexFitAlgorithm.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"

#include <tbb/tbb.h>

#include "Acts/Vertexing/LinearizedTrack.hpp"
#include "Acts/Vertexing/LinearizedTrackFactory.hpp"
#include "Acts/Vertexing/Vertex.hpp"

FWE::VertexFitAlgorithm::VertexFitAlgorithm(const Config&        cfg,
                                            Acts::Logging::Level level)
  : FW::BareAlgorithm("VertexFit", level), m_cfg(cfg)
{
}

/// @brief Algorithm that receives an Evgen input event, runs over all
/// vertices and smears corresponding tracks.
/// Track collections belonging to a certain vertex (truth-based vertex finder
/// emuluation)
/// are then passed to vertex fitter to fit vertex position.
FW::ProcessCode
FWE::VertexFitAlgorithm::execute(const FW::AlgorithmContext& context) const
{

  const double eta_cut = 3.0;

  /// Define parameter for pt-dependent IP resolution
  /// of the form sigma_d/z(p_t[GeV]) = A*exp(-B*p_t[GeV]) + C
  const double ipResA = 100.7439 * Acts::units::_um;
  const double ipResB = 0.23055;
  const double ipResC = 20. * Acts::units::_um;

  /// Create and fill input event
  const std::vector<FW::Data::SimVertex<>>* inputEvent = nullptr;
  if (context.eventStore.get(m_cfg.collection, inputEvent)
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  /// Define perigee surface center coordinates
  const Acts::Vector3D surfaceCenter(0., 0., 0.);

  std::shared_ptr<Acts::PerigeeSurface> perigeeSurface
      = Acts::Surface::makeShared<Acts::PerigeeSurface>(surfaceCenter);

  /// Set up stepper
  Acts::EigenStepper<Acts::ConstantBField> stepper(m_cfg.bField);

  /// Set up propagator with void navigator
  Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>> propagator(
      stepper);

  /// Set up propagator options
  Acts::PropagatorOptions<> pOptions(context.geoContext, context.magFieldContext);

  /// Create random number generator and spawn gaussian distribution
  FW::RandomEngine rng = m_cfg.randomNumberSvc->spawnGenerator(context);

  /// typedefs for simplicity
  using BoundParamsVector = std::vector<Acts::BoundParameters>;
  using InputTrackVector  = std::vector<InputTrack>;

  /// Vector to store smrdTracksAtVtx for all vertices of event
  std::vector<BoundParamsVector> smrdTrackCollection;

  std::vector<InputTrackVector> inputTrackCollection;

  /// Vector to store true vertices positions
  std::vector<Acts::Vector3D> trueVertices;

  /// Start looping over all vertices in current event
  for (auto& vtx : (*inputEvent)) {

    /// Vector to store smeared tracks at current vertex
    BoundParamsVector smrdTracksAtVtx;
    InputTrackVector  inputTrackVector;

    /// Iterate over all particle emerging from current vertex
    for (auto const& particle : vtx.out) {

      const Acts::Vector3D& ptclMom = particle.momentum();
      /// Calculate pseudo-rapidity
      const double eta = Acts::VectorHelpers::eta(ptclMom);
      /// Only charged particles for |eta| < eta_cut
      if (particle.q() != 0 && std::abs(eta) < eta_cut) {
        /// Define start track params
        Acts::CurvilinearParameters start(
            nullptr, particle.position(), ptclMom, particle.q());

        /// Run propagator
        auto result
            = propagator.propagate(start, *perigeeSurface, pOptions);

        if (result.ok()) {

          const auto& propRes = *result;

          // get perigee parameters
          const auto& perigeeParameters = propRes.endParameters->parameters();

          // apply d0 and z0 cuts
          if (std::abs(perigeeParameters[0]) > 10 * Acts::units::_mm
              || std::abs(perigeeParameters[1]) > 200 * Acts::units::_mm) {
            continue;
          }

          /// Calculate pt-dependent IP resolution
          const double particlePt
              = Acts::VectorHelpers::perp(ptclMom) / Acts::units::_GeV;
          const double ipRes = ipResA * std::exp(-ipResB * particlePt) + ipResC;

          /// except for IP resolution, following variances are rough guesses
          /// Gaussian distribution for IP resolution
          FW::GaussDist gaussDist_IP(0., ipRes);
          /// Gaussian distribution for angular resolution
          FW::GaussDist gaussDist_angular(0., 0.1);
          /// Gaussian distribution for q/p (momentum) resolution
          FW::GaussDist gaussDist_qp(0., 0.1 * perigeeParameters[4]);

          double rn_d0 = gaussDist_IP(rng);
          double rn_z0 = gaussDist_IP(rng);
          double rn_ph = gaussDist_angular(rng);
          double rn_th = gaussDist_angular(rng);
          double rn_qp = gaussDist_qp(rng);

          double smrd_d0    = perigeeParameters[0] + rn_d0;
          double smrd_z0    = perigeeParameters[1] + rn_z0;
          double smrd_phi   = perigeeParameters[2] + rn_ph;
          double smrd_theta = perigeeParameters[3] + rn_th;
          double srmd_qp    = perigeeParameters[4] + rn_qp;

          /// smearing can bring theta out of range ->close to beam line ->
          /// discard
          if (smrd_theta < 0 || smrd_theta > M_PI) {
            continue;
          }

          double new_eta = -log(tan(smrd_theta / 2));
          if (std::abs(new_eta) > eta_cut) continue;

          Acts::TrackParametersBase::ParVector_t paramVec;
          paramVec << smrd_d0, smrd_z0, smrd_phi, smrd_theta, srmd_qp;

          /// Fill vector of smeared tracks
          std::unique_ptr<Acts::ActsSymMatrixD<5>> covMat
              = std::make_unique<Acts::ActsSymMatrixD<5>>();
          covMat->setZero();
          (*covMat).diagonal() << rn_d0 * rn_d0, rn_z0 * rn_z0, rn_ph * rn_ph,
              rn_th * rn_th, rn_qp * rn_qp;

          Acts::BoundParameters currentBoundParams(context.geoContext,
              std::move(covMat), paramVec, perigeeSurface);

          smrdTracksAtVtx.push_back(currentBoundParams);

          inputTrackVector.push_back(InputTrack(currentBoundParams));
        }
      }
    }

    if (!smrdTracksAtVtx.empty()) {
      smrdTrackCollection.push_back(smrdTracksAtVtx);
      inputTrackCollection.push_back(inputTrackVector);

      /// Store true vertex position
      trueVertices.push_back(vtx.position);
    }
  }

  assert(smrdTrackCollection.size() == trueVertices.size());

  ACTS_INFO("Total number of vertices in event: " << trueVertices.size());

  tbb::concurrent_vector<Acts::Vertex<InputTrack>> fittedVerticesVector;

  // Vertex constraint
  Acts::Vertex<InputTrack> myConstraint;
  Acts::ActsSymMatrixD<3>  myCovMat;
  // Set some arbitrary large values on the main diagonal
  myCovMat(0, 0) = 30.;
  myCovMat(1, 1) = 30.;
  myCovMat(2, 2) = 30.;
  myConstraint.setCovariance(std::move(myCovMat));
  myConstraint.setPosition(Acts::Vector3D(0, 0, 0));

  // Vertex fitter options
  Acts::VertexFitterOptions<InputTrack> vfOptions(context.geoContext, context.magFieldContext);

  Acts::VertexFitterOptions<InputTrack> vfOptionsConstr(
          context.geoContext, context.magFieldContext, myConstraint);

  /// in-event parallel vertex fitting
  tbb::parallel_for(
      tbb::blocked_range<size_t>(0, smrdTrackCollection.size()),
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t vertex_idx = r.begin(); vertex_idx != r.end();
             ++vertex_idx) {

          BoundParamsVector& currentParamVectorAtVtx
              = smrdTrackCollection[vertex_idx];

          if (currentParamVectorAtVtx.size() > 1) {

            Acts::Vertex<InputTrack> fittedVertex;

            if (!m_cfg.doConstrainedFit) {
              fittedVertex = m_cfg.vertexFitter->fit(
                  inputTrackCollection[vertex_idx], propagator, vfOptions);
            } else {
              fittedVertex = m_cfg.vertexFitter->fit(
                  inputTrackCollection[vertex_idx], propagator, vfOptionsConstr);
            }

            Acts::Vector3D currentTrueVtx = trueVertices[vertex_idx];
            Acts::Vector3D diffVtx = currentTrueVtx - fittedVertex.position();

            ACTS_INFO("Event: " << context.eventNumber << ", vertex "
                                << vertex_idx
                                << " with "
                                << currentParamVectorAtVtx.size()
                                << " tracks");
            ACTS_INFO("True Vertex: "
                      << "("
                      << currentTrueVtx[0]
                      << ","
                      << currentTrueVtx[1]
                      << ","
                      << currentTrueVtx[2]
                      << ")");
            ACTS_INFO("Fitted Vertex: "
                      << "("
                      << fittedVertex.position()[0]
                      << ","
                      << fittedVertex.position()[1]
                      << ","
                      << fittedVertex.position()[2]
                      << ")");

            fittedVerticesVector.push_back(fittedVertex);
          }
        }
      });

  return FW::ProcessCode::SUCCESS;
}

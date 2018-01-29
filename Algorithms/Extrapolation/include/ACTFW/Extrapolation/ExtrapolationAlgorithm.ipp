// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Logger.hpp"

template <class T, class BoundT>
FW::ProcessCode
FW::ExtrapolationAlgorithm::executeTestT(
    RandomEngine&                            rEngine,
    UniformDist&                             uDist,
    Acts::ExtrapolationCell<T>&              ecc,
    barcode_type                             barcode,
    int                                      pdgcode,
    std::vector<Acts::ExtrapolationCell<T>>& eCells,
    std::vector<Acts::ProcessVertex>&        simulated,
    FW::DetectorData<geo_id_value,
                     std::pair<std::unique_ptr<const T>, barcode_type>>* dData)
    const
{

  // setup the extrapolation how you'd like it
  // Acts::ExtrapolationCell<T> ecc(startParameters);
  auto& startParameters = (*ecc.startParameters);

  // ecc.setParticleHypothesis(m_cfg.particleType);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
  ecc.searchMode = m_cfg.searchMode;
  // now set the behavioral bits
  if (m_cfg.collectSensitive)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectSensitive);
  if (m_cfg.collectPassive)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectPassive);
  if (m_cfg.collectBoundary)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectBoundary);
  if (m_cfg.collectMaterial)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);
  if (m_cfg.sensitiveCurvilinear) ecc.sensitiveCurvilinear = true;

  // we set the particle hypothesis depending od pdg
  // currently, only elctrons, muons, pions are supported
  if (abs(pdgcode) == 11) {
    // electron
    ecc.setParticleType(Acts::ParticleType::electron);
  } else if (abs(pdgcode) == 13) {
    // muon
    ecc.setParticleType(Acts::ParticleType::muon);
  } else if (m_cfg.randomNumbers) {
    // pion
    ecc.setParticleType(Acts::ParticleType::pion);
    // sample free path in terms of nuclear interaction length
    double al           = 1.;  // scaling here
    ecc.materialLimitL0 = -log(uDist(rEngine)) * al;
    ecc.addConfigurationMode(Acts::ExtrapolationMode::StopWithMaterialLimitL0);
  }

  // force a stop in the extrapoaltion mode
  if (m_cfg.pathLimit > 0.) {
    ecc.pathLimit = m_cfg.pathLimit;
    ecc.addConfigurationMode(Acts::ExtrapolationMode::StopWithPathLimit);
  }
  // screen output
  ACTS_DEBUG("===> forward extrapolation - collecting information <<===");

  // theta / phi
  auto   sPosition = startParameters.position();
  double sTheta    = startParameters.momentum().theta();
  double sPhi      = startParameters.momentum().theta();
  double tX0       = 0.;
  double tL0       = 0.;
  // material steps to be  filled
  std::vector<Acts::MaterialStep> materialSteps;

  // call the extrapolation engine
  Acts::ExtrapolationCode eCode = m_cfg.extrapolationEngine->extrapolate(ecc);
  if (eCode.isFailure()) {
    ACTS_WARNING("Extrapolation failed.");
    return FW::ProcessCode::ABORT;
  }

  // create the detector hits data
  if (dData && m_cfg.simulatedHitsCollection != "") {
    /// loop over steps and get the sensitive
    for (auto& es : ecc.extrapolationSteps) {
      // check if you have parameters
      if (es.parameters) {
        // get the surface
        const Acts::Surface& sf = es.parameters->referenceSurface();
        // check if you have material
        if (es.configuration.checkMode(
                Acts::ExtrapolationMode::CollectSensitive)
            && dData) {
          // fill the appropriate vector
          geo_id_value volumeID
              = sf.geoID().value(Acts::GeometryID::volume_mask);
          geo_id_value layerID = sf.geoID().value(Acts::GeometryID::layer_mask);
          geo_id_value moduleID
              = sf.geoID().value(Acts::GeometryID::sensitive_mask);
          // search and/or insert - we need to clone as the ECC will be wrritten
          auto parcpptr = std::unique_ptr<const T>(es.parameters->clone());
          std::pair<std::unique_ptr<const T>, barcode_type> eHit(
              std::move(parcpptr), barcode);
          FW::Data::insert(
              *dData, volumeID, layerID, moduleID, std::move(eHit));
        }
      }
    }
    // also check the end parameters
    if (ecc.endParameters) {
      // get the endSurface
      const Acts::Surface& esf = ecc.endParameters->referenceSurface();
      // check if sensitive
      if (esf.associatedDetectorElement()) {
        // fill the appropriate vector
        geo_id_value volumeID
            = esf.geoID().value(Acts::GeometryID::volume_mask);
        geo_id_value layerID = esf.geoID().value(Acts::GeometryID::layer_mask);
        geo_id_value moduleID
            = esf.geoID().value(Acts::GeometryID::sensitive_mask);
        // search and/or insert
        std::pair<std::unique_ptr<const T>, barcode_type> eHit(
            std::move(ecc.endParameters), barcode);
        FW::Data::insert(*dData, volumeID, layerID, moduleID, std::move(eHit));
      }
    }
  }
  // we allow for children
  if (ecc.interactionVertices.size()) {
    // loop over the vertices
    for (auto& vtx : ecc.interactionVertices) {
      ACTS_VERBOSE("Hadronic interaction vertex with "
                   << vtx.outgoingParticles().size()
                   << " ougoing particles");

      // vertex is outside cut
      if (vtx.position().perp() > m_cfg.maxD0
          || fabs(vtx.position().z()) > m_cfg.maxZ0) {
        ACTS_VERBOSE("Process vertex is outside the IP cut. Skipping.");
        continue;
      }

      // the asspcoated perigee for this vertex
      Acts::PerigeeSurface surface(vtx.position());
      // count the successful daughters
      //
      // Add children to the vector of children
      int                                   daughter = 0;
      std::vector<Acts::ParticleProperties> pIngoing = {};
      std::vector<Acts::ParticleProperties> pOutgoing;

      // create only for
      for (auto& op : vtx.outgoingParticles()) {
        ACTS_VERBOSE("Particle with charge = " << op.charge());
        double d0    = 0.;
        double z0    = 0.;
        double phi   = op.momentum().phi();
        double theta = op.momentum().theta();

        // ignore charged particles for the moment
        if (op.charge() == 0. || op.momentum().eta() > m_cfg.maxEta
            || op.momentum().perp() < m_cfg.minPt)
          continue;

        // create a daughter barcode in relation to the mother
        barcode_type daughterBc = op.barcode();
        if (m_cfg.barcodeSvc) {
          // get vertex, primary and generation
          barcode_type vtx  = m_cfg.barcodeSvc->vertex(barcode);
          barcode_type prim = m_cfg.barcodeSvc->primary(barcode);
          barcode_type gen  = m_cfg.barcodeSvc->generation(barcode);
          // increase the generation
          ++gen;
          // generate a new particle with a dummy process 211
          // @todo update process after final Fatras integration
          daughterBc
              = m_cfg.barcodeSvc->generate(vtx, prim, gen, ++daughter, 1);
          pOutgoing.push_back(Acts::ParticleProperties(
              op.momentum(), op.mass(), op.charge(), op.pdgID(), daughterBc));
        }

        double qop = op.charge() / op.momentum().mag();
        // parameters
        Acts::ActsVectorD<5> pars;
        pars << d0, z0, phi, theta, qop;
        // some screen output
        std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;
        BoundT boundparameters(std::move(cov), std::move(pars), surface);

        // prepare hits for charged  paramters
        Acts::ExtrapolationCell<T> ecg(boundparameters);
        if (executeTestT<T, BoundT>(rEngine,
                                    uDist,
                                    ecg,
                                    daughterBc,
                                    op.pdgID(),
                                    eCells,
                                    simulated,
                                    dData)
            != FW::ProcessCode::SUCCESS)
          ACTS_WARNING(
              "Test of neutral parameter extrapolation did not succeed.");
      }
      // add the vertex tot he simulated
      simulated.push_back(Acts::ProcessVertex(vtx.position(),
                                              vtx.interactionTime(),
                                              vtx.processType(),
                                              pIngoing,
                                              pOutgoing));
    }
  }

  /// fill the ecc step into the container at the end
  eCells.push_back(std::move(ecc));
  // return success
  return FW::ProcessCode::SUCCESS;
}

// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"
#include "PropagationAlgorithm.hpp"

namespace po = boost::program_options;
namespace au = Acts::units;

namespace FW {

namespace Options {

  /// @brief ExtrapolationAlgorithm options
  ///
  /// @tparam aopt_t Type of the options class from boost
  template <typename aopt_t>
  void
  addPropagationOptions(aopt_t& opt)
  {
    opt.add_options()(
        "prop-debug",
        po::value<bool>()->default_value(false),
        "Run in debug mode, will create propagation screen output.")(
        "prop-step-collection",
        po::value<std::string>()->default_value("propagation-steps"),
        "Propgation step collection.")(
        "prop-stepper",
        po::value<int>()->default_value(1),
        "Propgation type: 0 (StraightLine), 1 (Eigen), 2 (Atlas).")(
        "prop-mode",
        po::value<int>()->default_value(0),
        "Propgation modes: 0 (inside-out), 1 (surface to surface).")(
        "prop-cov",
        po::value<bool>()->default_value(false),
        "Propagate (random) test covariances.")(
        "prop-energyloss",
        po::value<bool>()->default_value(true),
        "Apply energy loss correction - in extrapolation mode only.")(
        "prop-scattering",
        po::value<bool>()->default_value(true),
        "Apply scattering correction - in extrapolation mode only.")(
        "prop-record-material",
        po::value<bool>()->default_value(true),
        "Record the material interaction and - in extrapolation mode only.")(
        "prop-material-collection",
        po::value<std::string>()->default_value("propagation-material"),
        "Propagation material collection.")(
        "prop-ntests",
        po::value<size_t>()->default_value(1000),
        "Number of tests performed.")(
        "prop-d0-sigma",
        po::value<double>()->default_value(15. * au::_um),
        "Sigma of the transverse impact parameter [in mm].")(
        "prop-z0-sigma",
        po::value<double>()->default_value(55. * au::_mm),
        "Sigma of the longitudinal impact parameter [in mm].")(
        "prop-phi-sigma",
        po::value<double>()->default_value(0.001),
        "Sigma of the azimuthal angle [in rad].")(
        "prop-theta-sigma",
        po::value<double>()->default_value(0.001),
        "Sigma of the polar angle [in rad].")(
        "prop-qp-sigma",
        po::value<double>()->default_value(0.0001),
        "Sigma of the signed inverse momentum [in GeV^{-1}].")(
        "prop-t-sigma",
        po::value<double>()->default_value(0.000001),
        "Sigma of the time parameter [in s].")(
        "prop-corr-offd",
        po::value<read_range>()->multitoken()->default_value({}),
        "The 15 off-diagonal correlation rho(d0,z0), rho(d0,phi), [...], "
        "rho(qop,t)")(
        "prop-phi-range",
        po::value<read_range>()->multitoken()->default_value({-M_PI, M_PI}),
        "Azimutal angle phi range for proprapolated tracks.")(
        "prop-eta-range",
        po::value<read_range>()->multitoken()->default_value({-4., 4.}),
        "Pseudorapidity range for proprapolated tracks.")(
        "prop-pt-range",
        po::value<read_range>()->multitoken()->default_value(
            {100. * au::_MeV, 100. * au::_GeV}),
        "Transverse momentum range for proprapolated tracks [in GeV].")(
        "prop-max-stepsize",
        po::value<double>()->default_value(10 * au::_m),
        "Maximum step size for the propagation [in mm].")(
        "prop-pt-loopers",
        po::value<double>()->default_value(0.3 * au::_GeV),
        "Transverse momentum below which loops are being detected [in GeV].");
  }

  /// read the evgen options and return a Config file
  ///
  /// @tparam vmap_t is the Type of the Parameter map to be read out
  /// @tparam bfield_t is the Type of the Magnetic field
  ///
  /// @param vm is the parameter map for the options
  /// @param magField is the magnetic field objects as shared pointer
  /// @param tGeometry is the tracking geometry object
  ///
  /// @returns a Config object for the ExtrapolationAlgorithm
  template <typename vmap_t, typename propagator_t>
  typename FW::PropagationAlgorithm<propagator_t>::Config
  readPropagationConfig(const vmap_t& vm, propagator_t propagator)
  {

    typename FW::PropagationAlgorithm<propagator_t>::Config pAlgConfig(
        std::move(propagator));

    read_range iphir = vm["prop-phi-range"].template as<read_range>();
    read_range ietar = vm["prop-eta-range"].template as<read_range>();
    read_range iptr  = vm["prop-pt-range"].template as<read_range>();

    /// Material interaction behavior
    pAlgConfig.energyLoss         = vm["prop-energyloss"].template as<bool>();
    pAlgConfig.multipleScattering = vm["prop-scattering"].template as<bool>();
    pAlgConfig.recordMaterialInteractions
        = vm["prop-record-material"].template as<bool>();

    /// Create the config for the Extrapoaltion algorithm
    pAlgConfig.debugOutput = vm["prop-debug"].template as<bool>();
    pAlgConfig.ntests      = vm["prop-ntests"].template as<size_t>();
    pAlgConfig.mode        = vm["prop-mode"].template as<int>();
    pAlgConfig.d0Sigma    = vm["prop-d0-sigma"].template as<double>() * au::_mm;
    pAlgConfig.z0Sigma    = vm["prop-z0-sigma"].template as<double>() * au::_mm;
    pAlgConfig.phiSigma   = vm["prop-phi-sigma"].template as<double>();
    pAlgConfig.thetaSigma = vm["prop-theta-sigma"].template as<double>();
    pAlgConfig.qpSigma    = vm["prop-qp-sigma"].template as<double>();
    pAlgConfig.tSigma     = vm["prop-t-sigma"].template as<double>();

    pAlgConfig.phiRange = {iphir[0], iphir[1]};
    pAlgConfig.etaRange = {ietar[0], ietar[1]};
    pAlgConfig.ptRange  = {iptr[0] * au::_GeV, iptr[1] * au::_GeV};
    pAlgConfig.ptLoopers
        = vm["prop-pt-loopers"].template as<double>() * au::_GeV;
    pAlgConfig.maxStepSize
        = vm["prop-max-stepsize"].template as<double>() * au::_mm;

    pAlgConfig.propagationStepCollection
        = vm["prop-step-collection"].template as<std::string>();
    pAlgConfig.propagationMaterialCollection
        = vm["prop-material-collection"].template as<std::string>();

    /// The covariance transport
    if (vm["prop-cov"].template as<bool>()) {
      /// Set the covariance transport to true
      pAlgConfig.covarianceTransport = true;
      /// Set the covariance matrix
      pAlgConfig.covariances(Acts::ParDef::eLOC_D0, Acts::ParDef::eLOC_D0)
          = pAlgConfig.d0Sigma * pAlgConfig.d0Sigma;
      pAlgConfig.covariances(Acts::ParDef::eLOC_Z0, Acts::ParDef::eLOC_Z0)
          = pAlgConfig.z0Sigma * pAlgConfig.z0Sigma;
      pAlgConfig.covariances(Acts::ParDef::ePHI, Acts::ParDef::ePHI)
          = pAlgConfig.phiSigma * pAlgConfig.phiSigma;
      pAlgConfig.covariances(Acts::ParDef::eTHETA, Acts::ParDef::eTHETA)
          = pAlgConfig.thetaSigma * pAlgConfig.thetaSigma;
      pAlgConfig.covariances(Acts::ParDef::eQOP, Acts::ParDef::eQOP)
          = pAlgConfig.qpSigma * pAlgConfig.qpSigma;
      pAlgConfig.covariances(Acts::ParDef::eT, Acts::ParDef::eT)
          = pAlgConfig.tSigma * pAlgConfig.tSigma;

      // Read if the offdiagonal parameters have been read
      auto readOffd = vm["prop-corr-offd"].template as<read_range>();
      // Only if they are properly defined, assign
      if (readOffd.size() == 15) {
        pAlgConfig.correlations(Acts::ParDef::eLOC_D0, Acts::ParDef::eLOC_Z0)
            = readOffd[0];
        pAlgConfig.correlations(Acts::ParDef::eLOC_D0, Acts::ParDef::ePHI)
            = readOffd[1];
        pAlgConfig.correlations(Acts::ParDef::eLOC_D0, Acts::ParDef::eTHETA)
            = readOffd[2];
        pAlgConfig.correlations(Acts::ParDef::eLOC_D0, Acts::ParDef::eQOP)
            = readOffd[3];
        pAlgConfig.correlations(Acts::ParDef::eLOC_D0, Acts::ParDef::eT)
            = readOffd[4];
        pAlgConfig.correlations(Acts::ParDef::eLOC_Z0, Acts::ParDef::ePHI)
            = readOffd[5];
        pAlgConfig.correlations(Acts::ParDef::eLOC_Z0, Acts::ParDef::eTHETA)
            = readOffd[6];
        pAlgConfig.correlations(Acts::ParDef::eLOC_Z0, Acts::ParDef::eQOP)
            = readOffd[7];
        pAlgConfig.correlations(Acts::ParDef::eLOC_Z0, Acts::ParDef::eT)
            = readOffd[8];
        pAlgConfig.correlations(Acts::ParDef::ePHI, Acts::ParDef::eTHETA)
            = readOffd[9];
        pAlgConfig.correlations(Acts::ParDef::ePHI, Acts::ParDef::eQOP)
            = readOffd[10];
        pAlgConfig.correlations(Acts::ParDef::ePHI, Acts::ParDef::eT)
            = readOffd[11];
        pAlgConfig.correlations(Acts::ParDef::eTHETA, Acts::ParDef::eQOP)
            = readOffd[12];
        pAlgConfig.correlations(Acts::ParDef::eTHETA, Acts::ParDef::eT)
            = readOffd[13];
        pAlgConfig.correlations(Acts::ParDef::eQOP, Acts::ParDef::eT)
            = readOffd[14];
      } else {
        /// Some pre-defined values (non-trivial helical correlations)
        pAlgConfig.correlations(Acts::ParDef::eLOC_D0, Acts::ParDef::ePHI)
            = -0.8;
        pAlgConfig.correlations(Acts::ParDef::eLOC_D0, Acts::ParDef::eQOP)
            = -0.3;
        pAlgConfig.correlations(Acts::ParDef::eLOC_Z0, Acts::ParDef::eTHETA)
            = -0.8;
        pAlgConfig.correlations(Acts::ParDef::ePHI, Acts::ParDef::eQOP) = 0.4;
      }
    }

    return pAlgConfig;
  }

}  // namespace Options
}  // namespace FW

// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "PropagationAlgorithm.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  /// @brief ExtrapolationAlgorithm options
  ///
  /// @tparam aopt_t Type of the options class from boost
  template <typename aopt_t>
  void
  addPropagationOptions(aopt_t& opt)
  {
    opt.add_options()("prop-output",
                      po::value<bool>()->default_value(true),
                      "Run with output mode on, will create an output file.")(
        "prop-debug",
        po::value<bool>()->default_value(false),
        "Run in debug mode, will create propagation screen output.")(
        "prop-stepper",
        po::value<int>()->default_value(1),
        "Propgation type: 0 (StraightLine), 1 (Eigen), 2 (Atlas).")(
        "prop-mode",
        po::value<int>()->default_value(0),
        "Propgation modes: 0 (inside-out), 1 (surface to surface).")(
        "prop-ext",
        po::value<bool>()->default_value(false),
        "Run in extrapolation mode, i.e. with Navigator.")(
        "ext-energyloss",
        po::value<bool>()->default_value(true),
        "Apply energy loss correction - in extrapolation mode only.")(
        "ext-scattering",
        po::value<bool>()->default_value(true),
        "Apply scattering correction - in extrapolation mode only.")(
        "prop-ntests",
        po::value<size_t>()->default_value(1000),
        "Number of tests performed.")(
        "prop-d0-sigma",
        po::value<double>()->default_value(15. * Acts::units::_um),
        "Sigma of the transverse impact parameter.")(
        "prop-z0-sigma",
        po::value<double>()->default_value(55. * Acts::units::_mm),
        "Sigma of the longitudinal impact parameter.")(
        "prop-phi-range",
        po::value<read_range>()->multitoken()->default_value({-M_PI, M_PI}),
        "Azimutal angle phi range for proprapolated tracks.")(
        "prop-eta-range",
        po::value<read_range>()->multitoken()->default_value({-4., 4.}),
        "Pseudorapidity range for proprapolated tracks.")(
        "prop-pt-range",
        po::value<read_range>()->multitoken()->default_value(
            {100. * Acts::units::_MeV, 100. * Acts::units::_GeV}),
        "Transverse momentum range for proprapolated tracks [in GeV].");
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

    /// Create the config for the Extrapoaltion algorithm
    pAlgConfig.debugOutput = vm["prop-debug"].template as<bool>();
    pAlgConfig.ntests      = vm["prop-ntests"].template as<size_t>();
    pAlgConfig.mode        = vm["prop-mode"].template as<int>();
    pAlgConfig.d0Sigma     = vm["prop-d0-sigma"].template as<double>();
    pAlgConfig.z0Sigma     = vm["prop-z0-sigma"].template as<double>();
    pAlgConfig.phiRange    = {iphir[0], iphir[1]};
    pAlgConfig.etaRange    = {ietar[0], ietar[1]};
    pAlgConfig.ptRange
        = {iptr[0] * Acts::units::_GeV, iptr[1] * Acts::units::_GeV};

    return pAlgConfig;
  }

}  // namespace Options
}  // namespace FW

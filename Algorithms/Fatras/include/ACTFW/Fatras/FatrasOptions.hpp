// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_OPTIONS_FATRASOPTIONS_HPP
#define ACTFW_OPTIONS_FATRASOPTIONS_HPP

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Fatras/EnergyLossSampler.hpp"
#include "Fatras/HadronicInteractionParametricSampler.hpp"
#include "Fatras/MultipleScatteringSamplerHighland.hpp"
#include "FatrasAlgorithm.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <typename AOPT>
  void
  addFatrasOptions(AOPT& opt)
  {
    opt.add_options()("fatras-energyloss",
                      po::value<bool>()->default_value(true),
                      "Apply energy loss correction.")(
        "fatras-scattering",
        po::value<bool>()->default_value(true),
        "Apply scattering correction.")("fatras-hadronic",
                                        po::value<bool>()->default_value(false),
                                        "Apply hadronic interaction.")(
        "fatras-min-pt",
        po::value<double>()->default_value(100. * Acts::units::_MeV),
        "Minimum transverse momentum for particle handling.")(
        "fatras-max-vr",
        po::value<double>()->default_value(1000. * Acts::units::_mm),
        "Maximal vertex radius for particle creation.")(
        "fatras-max-vz",
        po::value<double>()->default_value(3000. * Acts::units::_mm),
        "Maximal longitudinal vertex position for particle creation.")(
        "fatras-max-eta",
        po::value<double>()->default_value(5.),
        "Maximal (absolute) pseudo-rapidity for particle handling.")(
        "fatras-sim-particles",
        po::value<std::string>()->default_value("FatrasParticles"),
        "The collection of simulated particles.")(
        "fatras-sim-hits",
        po::value<std::string>()->default_value("FatrasHits"),
        "The collection of simulated hits");
  }

  /// read the evgen options and return a Config file
  template <typename AMAP, typename Propagator_type>
  typename FatrasAlgorithm<Propagator_type>::Config
  readFatrasConfig(const AMAP& vm, Propagator_type propagator)
  {
    // create a config
    typename FatrasAlgorithm<Propagator_type>::Config fatrasConfig(
        std::move(propagator));

    //    // read configuration from map
    //    // phsics - energy loss
    //    if (vm["fatras-energyloss"].template as<bool>()){
    //      // EnergyLossSampler
    //      using eLossSampler      =
    //      Fatras::EnergyLossSampler<FW::RandomEngine>;
    //      auto eLossConfig        = eLossSampler::Config();
    //      eLossConfig.scalorMOP   = 0.745167;  // validated with geant4
    //      eLossConfig.scalorSigma = 0.68925;   // validated with geant4
    //      auto eLSampler          =
    //      std::make_shared<eLossSampler>(eLossConfig);
    //      // eLSampler->setLogger(Acts::getDefaultLogger("ELoss", loglevel));
    //      fatrasConfig.energyLossSampler   = eLSampler;
    //    }
    //    // physics - scattering
    //    if (vm["fatras-scattering"].template as<bool>()){
    //      // the scattering sampler
    //      using MSCSampler
    //          = Fatras::MultipleScatteringSamplerHighland<FW::RandomEngine>;
    //      auto mscConfig  = MSCSampler::Config();
    //      auto mscSampler = std::make_shared<MSCSampler>(mscConfig);
    //      // set it to the config
    //      fatrasConfig.multipleScatteringSampler = mscSampler;
    //    }
    //    // physics - scattering
    //    if (vm["fatras-hadronic"].template as<bool>()){
    //      // Hadronic interaction sampler
    //      using hadIntSampler
    //          =
    //          Fatras::HadronicInteractionParametricSampler<FW::RandomEngine>;
    //      auto hiConfig  = hadIntSampler::Config();
    //      auto hiSampler = std::make_shared<hadIntSampler>(hiConfig);
    //      // set it to the config
    //      fatrasConfig.hadronicInteractionSampler = hiSampler;
    //    }
    //    // parameter
    //    fatrasConfig.minPt       = vm["fatras-min-pt"].template as<double>();
    //    fatrasConfig.maxVr       = vm["fatras-max-vr"].template as<double>();
    //    fatrasConfig.maxVz       = vm["fatras-max-vz"].template as<double>();
    //    fatrasConfig.maxEta      = vm["fatras-max-eta"].template as<double>();
    // set the collections
    fatrasConfig.simulatedHitCollection
        = vm["fatras-sim-hits"].template as<std::string>();
    fatrasConfig.simulatedParticleCollection
        = vm["fatras-sim-particles"].template as<std::string>();
    // and return the config
    return fatrasConfig;
  }
}
}

#endif  // ACTFW_OPTIONS_FATRASOPTIONS_HPP

// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Plugins/Pythia8/Generator.hpp"
#include "ACTFW/Plugins/Pythia8/GeneratorOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/EvgenReader.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"

template <typename vmap_t>
std::string
setupEvgenInput(vmap_t&                               vm,
                FW::Sequencer&                        sequencer,
                std::shared_ptr<FW::BarcodeSvc>       barcodeSvc,
                std::shared_ptr<FW::RandomNumbersSvc> randomNumberSvc)
{
  // Read the standard options
  auto logLevel = FW::Options::readLogLevel<vmap_t>(vm);
  auto nEvents  = FW::Options::readNumberOfEvents<vmap_t>(vm);

  // Read the particle gun configs
  auto particleGunCfg = FW::Options::readParticleGunConfig<vmap_t>(vm);

  // Define which
  auto evgenInput = vm["fatras-evgen-input"].template as<std::string>();

  // Check if the particle gun is to be used
  if (evgenInput == "gun") {
    // Are running in particle gun mode
    particleGunCfg.barcodeSvc      = barcodeSvc;
    particleGunCfg.randomNumberSvc = randomNumberSvc;
    particleGunCfg.nEvents         = nEvents;
    auto particleGun = std::make_shared<FW::ParticleGun>(particleGunCfg);
    // Add particle gun as a reader
    sequencer.addReaders({particleGun});

  } else {
    // Read the pythia8 configs
    auto pythia8Configs = FW::Options::readPythia8Config<vmap_t>(vm);
    pythia8Configs.first.randomNumberSvc  = randomNumberSvc;
    pythia8Configs.second.randomNumberSvc = randomNumberSvc;
    // The hard scatter generator
    auto hsPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
        pythia8Configs.first,
        Acts::getDefaultLogger("HardScatterPythia8Generator", logLevel));
    // The pileup generator
    auto puPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
        pythia8Configs.second,
        Acts::getDefaultLogger("PileUpPythia8Generator", logLevel));
    // Create the barcode service
    FW::BarcodeSvc::Config barcodeSvcCfg;
    auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
        barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));
    // Read the evgen config & set the missing parts
    auto readEvgenCfg                   = FW::Options::readEvgenConfig(vm);
    readEvgenCfg.hardscatterEventReader = hsPythiaGenerator;
    readEvgenCfg.pileupEventReader      = puPythiaGenerator;
    readEvgenCfg.randomNumberSvc        = randomNumberSvc;
    readEvgenCfg.barcodeSvc             = barcodeSvc;
    readEvgenCfg.nEvents                = nEvents;
    // create the read Algorithm
    auto readEvgen = std::make_shared<FW::EvgenReader>(
        readEvgenCfg, Acts::getDefaultLogger("EvgenReader", logLevel));
    // add readEvgen as a reader
    sequencer.addReaders({readEvgen});
  }
  // return the resulting evgen collection
  return "";
}

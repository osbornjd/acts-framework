// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "../ExtrapolationExampleBase.hpp"  //nasty but working
#include "ACTFW/AtlasITk/BuildGenericDetector.hpp"
#include "ACTFW/AtlasITk/GenericDetectorElement.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"

#include <fstream>
#include <stdio.h>
#include <stdlib.h>

void
testDetector(std::shared_ptr<const Acts::TrackingGeometry>& tGeo)
{
  const unsigned             nEvents = 1;
  const Acts::ConstantBField bField  = {0, 0, 0};

  // Definition about what to shoot
  FW::ParticleGun::Config cfgParGun;
  cfgParGun.evgenCollection = "EvgenParticles";
  cfgParGun.nParticles      = 100;
  cfgParGun.z0Range         = {0., 0.};
  cfgParGun.d0Range         = {0., 0.};
  cfgParGun.phiRange        = {-M_PI, M_PI};
  cfgParGun.etaRange        = {-3, 3};
  cfgParGun.ptRange         = {0., 3 * Acts::units::_GeV};
  cfgParGun.mass            = 105.6 * Acts::units::_MeV;
  cfgParGun.charge          = -Acts::units::_e;
  cfgParGun.pID             = 13;

  // Configure RNG and barcode
  FW::RandomNumbersSvc::Config          cfgRng;
  std::shared_ptr<FW::RandomNumbersSvc> RngSvc(
      new FW::RandomNumbersSvc(cfgRng));
  cfgParGun.randomNumbers = RngSvc;

  FW::BarcodeSvc::Config          cfgBarcode;
  std::shared_ptr<FW::BarcodeSvc> BarSvc(new FW::BarcodeSvc(cfgBarcode));
  cfgParGun.barcodes = BarSvc;

  FW::RandomNumbersSvc::Config cfgEpol;

  // Pass everything to the ExtrapolationExampleBase
  ACTFWExtrapolationExample::run(nEvents,
                                 std::make_shared<Acts::ConstantBField>(bField),
                                 tGeo,
                                 cfgParGun,
                                 cfgEpol,
                                 Acts::Logging::VERBOSE);
}

/// This function builds FASER, handles shooting particles on it and collects
/// the results. The configuration of everything is given by two blocks in this
/// code.
int
main(int argc, char* argv[])
{
  std::shared_ptr<const Acts::TrackingGeometry> tGeo
      = FWGen::buildGenericDetector(
          Acts::Logging::INFO, Acts::Logging::INFO, Acts::Logging::INFO, 3);
  // Test the setup
  testDetector(tGeo);
}

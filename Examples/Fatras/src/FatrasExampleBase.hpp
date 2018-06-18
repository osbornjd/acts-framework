// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Pythia8/Generator.hpp"
#include "ACTFW/Plugins/Pythia8/GeneratorOptions.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/EvgenReader.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Fatras/FatrasOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Geometry/GeometryOptions.hpp"
#include "Fatras/Kernel/Definitions.hpp"
#include "Fatras/Kernel/Particle.hpp"
#include "Fatras/Kernel/SelectorList.hpp"
#include "Fatras/Kernel/Interactor.hpp"
#include "Fatras/Kernel/Simulator.hpp"
#include "Fatras/Selectors/ChargeSelectors.hpp"
#include "Fatras/Selectors/SelectorHelpers.hpp"
#include "Fatras/Selectors/KinematicCasts.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/MagneticField/SharedBFieldMap.hpp"
//#include "Acts/Propagator/AtlasStepper.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Extrapolator/Navigator.hpp"

namespace po = boost::program_options;

typedef Fatras::SensitiveHit FatrasHit;
typedef std::vector<Fatras::Vertex> FatrasEvent;

template <typename bfield_t>
void setupSimulation(bfield_t fieldMap,
                     FW::Sequencer& sequencer,
                     po::variables_map& vm,
                     std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
                     std::shared_ptr<FW::RandomNumbersSvc> randomNumberSvc,
                     const std::string& evgenCollection,
                     Acts::Logging::Level logLevel)
{
  
  // create a navigator for this tracking geometry
  Acts::Navigator cNavigator(tGeometry);
  Acts::Navigator nNavigator(tGeometry);
  
  using ChargedStepper     = Acts::EigenStepper<bfield_t>;
  using ChargedPropagator  = Acts::Propagator<ChargedStepper,Acts::Navigator>; 
  using NeutralStepper     = Acts::StraightLineStepper;  
  using NeutralPropagator  = Acts::Propagator<NeutralStepper,Acts::Navigator>;
  
  ChargedStepper    cStepper(std::move(fieldMap));
  ChargedPropagator cPropagator(std::move(cStepper), std::move(cNavigator));
  NeutralStepper    nStepper;
  NeutralPropagator nPropagator(std::move(nStepper), std::move(nNavigator));
  
  // The Selector for charged particles, including kinematic cuts 
  typedef Fatras::ChargedSelector             CSelector;  
  typedef Fatras::Max<Fatras::casts::absEta>  CMaxEtaAbs;
  typedef Fatras::Min<Fatras::casts::pT>      CMinPt;  
  typedef Fatras::SelectorListAND<CSelector,CMinPt,CMaxEtaAbs> ChargedSelector;
  
  typedef Fatras::NeutralSelector             NSelector;  
  typedef Fatras::Max<Fatras::casts::absEta>  NMaxEtaAbs;
  typedef Fatras::Min<Fatras::casts::E>       NMinE;  
  typedef Fatras::SelectorListAND<NSelector,NMinE,NMaxEtaAbs> NeutralSelector;
  
  typedef Fatras::Interactor<FW::RandomEngine> ChargedInteractor;
  typedef Fatras::Interactor<FW::RandomEngine> NeutralInteractor;
  
  typedef Fatras::Simulator<ChargedPropagator, 
                            ChargedSelector,
                            ChargedInteractor,
                            NeutralPropagator, 
                            NeutralSelector,
                            NeutralInteractor> FatrasSimulator;
                                         
  FatrasSimulator fatrasSimulator(cPropagator,nPropagator);
  
  using FatrasAlgorithm = FW::FatrasAlgorithm<FatrasSimulator,
                                              FatrasEvent,
                                              FatrasHit>;
  
  typename FatrasAlgorithm::Config fatrasConfig =
    FW::Options::readFatrasConfig<po::variables_map, 
                                  FatrasSimulator,
                                  FatrasEvent,
                                  FatrasHit>(vm, fatrasSimulator);
  fatrasConfig.randomNumberSvc      = randomNumberSvc;
  fatrasConfig.inputEventCollection = evgenCollection;
  // finally the fatras algorithm
  auto fatrasAlgorithm  = std::make_shared<FatrasAlgorithm>(fatrasConfig, logLevel);
  
  // finalize the squencer setting and run
  sequencer.appendEventAlgorithms({fatrasAlgorithm});

}


template <typename geometry_getter_t>
int
fatrasExample(int argc, char* argv[], geometry_getter_t trackingGeometry){
  
  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the geometry options
  FW::Options::addGeometryOptions<po::options_description>(desc);  
  // add the particle gun options
  FW::Options::addParticleGunOptions<po::options_description>(desc);
  // add the evgen options
  FW::Options::addEvgenReaderOptions<po::options_description>(desc);
  // add the pythia 8 options
  FW::Options::addPythia8Options<po::options_description>(desc);
  // add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // add the fatras options
  FW::Options::addFatrasOptions<po::options_description>(desc);
  
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents  = standardOptions.first;
  auto logLevel = standardOptions.second;
  // Create the random number engine
  auto randomNumberSvcCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumberSvc = std::make_shared<FW::RandomNumbersSvc>(randomNumberSvcCfg);
  // add it to the sequencer
  sequencer.addServices({randomNumberSvc});
  // Create the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));
  
  
  // Now read the particle gun configs
  auto particleGunCfg
      = FW::Options::readParticleGunConfig<po::variables_map>(vm);
  std::string evgenCollection = "";
  if (particleGunCfg.on){
    // we are running in particle gun mode
    particleGunCfg.barcodeSvc      = barcodeSvc;
    particleGunCfg.randomNumberSvc = randomNumberSvc;
    particleGunCfg.nEvents         = nEvents;    
    auto particleGun = std::make_shared<FW::ParticleGun>(particleGunCfg);
    // add particle gun as a reader
    evgenCollection = particleGunCfg.evgenCollection;
    sequencer.addReaders({particleGun});
    
  } else {
    // now read the pythia8 configs
    auto pythia8Configs = FW::Options::readPythia8Config<po::variables_map>(vm);
    pythia8Configs.first.randomNumberSvc  = randomNumberSvc;
    pythia8Configs.second.randomNumberSvc = randomNumberSvc;
    // the hard scatter generator
    auto hsPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
        pythia8Configs.first,
        Acts::getDefaultLogger("HardScatterPythia8Generator", logLevel));
    // the pileup generator
    auto puPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
        pythia8Configs.second,
        Acts::getDefaultLogger("PileUpPythia8Generator", logLevel));
    // Create the barcode service
    FW::BarcodeSvc::Config barcodeSvcCfg;
    auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
        barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));
    // now read the evgen config & set the missing parts
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
    evgenCollection = readEvgenCfg.evgenCollection;
    sequencer.addReaders({readEvgen});    
  }

  // get the tracking geometry
  auto tGeometry = trackingGeometry(vm);

  // create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);  
  // a charged propagator
  if (bField.first){
    // create the shared field
    using BField = Acts::SharedBField<Acts::InterpolatedBFieldMap>;
    BField fieldMap(bField.first);
    // now setup of the simulation 
    setupSimulation(std::move(fieldMap),
                    sequencer,
                    vm,
                    tGeometry,
                    randomNumberSvc,
                    evgenCollection,
                    logLevel);
  } else {
    // create the shared field
    using CField = Acts::ConstantBField;
    CField fieldMap(*bField.second);
    // now setup of the simulation 
    setupSimulation(std::move(fieldMap),
                    sequencer,
                    vm,
                    tGeometry,
                    randomNumberSvc,
                    evgenCollection,
                    logLevel);
  }
  
  // initiate the run
  sequencer.run(nEvents);
  return 1;
}





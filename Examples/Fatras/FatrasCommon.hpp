// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Common setup functions for fatras examples

#ifndef ACTFW_FATRASCOMMON_HPP
#define ACTFW_FATRASCOMMON_HPP

#include <memory>
#include <string>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Fatras/FatrasAlgorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Plugins/Root/RootGridUtils.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Digitization/PlanarModuleStepper.hpp"
#include "Fatras/EnergyLossSampler.hpp"
#include "Fatras/HadronicInteractionParametricSampler.hpp"
#include "Fatras/MaterialInteractionEngine.hpp"
#include "Fatras/MultipleScatteringSamplerHighland.hpp"
#include "ACTS/Utilities/detail/Axis.hpp"
#include "ACTS/Utilities/detail/Grid.hpp"

namespace Acts {
  typedef detail::Grid<double, detail::EquidistantAxis, detail::EquidistantAxis> ResolutionGrid;
  typedef std::pair<ResolutionGrid,ResolutionGrid> LayerResolution;
}

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH2F.h"

/// Setup extrapolation and digitization.
///
/// Expects a `EvgenParticles` object in the event store with the truth
/// particles.
template <class MagneticField>
FW::ProcessCode
setupSimulation(FW::Sequencer&                                sequencer,
                std::shared_ptr<const Acts::TrackingGeometry> geometry,
                std::shared_ptr<FW::RandomNumbersSvc>         random,
                std::shared_ptr<FW::BarcodeSvc>               barcodeSvc,
                std::shared_ptr<MagneticField>                bfield,
                std::array<bool, 4>&                          exoptions,
                Acts::Logging::Level loglevel = Acts::Logging::VERBOSE,
                std::string resolutionFile = "")
{
  // enable root thread safety in order to use root writers in multi threaded
  // mode
  ROOT::EnableThreadSafety();
  
  // set up the (potential) fatras material effects
  using MatIntEngine = Fatras::MaterialInteractionEngine<FW::RandomEngine>;
  std::shared_ptr<MatIntEngine> materialEngine = nullptr;
    
  // fatras is chosen 
  if (exoptions[0]){
    
      // we need the msc sampler 
      using MSCSampler
          = Fatras::MultipleScatteringSamplerHighland<FW::RandomEngine>;
      auto mscConfig  = MSCSampler::Config();
      auto mscSampler = std::make_shared<MSCSampler>(mscConfig);
      
      // EnergyLossSampler
      using eLossSampler      = Fatras::EnergyLossSampler<FW::RandomEngine>;
      auto eLossConfig        = eLossSampler::Config();
      eLossConfig.scalorMOP   = 0.745167;  // validated with geant4
      eLossConfig.scalorSigma = 0.68925;   // validated with geant4
      auto eLSampler          = std::make_shared<eLossSampler>(eLossConfig);
      eLSampler->setLogger(Acts::getDefaultLogger("ELoss", loglevel));
      
      // Hadronic interaction sampler
      using hadIntSampler
          = Fatras::HadronicInteractionParametricSampler<FW::RandomEngine>;
      auto hiConfig  = hadIntSampler::Config();
      auto hiSampler = std::make_shared<hadIntSampler>(hiConfig);
      
      // MaterialInteractionEngine
      auto matConfig     = MatIntEngine::Config();
      matConfig.multipleScatteringSampler  = (exoptions[2] ? mscSampler : nullptr);
      matConfig.energyLossSampler          = (exoptions[1] ? eLSampler : nullptr);
      matConfig.parametricScattering       = true;
      matConfig.hadronicInteractionSampler = (exoptions[3] ? hiSampler : nullptr);
      materialEngine = std::make_shared<MatIntEngine>(matConfig);
      materialEngine->setLogger(Acts::getDefaultLogger("MaterialEngine", loglevel));
  }
  
  // extrapolation algorithm
  FW::ExtrapolationAlgorithm::Config eTestConfig;
  eTestConfig.evgenCollection                  = "EvgenParticles";
  eTestConfig.simulatedParticlesCollection     = "FatrasParticles";
  eTestConfig.simulatedHitsCollection          = "FatrasHits";
  eTestConfig.simulatedChargedExCellCollection = "excells_charged";
  eTestConfig.simulatedNeutralExCellCollection = "excells_neutral";
  eTestConfig.searchMode                       = 1;
  eTestConfig.extrapolationEngine
      = FW::initExtrapolator(geometry, bfield, loglevel, materialEngine);

  eTestConfig.skipNeutral          = true;
  eTestConfig.collectSensitive     = true;
  eTestConfig.collectPassive       = true;
  eTestConfig.collectBoundary      = true;
  eTestConfig.collectMaterial      = true;
  eTestConfig.sensitiveCurvilinear = false;
  eTestConfig.pathLimit            = -1.;
  eTestConfig.maxD0                = 1000.;
  eTestConfig.maxZ0                = 3000.;
  eTestConfig.randomNumbers        = random;
  eTestConfig.barcodeSvc           = barcodeSvc;
  // Set up the FatrasAlgorithm
  using FatrasAlg = FW::FatrasAlgorithm<MatIntEngine>;
  FatrasAlg::Config fatrasConfig;
  fatrasConfig.exConfig                  = eTestConfig;
  fatrasConfig.randomNumbers             = random;
  fatrasConfig.materialInteractionEngine = materialEngine;
  auto fatrasAlg = std::make_shared<FatrasAlg>(fatrasConfig, loglevel);


  // digitisation
  Acts::PlanarModuleStepper::Config pmStepperConfig;
  
  auto pmStepper = std::make_shared<Acts::PlanarModuleStepper>(
      pmStepperConfig, Acts::getDefaultLogger("PlanarModuleStepper", loglevel));

  FW::DigitizationAlgorithm::Config digConfig;
  digConfig.simulatedHitsCollection = eTestConfig.simulatedHitsCollection;
  digConfig.clustersCollection      = "FatrasClusters";
  digConfig.spacePointsCollection   = "FatrasSpacePoints";
  digConfig.randomNumbers           = random;
  digConfig.planarModuleStepper     = pmStepper;
  
  // load the resolutions
  // @todo move reading into a IReaderT 
  if (resolutionFile != ""){
    // create the map
    std::shared_ptr<Acts::ResolutionMap> lResolutions 
      = std::shared_ptr<Acts::ResolutionMap>(new Acts::ResolutionMap);
    // read the root file
    TFile* rFile = TFile::Open(resolutionFile.c_str());
    if (rFile){
      TTree* tVids = dynamic_cast<TTree*>(rFile->Get("volumeIDs"));
      // now get the list of vids and lids 
      if (tVids){
        // get the volume ids
        std::vector<int>* b_vids = new std::vector<int>;
        tVids->SetBranchAddress("volumeIDs", &b_vids);
        tVids->GetEntry(0);
        // now loop over the volume IDs
        for (auto& vid : (*b_vids)){
          // change into the dedicated directory
          TString vidDir = "vid";
          vidDir += vid;
          // get the layer IDs 
          TTree* tLids = dynamic_cast<TTree*>(rFile->Get(vidDir+"/layerIDs"));
          if (tLids){
            // get the layer ids
            std::vector<int>* b_lids = new std::vector<int>;
            tLids->SetBranchAddress("layerIDs", &b_lids);
            tLids->GetEntry(0);
            // loop over the layers
            for (auto& lid: (*b_lids)){
              // retrieve the summary histograms from the layers
              TString hHistName = vidDir;
              TString lidDir = "lid";
              lidDir += lid;
              hHistName += "/";
              hHistName += lidDir;
              hHistName += "/summary_res_l";
              // l0 / l1 histogram retrievals
              TString l0HistName = hHistName;
              l0HistName += "0";
              l0HistName += "_"+vidDir;
              l0HistName += "_"+lidDir;
              TString l1HistName = hHistName;
              l1HistName += "1";
              l1HistName += "_"+vidDir;
              l1HistName += "_"+lidDir;              
              TH2F* l0Hist = dynamic_cast<TH2F*>(rFile->Get(l0HistName));
              TH2F* l1Hist = dynamic_cast<TH2F*>(rFile->Get(l1HistName));
              // get the layer type
              int lType = 0;
              // default resolution for layer 
              double rmsl0 = 0.;
              double rmsl1 = 0.;
              // create a layer Type Tree
              TTree* ltTree = dynamic_cast<TTree*>
                (rFile->Get(vidDir+"/"+lidDir+"/layerInfo"));
              // everything retrieved
              if (l0Hist and l1Hist and ltTree){
                // retieving the layer type information
                ltTree->SetBranchAddress("layerType", &lType);
                ltTree->SetBranchAddress("layerRms0", &rmsl0);
                ltTree->SetBranchAddress("layerRms1", &rmsl1);
                ltTree->GetEntry(0);
                // create a GeoId of the volume index and layer index
                Acts::GeometryID lgeoID(0);
                lgeoID.add(vid, Acts::GeometryID::volume_mask);
                lgeoID.add(lid, Acts::GeometryID::layer_mask);
                // create the two resolution maps
                auto g0 = FW::Root::histToGrid(*l0Hist, {true, rmsl0}, true);
                auto g1 = FW::Root::histToGrid(*l1Hist, {true, rmsl1}, true);
                // create a new pair of those and insert to the map
                auto g0g1 = std::make_shared<Acts::LayerResolution>
                  (std::move(g0),std::move(g1)); 
                //lResolutions->operator[](lgeoID) = g0g1;
                (*lResolutions)[lgeoID]      = std::move(g0g1);
                digConfig.layerTypes[lgeoID] = lType;
              }
            }
            // delete the vector for layer IDs
            delete b_lids;
          }
        }
        // dete the vector volume IDs
        delete b_vids;
      }
    }
    // assign the resolution map
    digConfig.layerResolutions = lResolutions;
  }
    
  auto digitzationAlg
      = std::make_shared<FW::DigitizationAlgorithm>(digConfig, loglevel);

  // add algorithms to sequencer
  //  if (sequencer.appendEventAlgorithms({fatrasAlg, digitzationAlg})
  if (sequencer.appendEventAlgorithms({fatrasAlg, digitzationAlg})
      != FW::ProcessCode::SUCCESS)
    return FW::ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}

/// Setup writers to store simulation output
///
/// Expects `FatrasParticles`, `FatrasClusters`, `FatrasSpacePoints` objects
/// to be present in the event store.
FW::ProcessCode
setupWriters(FW::Sequencer&                  sequencer,
             std::shared_ptr<FW::BarcodeSvc> barcode,
             std::string                     outputDir,
             Acts::Logging::Level            loglevel = Acts::Logging::INFO);

#endif  // ACTFW_FATRASCOMMON_HPP

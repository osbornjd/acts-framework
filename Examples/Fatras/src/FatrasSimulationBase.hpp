// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Fatras/FatrasAlgorithm.hpp"
#include "ACTFW/Fatras/FatrasOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Extrapolator/Navigator.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/MagneticField/SharedBFieldMap.hpp"
#include "Acts/Propagator/AtlasStepper.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"
#include "Acts/Propagator/detail/DebugOutputActor.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Fatras/Kernel/Definitions.hpp"
#include "Fatras/Kernel/Interactor.hpp"
#include "Fatras/Kernel/Particle.hpp"
#include "Fatras/Kernel/SelectorList.hpp"
#include "Fatras/Kernel/Simulator.hpp"
#include "Fatras/Selectors/ChargeSelectors.hpp"
#include "Fatras/Selectors/KinematicCasts.hpp"
#include "Fatras/Selectors/SelectorHelpers.hpp"

typedef Fatras::SensitiveHit        FatrasHit;
typedef std::vector<Fatras::Vertex> FatrasEvent;

/// Simple struct to select sensitive surfaces
struct SurfaceSelector
{

  bool selectSensitive = true;
  bool selectMaterial  = false;
  bool selectPassive   = false;

  /// SurfaceSelector with options
  ///
  /// @param sSensitive is the directive to select sensitive surfaces
  /// @param sMaterial is the directive to select material surfaces
  /// @param sPassive is the directive to select passivle surfaces
  SurfaceSelector(bool sSensitive = true,
                  bool sMaterial  = false,
                  bool sPassive   = true)
    : selectSensitive(sSensitive)
    , selectMaterial(sMaterial)
    , selectPassive(sPassive)
  {
  }

  /// Call operator to check if a surface should be selected
  ///
  /// @param surface is the test surface
  bool
  operator()(const Acts::Surface& surface) const
  {
    if (selectPassive) return true;
    if (selectSensitive && surface.associatedDetectorElement()) {
      std::cout << "On sensitive " << std::endl;
      return true;
    }
    if (selectMaterial && surface.associatedMaterial()) return true;
    return false;
  }
};

template <typename bfield_t>
void
setupSimulation(bfield_t                                      fieldMap,
                FW::Sequencer&                                sequencer,
                po::variables_map&                            vm,
                std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
                std::shared_ptr<FW::BarcodeSvc>               barcodeSvc,
                std::shared_ptr<FW::RandomNumbersSvc>         randomNumberSvc,
                const std::string&                            evgenCollection,
                Acts::Logging::Level                          logLevel)
{

  // create a navigator for this tracking geometry
  Acts::Navigator cNavigator(tGeometry);
  Acts::Navigator nNavigator(tGeometry);

  // using ChargedStepper     = Acts::AtlasStepper<bfield_t>;
  using ChargedStepper    = Acts::EigenStepper<bfield_t>;
  using ChargedPropagator = Acts::Propagator<ChargedStepper, Acts::Navigator>;
  using NeutralStepper    = Acts::StraightLineStepper;
  using NeutralPropagator = Acts::Propagator<NeutralStepper, Acts::Navigator>;

  ChargedStepper    cStepper(std::move(fieldMap));
  ChargedPropagator cPropagator(std::move(cStepper), std::move(cNavigator));
  NeutralStepper    nStepper;
  NeutralPropagator nPropagator(std::move(nStepper), std::move(nNavigator));

  // The Selector for charged particles, including kinematic cuts
  typedef Fatras::ChargedSelector            CSelector;
  typedef Fatras::Max<Fatras::casts::absEta> CMaxEtaAbs;
  typedef Fatras::Min<Fatras::casts::pT>     CMinPt;
  typedef Fatras::SelectorListAND<CSelector, CMinPt, CMaxEtaAbs>
      ChargedSelector;

  typedef Fatras::NeutralSelector            NSelector;
  typedef Fatras::Max<Fatras::casts::absEta> NMaxEtaAbs;
  typedef Fatras::Min<Fatras::casts::E>      NMinE;
  typedef Fatras::SelectorListAND<NSelector, NMinE, NMaxEtaAbs> NeutralSelector;

  typedef Fatras::PhysicsList<> PhysicsList;

  typedef Fatras::Interactor<FW::RandomEngine, SurfaceSelector, PhysicsList>
                                               ChargedInteractor;
  typedef Fatras::Interactor<FW::RandomEngine> NeutralInteractor;

  typedef Fatras::Simulator<ChargedPropagator,
                            ChargedSelector,
                            ChargedInteractor,
                            NeutralPropagator,
                            NeutralSelector,
                            NeutralInteractor>
      FatrasSimulator;

  FatrasSimulator fatrasSimulator(cPropagator, nPropagator);

  using FatrasAlgorithm
      = FW::FatrasAlgorithm<FatrasSimulator, FatrasEvent, FatrasHit>;

  typename FatrasAlgorithm::Config fatrasConfig
      = FW::Options::readFatrasConfig<po::variables_map,
                                      FatrasSimulator,
                                      FatrasEvent,
                                      FatrasHit>(vm, fatrasSimulator);
  fatrasConfig.randomNumberSvc      = randomNumberSvc;
  fatrasConfig.inputEventCollection = evgenCollection;
  // finally the fatras algorithm
  auto fatrasAlgorithm
      = std::make_shared<FatrasAlgorithm>(fatrasConfig, logLevel);

  // finalize the squencer setting and run
  sequencer.appendEventAlgorithms({fatrasAlgorithm});
}

template <typename vmap_t>
FW::ProcessCode
setupSimulation(vmap_t&                                       vm,
                FW::Sequencer&                                sequencer,
                std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
                std::shared_ptr<FW::BarcodeSvc>               barcodeSvc,
                std::shared_ptr<FW::RandomNumbersSvc>         randomNumberSvc,
                const std::string&                            evgenCollection)
{

  // Now read the standard options
  auto standardOptions = FW::Options::readStandardOptions<vmap_t>(vm);
  auto logLevel        = standardOptions.second;

  // create BField service
  auto bField = FW::Options::readBField<vmap_t>(vm);
  // a charged propagator
  if (bField.first) {
    // create the shared field
    using BField = Acts::SharedBField<Acts::InterpolatedBFieldMap>;
    BField fieldMap(bField.first);
    // now setup of the simulation
    setupSimulation(std::move(fieldMap),
                    sequencer,
                    vm,
                    tGeometry,
                    barcodeSvc,
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
                    barcodeSvc,
                    randomNumberSvc,
                    evgenCollection,
                    logLevel);
  }
  return FW::ProcessCode::SUCCESS;
}

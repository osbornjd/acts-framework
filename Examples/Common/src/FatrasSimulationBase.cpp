// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "detail/FatrasSimulationBase.hpp"

#include <boost/program_options.hpp>

#include "ACTFW/EventData/SimHit.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Fatras/FatrasAlgorithm.hpp"
#include "ACTFW/Fatras/FatrasOptions.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Generators/FlattenEvent.hpp"
#include "ACTFW/Io/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Io/Root/RootParticleWriter.hpp"
#include "ACTFW/Io/Root/RootSimHitWriter.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/BField/ScalableBField.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "Acts/Geometry/GeometryID.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/MagneticField/SharedBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"
#include "Acts/Propagator/detail/DebugOutputActor.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "ActsFatras/Kernel/Interactor.hpp"
#include "ActsFatras/Kernel/Process.hpp"
#include "ActsFatras/Kernel/SelectorList.hpp"
#include "ActsFatras/Kernel/Simulator.hpp"
#include "ActsFatras/Physics/EnergyLoss/BetheBloch.hpp"
#include "ActsFatras/Physics/EnergyLoss/BetheHeitler.hpp"
#include "ActsFatras/Physics/Scattering/Highland.hpp"
#include "ActsFatras/Physics/Scattering/Scattering.hpp"
#include "ActsFatras/Selectors/ChargeSelectors.hpp"
#include "ActsFatras/Selectors/KinematicCasts.hpp"
#include "ActsFatras/Selectors/SelectorHelpers.hpp"

/// Construct SimHits from Fatras.
struct SimHitCreator
{
  /// @param surface is the Surface where the hit is created
  /// @param position is the global hit position
  /// @param direction is the momentum direction at hit position
  /// @param value is the simulated value
  /// @param time is the timeStamp
  /// @param particle is the particle for the truth link
  FW::Data::SimHit
  operator()(const Acts::Surface&         surface,
             const Acts::Vector3D&        position,
             const Acts::Vector3D&        direction,
             double                       value,
             double                       time,
             const FW::Data::SimParticle& simParticle) const
  {
    FW::Data::SimHit simHit(surface);
    simHit.position  = position;
    simHit.time      = time;
    simHit.direction = direction;
    simHit.value     = value;
    simHit.particle  = simParticle;
    return simHit;
  }
};

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
  /// @param sPassive is the directive to select passive surfaces
  SurfaceSelector(bool sSensitive = true,
                  bool sMaterial  = false,
                  bool sPassive   = false)
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
    if (selectSensitive && surface.associatedDetectorElement()) { return true; }
    if (selectMaterial && surface.surfaceMaterial()) { return true; }
    if (selectPassive) { return true; }
    return false;
  }
};

/// The selector
struct Selector
{
  /// call operator
  template <typename detector_t, typename particle_t>
  bool
  operator()(const detector_t, const particle_t&) const
  {
    return true;
  }
};

/// @brief Simulation setup for the FatrasAlgorithm
///
/// @tparam bfield_t Type of the bfield for the simulation to be set up
///
/// @param fieldMap The field map for the simulation setup
/// @param sequencer The framework sequencer
/// @param vm The boost variable map to resolve
/// @param tGeometry The TrackingGeometry for the tracking setup
/// @param barcodesSvc The barcode service to be used for the simulation
/// @param randomNumberSvc The random number service to be used for the
/// simulation
template <typename bfield_t>
void
setupSimulationAlgorithm(
    bfield_t                                      fieldMap,
    FW::Sequencer&                                sequencer,
    boost::program_options::variables_map&        vm,
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
    std::shared_ptr<FW::RandomNumbers>            randomNumberSvc)
{
  // Read the log level
  Acts::Logging::Level logLevel = FW::Options::readLogLevel(vm);

  /// Read the evgen particle collection
  std::string evgenCollection = "particles";

  // define propagator types for charged and neutral particles
  using Navigator         = Acts::Navigator;
  using ChargedStepper    = Acts::EigenStepper<bfield_t>;
  using ChargedPropagator = Acts::Propagator<ChargedStepper, Navigator>;
  using NeutralStepper    = Acts::StraightLineStepper;
  using NeutralPropagator = Acts::Propagator<NeutralStepper, Navigator>;
  // define selector types for particle cuts
  using NonZeroCharge = ActsFatras::ChargedSelector;
  using ZeroCharge    = ActsFatras::NeutralSelector;
  using PtMin         = ActsFatras::Min<ActsFatras::Casts::pT>;
  using EMin          = ActsFatras::Min<ActsFatras::Casts::E>;
  using AbsEtaMax     = ActsFatras::Max<ActsFatras::Casts::absEta>;
  using ChargedSelector
      = ActsFatras::SelectorListAND<NonZeroCharge, PtMin, AbsEtaMax>;
  using NeutralSelector
      = ActsFatras::SelectorListAND<ZeroCharge, EMin, AbsEtaMax>;

  // Define interactor types
  using All = Selector;
  // Define the processes with selectors (no hadronic interaction yet??)
  using BetheBlochProcess
      = ActsFatras::Process<ActsFatras::BetheBloch, All, All, All>;
  using BetheHeitlerProcess
      = ActsFatras::Process<ActsFatras::BetheHeitler, All, All, All>;
  using ScatteringProcess = ActsFatras::
      Process<ActsFatras::Scattering<ActsFatras::Highland>, All, All, All>;
  using PhysicsList = ActsFatras::
      PhysicsList<BetheBlochProcess, BetheHeitlerProcess, ScatteringProcess>;

  using ChargedInteractor = ActsFatras::Interactor<FW::RandomEngine,
                                                   FW::Data::SimParticle,
                                                   FW::Data::SimHit,
                                                   SimHitCreator,
                                                   SurfaceSelector,
                                                   PhysicsList>;
  using NeutralInteractor = ActsFatras::Interactor<FW::RandomEngine,
                                                   FW::Data::SimParticle,
                                                   FW::Data::SimHit,
                                                   SimHitCreator>;
  // define simulator type
  using Simulator = ActsFatras::Simulator<ChargedPropagator,
                                          ChargedSelector,
                                          ChargedInteractor,
                                          NeutralPropagator,
                                          NeutralSelector,
                                          NeutralInteractor>;
  // define algorithm type
  using SimulationAlgorithm = FW::FatrasAlgorithm<Simulator>;

  // construct the propagators
  Navigator         navigator(trackingGeometry);
  ChargedStepper    chargedStepper(std::move(fieldMap));
  ChargedPropagator chargedPropagator(std::move(chargedStepper), navigator);
  NeutralPropagator neutralPropagator(NeutralStepper(), navigator);
  // construct the simulator
  Simulator simulator(std::move(chargedPropagator),
                      std::move(neutralPropagator));
  simulator.debug = vm["fatras-debug-output"].template as<bool>();
  // set the switch for processes in physics list
  simulator.physicsList.template get<BetheBlochProcess>().process.betheBloch
      = vm["fatras-em-ionisation"].template as<bool>();
  simulator.physicsList.template get<BetheHeitlerProcess>().process.betheHeitler
      = vm["fatras-em-radiation"].template as<bool>();
  simulator.physicsList.template get<ScatteringProcess>().process.scattering
      = vm["fatras-em-scattering"].template as<bool>();

  // construct the simulation algorithm
  auto fatrasConfig = FW::Options::readFatrasConfig(vm, std::move(simulator));
  fatrasConfig.randomNumberSvc      = randomNumberSvc;
  fatrasConfig.inputEventCollection = evgenCollection;

  // Finally the fatras algorithm
  sequencer.addAlgorithm(
      std::make_shared<SimulationAlgorithm>(fatrasConfig, logLevel));

  // Output directory
  std::string outputDir = vm["output-dir"].template as<std::string>();

  // flatten event to just particles
  FW::FlattenEvent::Config flatten;
  flatten.inputEvent      = fatrasConfig.simulatedEventCollection;
  flatten.outputParticles = fatrasConfig.simulatedEventCollection + "-flat";
  sequencer.addAlgorithm(std::make_shared<FW::FlattenEvent>(flatten, logLevel));

  // Write simulation information as CSV files
  std::shared_ptr<FW::CsvParticleWriter> pWriterCsv = nullptr;
  if (vm["output-csv"].template as<bool>()) {
    FW::CsvParticleWriter::Config pWriterCsvConfig;
    pWriterCsvConfig.inputParticles = flatten.outputParticles;
    pWriterCsvConfig.outputDir      = outputDir;
    pWriterCsvConfig.outputStem     = fatrasConfig.simulatedEventCollection;
    sequencer.addWriter(
        std::make_shared<FW::CsvParticleWriter>(pWriterCsvConfig, logLevel));
  }

  // Write simulation information as ROOT files
  std::shared_ptr<FW::RootParticleWriter> pWriterRoot = nullptr;
  if (vm["output-root"].template as<bool>()) {
    // Write particles as ROOT TTree
    FW::RootParticleWriter::Config pWriterRootConfig;
    pWriterRootConfig.inputParticles = flatten.outputParticles;
    pWriterRootConfig.filePath       = FW::joinPaths(
        outputDir, fatrasConfig.simulatedEventCollection + ".root");
    pWriterRootConfig.treeName = fatrasConfig.simulatedEventCollection;
    sequencer.addWriter(
        std::make_shared<FW::RootParticleWriter>(pWriterRootConfig, logLevel));

    // Write simulated hits as ROOT TTree
    FW::RootSimHitWriter::Config fhitWriterRootConfig;
    fhitWriterRootConfig.inputSimulatedHits
        = fatrasConfig.simulatedHitCollection;
    fhitWriterRootConfig.filePath = FW::joinPaths(
        outputDir, fatrasConfig.simulatedHitCollection + ".root");
    fhitWriterRootConfig.treeName = fatrasConfig.simulatedHitCollection;
    sequencer.addWriter(
        std::make_shared<FW::RootSimHitWriter>(fhitWriterRootConfig, logLevel));
  }
}

void
setupSimulation(boost::program_options::variables_map&        vm,
                FW::Sequencer&                                sequencer,
                std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
                std::shared_ptr<FW::RandomNumbers>            randomNumberSvc)
{
  // Create BField service
  auto bFieldVar = FW::Options::readBField(vm);

  std::visit(
      [&](auto& bField) {
        using field_type =
            typename std::decay_t<decltype(bField)>::element_type;
        Acts::SharedBField<field_type> fieldMap(bField);
        setupSimulationAlgorithm(
            std::move(fieldMap), sequencer, vm, tGeometry, randomNumberSvc);
      },
      bFieldVar);
}

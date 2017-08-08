/// @file
/// @brief Common setup functions for fatras examples

#ifndef ACTFW_FATRASCOMMON_HPP
#define ACTFW_FATRASCOMMON_HPP

#include <memory>
#include <string>

#include <ACTS/Detector/TrackingGeometry.hpp>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

/// Setup extrapolation and digitization.
///
/// Expects a `Particles` object in the event store with the truth particles.
FW::ProcessCode
setupSimulation(FW::Sequencer&                                sequencer,
                std::shared_ptr<const Acts::TrackingGeometry> geometry,
                std::shared_ptr<FW::RandomNumbersSvc>         random);

/// Setup writers to store simulation output
///
/// Expects `FatrasParticles`, `FatrasClusters`, `FatrasSpacePoints` objects
/// to be present in the event store.
FW::ProcessCode
setupWriters(FW::Sequencer&                  sequencer,
             std::shared_ptr<FW::BarcodeSvc> barcode,
             std::string                     outputDir);

#endif  // ACTFW_FATRASCOMMON_HPP

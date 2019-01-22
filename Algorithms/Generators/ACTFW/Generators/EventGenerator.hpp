// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @date 2018-03-13
/// @author Moritz Kiehn <msmk@cern.ch>

#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <vector>

#include <Acts/Utilities/Definitions.hpp>
#include <Acts/Utilities/Logger.hpp>

#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/IReader.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

namespace FW {

class BarcodeSvc;
class RandomNumbersSvc;

/// Event generator based on separate process and vertex generators.
///
/// This must be a reader and not just an algorithm since it might read in
/// pre-computed generator samples, e.g. via HEPMC, and therefore has an
/// internal state that will be modified.
class EventGenerator final : public FW::IReader
{
public:
  /// Combined set of generator functions.
  ///
  /// Each generator creates a number of primary vertices (multiplicity),
  /// each with an separate vertex position (vertex), and a number
  /// of processes with associated particles anchored at the vertex position.
  /// The first process at each vertex position
  using MultiplicityGenerator = std::function<size_t(RandomEngine&)>;
  using VertexGenerator
      = std::function<Acts::ActsVector<double, 4>(RandomEngine&)>;
  using ProcessesGenerator
      = std::function<std::vector<Data::SimVertex<Data::SimParticle>>(
          RandomEngine&)>;
  struct Generator
  {
    MultiplicityGenerator multiplicity = nullptr;
    VertexGenerator       vertex       = nullptr;
    ProcessesGenerator    processes    = nullptr;
  };

  struct Config
  {
    /// Name of the output collection
    std::string output;
    /// List of generators that should be used to generate the event
    std::vector<Generator> generators;
    /// Shuffle generated vertices to mix generator output
    bool shuffle = false;
    /// The random number service
    std::shared_ptr<RandomNumbersSvc> randomNumbers = nullptr;
    /// The barcode service to generate particle identifiers
    std::shared_ptr<BarcodeSvc> barcodeSvc = nullptr;
  };

  EventGenerator(const Config&        cfg,
                 Acts::Logging::Level level = Acts::Logging::INFO);

  std::string
  name() const override final;
  size_t
  numEvents() const override final;
  ProcessCode
  skip(size_t skip) override final;
  ProcessCode
  read(AlgorithmContext context) override final;

private:
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }

  Config                              m_cfg;
  std::unique_ptr<const Acts::Logger> m_logger;
};

}  // namespace FW

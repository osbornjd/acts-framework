// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_EXAMPLES_READEVGENALGORITHM_H
#define ACTFW_EXAMPLES_READEVGENALGORITHM_H

#include <array>
#include <memory>

#include "ACTFW/Framework/IReader.hpp"
#include "ACTFW/Readers/IReaderT.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

class WhiteBoard;
class RandomNumbersSvc;
class BarcodeSvc;

/// @class ReadEvgenAlgorithm
///
/// ReadEvgenAlgorithm to read EvGen from some input
/// Allows for pile-up reading as well
class ReadEvgenAlgorithm : public FW::IReader
{
public:
  /// @struct Config
  /// configuration struct for this Algorithm
  struct Config
  {
    /// the hard scatter reader (optional)
    std::shared_ptr<IReaderT<std::vector<Acts::ProcessVertex>>>
        hardscatterEventReader = nullptr;
    /// the pileup reader (optional)
    std::shared_ptr<IReaderT<std::vector<Acts::ProcessVertex>>>
        pileupEventReader = nullptr;
    /// name of the output collection
    std::string evgenCollection = "EvgenParticles";
    /// the Poisson parameter for pileup generation
    int pileupPoissonParameter = 40;
    /// the Gaussian parameters for transverse and axial vertex generation
    std::array<double, 2> vertexTParameters = {{0., 1.}};
    std::array<double, 2> vertexZParameters = {{0., 1.}};
    /// the BarcodeSvc
    std::shared_ptr<BarcodeSvc> barcodeSvc = nullptr;
    /// the random number service
    std::shared_ptr<RandomNumbersSvc> randomNumbers = nullptr;
  };

  /// Constructor
  ReadEvgenAlgorithm(const Config&                       cnf,
                     std::unique_ptr<const Acts::Logger> logger
                     = Acts::getDefaultLogger("ReadEvgenAlgorithm",
                                              Acts::Logging::INFO));

  /// Framework name() method
  std::string
  name() const final override;

  /// Skip a few events in the IO stream
  /// @param [in] nEvents is the number of skipped events
  FW::ProcessCode
  skip(size_t nEvents) final override;

  /// Read out data from the input stream
  FW::ProcessCode
  read(FW::AlgorithmContext ctx) final override;

private:
  Config                              m_cfg;
  std::unique_ptr<const Acts::Logger> m_logger;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

}  // namespace FW

#endif  /// ACTFW_EXAMPLES_READEVGENALGORITHM_H

// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>
#include <mutex>

#include <Acts/Utilities/Logger.hpp>
#include <Fatras/Kernel/Particle.hpp>
#include <Pythia8/Pythia.h>

#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Readers/IReaderT.hpp"

namespace FW {

using InputReader = IReaderT<std::vector<Fatras::Vertex>>;

namespace GPythia8 {

  /// @class IParticleReader
  ///
  /// Interface class that fills a vector of process vertices
  /// proerties for feeding into the fast simulation
  ///
  class Generator : public InputReader
  {
  public:
    struct Config
    {
      int    pdgBeam0  = 2212;    ///< pdg code of incoming beam 1
      int    pdgBeam1  = 2212;    ///< pdg code of incoming beam 2
      double cmsEnergy = 14000.;  ///< center of mass energy
      std::vector<std::string> processStrings
          = {{"HardQCD:all = on"}};  ///< pocesses
      std::shared_ptr<FW::RandomNumbersSvc> randomNumberSvc = nullptr;
    };

    /// Constructor
    /// @param cfg is the configuration class
    /// @param logger is the logger instance
    Generator(const Config&                       cfg,
              std::unique_ptr<const Acts::Logger> logger
              = Acts::getDefaultLogger("Generator", Acts::Logging::INFO));

    /// Framework name() method
    std::string
    name() const final override;

    // clang-format off
    /// @copydoc FW::IReaderT::read(std::vector<Fatras::Vertex>& pProperties,size_t,const FW::AlgorithmContext*)
    // clang-format on
    FW::ProcessCode
    read(std::vector<Fatras::Vertex>& pProperties,
         size_t                       skip    = 0,
         const FW::AlgorithmContext*  context = nullptr) final override;

  private:
    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return (*m_logger);
    }

    /// the configuration class
    Config m_cfg;
    /// logger instance
    std::unique_ptr<const Acts::Logger> m_logger;
    /// the pythia object
    Pythia8::Pythia m_pythia8;
    /// mutex used to protect multi-threaded reads
    std::mutex m_read_mutex;
  };
}  // namespace GPythia8
}  // namespace FW

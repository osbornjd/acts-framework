// This file is part of the Acts project.
//
// Copyright (C) 2017,2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <array>
#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"

namespace FW {

/// An example algorithm that uses the random number generator.
class HelloRandomAlgorithm : public FW::BareAlgorithm
{
public:
  struct Config
  {
    std::shared_ptr<FW::RandomNumbers> randomNumbers = nullptr;

    std::array<double, 2> gaussParameters   = {{0., 1.}};
    std::array<double, 2> uniformParameters = {{0., 1.}};
    std::array<double, 2> gammaParameters   = {{0., 1.}};
    int                   poissonParameter  = 40;
    size_t                drawsPerEvent     = 0;
  };

  HelloRandomAlgorithm(const Config&        cnf,
                       Acts::Logging::Level level = Acts::Logging::INFO);

  // Generate random numbers from various distributions.
  FW::ProcessCode
  execute(const AlgorithmContext& context) const final override;

private:
  Config m_cfg;
};

}  // namespace FW

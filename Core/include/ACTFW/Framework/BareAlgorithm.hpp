// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>
#include <string>

#include "ACTFW/Framework/IAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace FW {

/// A helper class for users to implement framework algorithms
///
/// This class can be used for algorithms that only implement the per-event
/// execute method. All other methods have locked default no-op implementations.
/// Algorithms that do not fit this scheme should implement the algorithm
/// interface directly without this helper class.
class BareAlgorithm : public IAlgorithm
{
public:
  /// Constructor
  ///
  /// @name The algorithm name
  /// @level The logging level for this algorithm
  BareAlgorithm(std::string          name,
                Acts::Logging::Level level = Acts::Logging::INFO);

  /// Framework name() method
  std::string
  name() const final override;

  /// Framework execute method
  ///
  /// This function must be implemented by subclasses.
  /// @param context The object that allows event consistency
  /// in multi-threaded execution
  virtual ProcessCode
  execute(AlgorithmContext context) const override = 0;

protected:
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }

private:
  std::string                         m_name;
  std::unique_ptr<const Acts::Logger> m_logger;
};

}  // namespace FW

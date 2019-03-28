// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>
#include "ACTFW/ContextualDetector/AlignedDetectorElement.hpp"
#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/IContextDecorator.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/GeometryContext.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace FW {

namespace Contextual {

  /// @brief A mockup service that rotates the modules in a
  /// simple tracking geometry
  ///
  /// It acts on the PayloadDetectorElement, i.e. the
  /// geometry context carries the full transform store (payload)
  class AlignmentDecorator : public IContextDecorator
  {
  public:
    using LayerStore    = std::vector<std::shared_ptr<AlignedDetectorElement>>;
    using DetectorStore = std::vector<LayerStore>;

    /// @brief nested configuration struct
    struct Config
    {
      /// The detector store (filled at creation creation)
      DetectorStore detectorStore;

      /// Alignment frequency - every X events
      unsigned int iovSize = 100;

      /// Flush store size - remove "oldest" after
      unsigned int flushSize = 200;
    };

    /// Constructor
    ///
    /// @param cfg Configuration struct
    /// @param logger The logging framework
    AlignmentDecorator(const Config&                       cfg,
                       std::unique_ptr<const Acts::Logger> logger
                       = Acts::getDefaultLogger("AlignmentDecorator",
                                                Acts::Logging::INFO));

    /// Virtual destructor
    virtual ~AlignmentDecorator() = default;

    /// @brief decorates (adds, modifies) the AlgorithmContext
    /// with a geometric rotation per event
    ///
    /// @note If decorarators depend on each other, they have to be
    /// added in order.
    ///
    /// @param context the bare (or at least non-const) Event context
    ProcessCode
    decorate(AlgorithmContext& context) const final override;

    /// @brief decorator name() for screen output
    const std::string&
    name() const final override
    {
      return m_name;
    }

  private:
    Config                              m_cfg;     ///< the configuration class
    std::unique_ptr<const Acts::Logger> m_logger;  ///!< the logging instance
    std::string                         m_name = "AlignmentDecorator";

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_logger;
    }
  };
}  // namespace Contextual
}  // namespace FW

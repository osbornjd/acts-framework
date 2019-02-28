// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>
#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/IContextDecorator.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/GeometryContext.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace Acts {
class TrackingGeometry;
}

namespace FW {

namespace Alignable {

  /// A mockup service that rotates a
  /// cylindrical geoemtry
  class GeometryRotationDecorator : public IContextDecorator
  {
  public:
    /// @brief nested configuration struct
    struct Config
    {
      /// The trackng geometry
      std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;

      /// Incremental rotation
      double rotationStep = 0.2 * M_PI;
    };

    /// Constructor
    ///
    /// @param cfg Configuration struct
    /// @param logger The logging framework
    GeometryRotationDecorator(
        const Config&                       cfg,
        std::unique_ptr<const Acts::Logger> logger
        = Acts::getDefaultLogger("GeometryRotationDecorator",
                                 Acts::Logging::INFO));

    /// Virtual destructor
    virtual ~GeometryRotationDecorator() = default;

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
    std::string                         m_name = "GeometryRotationDecorator";

    /// Map of nominal transforms
    std::vector<Acts::Transform3D> m_nominalStore;

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_logger;
    }

    /// Populate the nominal transforms
    /// this parses the TrackingGeometry and fills the nominal store
    ///
    /// @param tGeometry the tracking geometry
    void
    parseGeometry(const Acts::TrackingGeometry& tGeometry);
  };
}  // namespace Alignable

}  // namespace FW

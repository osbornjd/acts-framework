// This file is part of the Acts project.
//
// Copyright (C) 2016-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Vertexing/FullBilloirVertexFitter.hpp"

namespace FWE {

class VertexFitAlgorithm : public FW::BareAlgorithm
{
public:
  struct Config
  {
    /// Input track collection
    std::string trackCollection;  ///< Input track collection

    /// Vertex fitter
    std::
        shared_ptr<Acts::
                       FullBilloirVertexFitter<Acts::ConstantBField,
                                               Acts::BoundParameters,
                                               Acts::
                                                   Propagator<Acts::
                                                                  EigenStepper<Acts::
                                                                                   ConstantBField>>>>
            vertexFitter = nullptr;

    bool doConstrainedFit = false;
  };

  /// Constructor
  VertexFitAlgorithm(const Config&        cfg,
                     Acts::Logging::Level level = Acts::Logging::INFO);

  /// Framework execute method
  /// @param [in] context is the Algorithm context for event consistency
  FW::ProcessCode
  execute(const FW::AlgorithmContext& context) const final override;

private:
  /// The config class
  Config m_cfg;
};

}  // namespace FWE
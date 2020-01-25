// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <map>
#include <string>
#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/EventData/SimHit.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimSourceLink.hpp"
#include "ACTFW/Utilities/Helpers.hpp"
#include "Acts/EventData/Measurement.hpp"
#include "Acts/EventData/MultiTrajectory.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/EventData/TrackState.hpp"
#include "Acts/Geometry/GeometryID.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace FW {

// Tools to make fake rate plots to show tracking fake rate.
class FakeRatePlotTool
{
public:
  /// @brief The nested configuration struct
  struct Config
  {
    std::map<std::string, PlotHelpers::Binning> varBinning = {
        {"Eta", PlotHelpers::Binning("#eta", 50, -4, 4)},
        {"Phi", PlotHelpers::Binning("#phi", 100, -3.15, 3.15)},
        {"Pt", PlotHelpers::Binning("pT [GeV/c]", 20, 0, 100)},
        {"Multiplicity", PlotHelpers::Binning("N_{tracks}", 20, -0.5, 19.5)}};
  };

  /// @brief Nested Cache struct
  struct FakeRatePlotCache
  {
    TH1F*        nRecoTracks;          ///< number of reco tracks
    TH1F*        nTruthMatchedTracks;  ///< number of truth-matched reco tracks
    TH1F*        nFakeTracks;          ///< number of fake tracks
    TEfficiency* fakerate_vs_eta;      ///< Tracking fake rate vs eta
    TEfficiency* fakerate_vs_phi;      ///< Tracking fake rate vs phi
    TEfficiency* fakerate_vs_pT;       ///< Tracking fake rate vs pT
  };

  /// Constructor
  ///
  /// @param cfg Configuration struct
  /// @param level Message level declaration
  FakeRatePlotTool(const Config&        cfg,
                   Acts::Logging::Level level = Acts::Logging::INFO);

  /// @brief book the fake rate plots
  /// @param fakeRatePlotCache the cache for fake rate plots
  void
  book(FakeRatePlotCache& fakeRatePlotCache) const;

  /// @brief fill fake rate plots
  ///
  /// @param fakeRatePlotCache cache object for fake rate plots
  /// @param truthParticle the truth Particle
  /// @param status the reconstruction status
  void
  fill(FakeRatePlotCache&       fakeRatePlotCache,
       const Data::SimParticle& truthParticle,
       bool                     status) const;

  /// @brief fill number of reco/truth-matched/fake tracks for a single
  /// Multi-trajectory
  ///
  /// @param fakeRatePlotCache cache object for fake rate plots
  /// @param nTruthMatchedTracks the number of truth-Matched tracks
  /// @param nFakeTracks the number of fake tracks
  void
  fill(FakeRatePlotCache& fakeRatePlotCache,
       const size_t&      nTruthMatchedTracks,
       const size_t&      nFakeTracks) const;

  /// @brief write the fake rate plots to file
  /// @param fakeRatePlotCache cache object for fake rate plots
  void
  write(const FakeRatePlotCache& fakeRatePlotCache) const;

  /// @brief delete the fake rate plots
  /// @param fakeRatePlotCache cache object for fake rate plots
  void
  clear(FakeRatePlotCache& fakeRatePlotCache) const;

private:
  Config                              m_cfg;     ///< The Config class
  std::unique_ptr<const Acts::Logger> m_logger;  ///< The logging instance

  /// The logger
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

}  // namespace FW

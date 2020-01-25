// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Validation/TrackSummaryPlotTool.hpp"

using Acts::VectorHelpers::eta;
using Acts::VectorHelpers::perp;

FW::TrackSummaryPlotTool::TrackSummaryPlotTool(
    const FW::TrackSummaryPlotTool::Config& cfg,
    Acts::Logging::Level                    level)
  : m_cfg(cfg), m_logger(Acts::getDefaultLogger("TrackSummaryPlotTool", level))
{
}

void
FW::TrackSummaryPlotTool::book(
    TrackSummaryPlotTool::RecoTrackPlotCache& recoTrackPlotCache) const
{
  PlotHelpers::Binning bEta  = m_cfg.varBinning.at("Eta");
  PlotHelpers::Binning bPt   = m_cfg.varBinning.at("Pt");
  PlotHelpers::Binning bMult = m_cfg.varBinning.at("Multiplicity");
  ACTS_DEBUG("Initialize the histograms for track info plots");
  // number of track states versus eta
  recoTrackPlotCache.nStates_vs_eta = PlotHelpers::bookProf(
      "nStates_vs_eta", "Number of total states vs. eta", bEta, bMult);
  // number of measurements versus eta
  recoTrackPlotCache.nMeasurements_vs_eta = PlotHelpers::bookProf(
      "nMeasurements_vs_eta", "Number of measurements vs. eta", bEta, bMult);
  // number of holes versus eta
  recoTrackPlotCache.nHoles_vs_eta = PlotHelpers::bookProf(
      "nHoles_vs_eta", "Number of holes vs. eta", bEta, bMult);
  // number of outliers versus eta
  recoTrackPlotCache.nOutliers_vs_eta = PlotHelpers::bookProf(
      "nOutliers_vs_eta", "Number of outliers vs. eta", bEta, bMult);
  // number of track states versus pt
  recoTrackPlotCache.nStates_vs_pt = PlotHelpers::bookProf(
      "nStates_vs_pt", "Number of total states vs. pt", bPt, bMult);
  // number of measurements versus pt
  recoTrackPlotCache.nMeasurements_vs_pt = PlotHelpers::bookProf(
      "nMeasurements_vs_pt", "Number of measurements vs. pt", bPt, bMult);
  // number of holes versus pt
  recoTrackPlotCache.nHoles_vs_pt = PlotHelpers::bookProf(
      "nHoles_vs_pt", "Number of holes vs. pt", bPt, bMult);
  // number of outliers versus pt
  recoTrackPlotCache.nOutliers_vs_pt = PlotHelpers::bookProf(
      "nOutliers_vs_pt", "Number of outliers vs. pt", bPt, bMult);
}

void
FW::TrackSummaryPlotTool::clear(RecoTrackPlotCache& recoTrackPlotCache) const
{
  delete recoTrackPlotCache.nStates_vs_eta;
  delete recoTrackPlotCache.nMeasurements_vs_eta;
  delete recoTrackPlotCache.nOutliers_vs_eta;
  delete recoTrackPlotCache.nHoles_vs_eta;
  delete recoTrackPlotCache.nStates_vs_pt;
  delete recoTrackPlotCache.nMeasurements_vs_pt;
  delete recoTrackPlotCache.nOutliers_vs_pt;
  delete recoTrackPlotCache.nHoles_vs_pt;
}

void
FW::TrackSummaryPlotTool::write(
    const TrackSummaryPlotTool::RecoTrackPlotCache& recoTrackPlotCache) const
{
  ACTS_DEBUG("Write the plots to output file.");
  recoTrackPlotCache.nStates_vs_eta->Write();
  recoTrackPlotCache.nMeasurements_vs_eta->Write();
  recoTrackPlotCache.nOutliers_vs_eta->Write();
  recoTrackPlotCache.nHoles_vs_eta->Write();
  recoTrackPlotCache.nStates_vs_pt->Write();
  recoTrackPlotCache.nMeasurements_vs_pt->Write();
  recoTrackPlotCache.nOutliers_vs_pt->Write();
  recoTrackPlotCache.nHoles_vs_pt->Write();
}

void
FW::TrackSummaryPlotTool::fill(
    TrackSummaryPlotTool::RecoTrackPlotCache& recoTrackPlotCache,
    const Data::SimParticle&                  truthParticle,
    const size_t&                             nStates,
    const size_t&                             nMeasurements,
    const size_t&                             nOutliers,
    const size_t&                             nHoles) const
{
  Acts::Vector3D truthMom = truthParticle.momentum();

  double t_eta = eta(truthMom);
  double t_pT  = perp(truthMom);

  PlotHelpers::fillProf(recoTrackPlotCache.nStates_vs_eta, t_eta, nStates);
  PlotHelpers::fillProf(
      recoTrackPlotCache.nMeasurements_vs_eta, t_eta, nMeasurements);
  PlotHelpers::fillProf(recoTrackPlotCache.nOutliers_vs_eta, t_eta, nOutliers);
  PlotHelpers::fillProf(recoTrackPlotCache.nHoles_vs_eta, t_eta, nHoles);
  PlotHelpers::fillProf(recoTrackPlotCache.nStates_vs_pt, t_pT, nStates);
  PlotHelpers::fillProf(
      recoTrackPlotCache.nMeasurements_vs_pt, t_pT, nMeasurements);
  PlotHelpers::fillProf(recoTrackPlotCache.nOutliers_vs_pt, t_pT, nOutliers);
  PlotHelpers::fillProf(recoTrackPlotCache.nHoles_vs_pt, t_pT, nHoles);
}

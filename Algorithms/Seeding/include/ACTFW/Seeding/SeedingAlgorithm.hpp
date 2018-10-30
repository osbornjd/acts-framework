// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_EXAMPLES_WHITEBOARDALGORITHM_H
#define ACTFW_EXAMPLES_WHITEBOARDALGORITHM_H 1

#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

#include "tricktrack/HitChainMaker.h"
#include "tricktrack/HitDoublets.h"
#include "tricktrack/SpacePoint.h"
#include "tricktrack/TripletFilter.h"
#include "tricktrack/CMGraphUtils.h"
#include "tricktrack/CMCell.h"



namespace FWE {

using Hit = tricktrack::SpacePoint<size_t>;
using namespace tricktrack;

void findTripletsForTest( std::vector<Hit>
                             barrel0,
                         std::vector<Hit>
                             barrel1,
                         std::vector<Hit>
                             barrel2,
                         std::vector<CMCell<Hit>::CMntuplet>& foundTracklets) {

  std::vector<HitDoublets<Hit>*> doublets;
  auto doublet1 = new HitDoublets<Hit>(barrel0, barrel1);
  auto doublet2 = new HitDoublets<Hit>(barrel1, barrel2);
  doublets.push_back(doublet1);
  doublets.push_back(doublet2);

  for (const auto& p0 : barrel0) {
    for (const auto& p1 : barrel1) {
      doublets[0]->add(p0.identifier(), p1.identifier());
    }
  }
  for (const auto& p1 : barrel1) {
    for (const auto& p2 : barrel2) {
      doublets[1]->add(p1.identifier(), p2.identifier());
    }
  }
  
  CMGraph g = createGraph({{"innerlayer", "middleLayer", "outerLayer"}});


  
  auto automaton = new HitChainMaker<Hit>(g);

  // create 
  TripletFilter<Hit> ff = std::bind(defaultGeometricFilter<Hit>, std::placeholders::_1,
  std::placeholders::_2,  
                                    0.8, // ptmin 
                                    0.,  // region_origin_x
                                    0.,  // region_origin_y
                                    0.002, // region_origin_radius
                                    0.2, // phiCut
                                    0.8, // hardPtCut
                                    0.2  // thetaCut
                                    );
  automaton->createAndConnectCells(doublets, ff);
  automaton->evolve(3);
  automaton->findNtuplets(foundTracklets, 3);
}

/// @class Algorithm
///
/// Test algorithm for the WhiteBoard writing/reading
///
class SeedingAlgorithm : public FW::BareAlgorithm
{
public:
  /// @class Config
  /// Nested Configuration class for the SeedingAlgorithm
  /// It extends the Algorithm::Config Class
  struct Config
  {
    // Input collection of DataClassOne (optional)
    std::string inputClassOneCollection = "";
    // Output collection of DataClassOne (optional)
    std::string outputClassOneCollection = "";
    // Input collection of DataClassTwo (optional)
    std::string inputClassTwoCollection = "";
    // Output collection of DataClassTwo (optional)
    std::string outputClassTwoCollection = "";
  };

  /// Constructor
  ///
  /// @param cfg is the configruation
  SeedingAlgorithm(const Config&        cfg,
                      Acts::Logging::Level level = Acts::Logging::INFO);

  /// Framework execode method
  FW::ProcessCode
  execute(FW::AlgorithmContext ctx) const final override;

private:
  Config m_cfg;
};

}  // namespace FWE

#include "SeedingAlgorithm.ipp"

#endif

// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "DataClassOne.hpp"
#include "DataClassTwo.hpp"


FWE::SeedingAlgorithm::SeedingAlgorithm(const Config&        cfg,
                                              Acts::Logging::Level level)
  : FW::BareAlgorithm("SeedingAlgorithm", level), m_cfg(cfg)
{
}

FW::ProcessCode
FWE::SeedingAlgorithm::execute(FW::AlgorithmContext ctx) const
{
  // -------- Reading -----------------------
  // Reading Class One
  if (!m_cfg.inputClassOneCollection.empty()) {
    ACTS_INFO("Reading ClassOneCollection " << m_cfg.inputClassOneCollection);
    // read in the collection
    const FWE::DataClassOneCollection* dcoCollIn = nullptr;
    // write to the EventStore
    if (ctx.eventStore.get(m_cfg.inputClassOneCollection, dcoCollIn)
        == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
    // screen output
    ACTS_VERBOSE("Read DataClassOneCollection with size " << dcoCollIn->size());
    for (auto& idco : (*dcoCollIn))
      ACTS_VERBOSE("Read in  DataClassOne object as " << idco.data());
  }



  // Writing Class Two
  if (!m_cfg.outputClassTwoCollection.empty()) {
    ACTS_INFO("Writing ClassTwoCollection " << m_cfg.outputClassTwoCollection);
    // create a new collection
    DataClassTwoCollection dctCollOut = {{"Two", double(ctx.eventNumber)}};
    ACTS_VERBOSE("Written out DataClassTwo object as "
                 << dctCollOut.back().data());
    // write to the EventStore
    if (ctx.eventStore.add(m_cfg.outputClassTwoCollection,
                           std::move(dctCollOut))
        == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
  }

  std::vector<Hit> inner_hits;
  inner_hits.push_back(Hit(0, 1, 0, 0));
  inner_hits.push_back(Hit(0, 1.001, 0, 1));
  std::vector<Hit> middle_hits;
  middle_hits.push_back(Hit(0, 2, 0, 0));
  middle_hits.push_back(Hit(0, 2.002, 0, 1));
  std::vector<Hit> outer_hits;
  outer_hits.push_back(Hit(0, 3, 0, 0));
  outer_hits.push_back(Hit(0, 3.003, 0, 1));

  std::vector<CMCell<Hit>::CMntuplet> foundTracklets;
  findTripletsForTest(inner_hits, middle_hits, outer_hits, foundTracklets);
  std::cout << "found " << foundTracklets.size() << " triplets" << std::endl;
  return FW::ProcessCode::SUCCESS;
}

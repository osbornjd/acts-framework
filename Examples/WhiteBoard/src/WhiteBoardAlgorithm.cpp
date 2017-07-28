#include <iostream>

#include "ACTFW/Framework/WhiteBoard.hpp"
#include "DataClassOne.hpp"
#include "DataClassTwo.hpp"
#include "WhiteBoardAlgorithm.hpp"

FWE::WhiteBoardAlgorithm::WhiteBoardAlgorithm(const Config& cfg)
  : FW::BareAlgorithm("WhiteBoardAlgorithm"), m_cfg(cfg)
{
}

FW::ProcessCode
FWE::WhiteBoardAlgorithm::execute(FW::AlgorithmContext context) const
{
  // Retrieve relevant information from the execution context
  size_t eventNumber = context.eventNumber;
  auto   eventStore  = context.eventStore;

  // -------- Reading -----------------------
  // Reading Class One
  if (!m_cfg.inputClassOneCollection.empty()) {
    ACTS_INFO("Reading ClassOneCollection " << m_cfg.inputClassOneCollection);
    // read in the collection
    const FWE::DataClassOneCollection* dcoCollIn = nullptr;
    // write to the EventStore
    if (eventStore
        && eventStore->get(m_cfg.inputClassOneCollection, dcoCollIn)
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
    // screen output
    ACTS_VERBOSE("Read DataClassOneCollection with size " << dcoCollIn->size());
    for (auto& idco : (*dcoCollIn))
      ACTS_VERBOSE("Read in  DataClassOne object as " << idco.data());
  }

  // Reading Class Two
  if (!m_cfg.inputClassTwoCollection.empty()) {
    ACTS_INFO("Reading ClassTwoCollection " << m_cfg.inputClassTwoCollection);
    // read in the collection
    const FWE::DataClassTwoCollection* dctCollIn = nullptr;
    // write to the EventStore
    if (eventStore
        && eventStore->get(m_cfg.inputClassTwoCollection, dctCollIn)
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
    // screen output
    ACTS_VERBOSE("Read DataClassTwoCollection with size " << dctCollIn->size());
    for (auto& idct : (*dctCollIn))
      ACTS_VERBOSE("Read in  DataClassTwo object as " << idct.data());
  }

  // ---------- Writing -----------------------
  // Writing Class One
  if (!m_cfg.outputClassOneCollection.empty()) {
    ACTS_INFO("Writing ClassOneCollection " << m_cfg.outputClassOneCollection);
    // create a new collection
    DataClassOneCollection dcoCollOut = {{"One", eventNumber}};
    ACTS_VERBOSE("Written out DataClassOne object as "
                 << dcoCollOut.back().data());
    // write to the EventStore
    if (eventStore
        && eventStore->add(m_cfg.outputClassOneCollection,
                           std::move(dcoCollOut))
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
  }

  // Writing Class Two
  if (!m_cfg.outputClassTwoCollection.empty()) {
    ACTS_INFO("Writing ClassTwoCollection " << m_cfg.outputClassTwoCollection);
    // create a new collection
    DataClassTwoCollection dctCollOut = {{"Two", double(eventNumber)}};
    ACTS_VERBOSE("Written out DataClassTwo object as "
                 << dctCollOut.back().data());
    // write to the EventStore
    if (eventStore
        && eventStore->add(m_cfg.outputClassTwoCollection,
                           std::move(dctCollOut))
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
  }
  // Return with success
  return FW::ProcessCode::SUCCESS;
}

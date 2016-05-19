#include "ACTFW/Framework/MsgStreamMacros.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "WhiteBoardAlgorithm.hpp"
#include "DataClassOne.hpp"
#include "DataClassTwo.hpp"
#include <iostream>

FWE::WhiteBoardAlgorithm::WhiteBoardAlgorithm(const Config& cfg) :
    FW::Algorithm(cfg),
    m_cfg(cfg)
{}

FWE::WhiteBoardAlgorithm::~WhiteBoardAlgorithm()
{}

/** Framework finalize mehtod */
FW::ProcessCode FWE::WhiteBoardAlgorithm::initialize(std::shared_ptr<FW::WhiteBoard> eStore,
                                                     std::shared_ptr<FW::WhiteBoard> jStore)
{
    // call the algorithm initialize for setting the stores
    if ( FW::Algorithm::initialize(eStore,jStore) != FW::ProcessCode::SUCCESS){
        MSG_FATAL("Algorithm::initialize() did not succeed!");
        return FW::ProcessCode::SUCCESS;
    }
    MSG_VERBOSE("initialize successful.");
    return FW::ProcessCode::SUCCESS;
}

/** Framework execode method */
FW::ProcessCode FWE::WhiteBoardAlgorithm::execute(size_t eventNumber)
{
    
    // -------- Reading -----------------------
    // Reading Class One
    if (!m_cfg.inputClassOneCollection.empty()){
        MSG_INFO("Reading ClassOneCollection " << m_cfg.inputClassOneCollection);
        // read in the collection
        FWE::DataClassOneCollection* dcoCollIn = nullptr;
        // write to the EventStore
        if (eventStore() && eventStore()->readT(dcoCollIn,m_cfg.inputClassOneCollection) == FW::ProcessCode::ABORT)
            return FW::ProcessCode::ABORT;
        // screen output
        MSG_VERBOSE("Read DataClassOneCollection with size " << dcoCollIn->size());
        for (auto& idco : (*dcoCollIn))
            MSG_VERBOSE("Read in  DataClassOne object as " << idco->data());
    }
    
    // Reading Class Two
    if (!m_cfg.inputClassTwoCollection.empty()){
        MSG_INFO("Reading ClassTwoCollection " << m_cfg.inputClassTwoCollection);
        // read in the collection
        FWE::DataClassTwoCollection* dctCollIn = nullptr;
        // write to the EventStore
        if (eventStore() && eventStore()->readT(dctCollIn,m_cfg.inputClassTwoCollection) == FW::ProcessCode::ABORT)
            return FW::ProcessCode::ABORT;
        // screen output
        MSG_VERBOSE("Read DataClassTwoCollection with size " << dctCollIn->size());
        for (auto& idct : (*dctCollIn))
            MSG_VERBOSE("Read in  DataClassTwo object as " << idct->data());
    }
    
    // ---------- Writing -----------------------
    // Writing Class One
    if (!m_cfg.outputClassOneCollection.empty()){
        MSG_INFO("Writing ClassOneCollection " << m_cfg.outputClassOneCollection);
        // create a new collection
        DataClassOneCollection* dcoCollOut = new DataClassOneCollection;
        std::unique_ptr<DataClassOne> dcoOut(new DataClassOne("One", eventNumber));
        // screen output
        MSG_VERBOSE("Written out DataClassOne object as " << dcoOut->data());
        dcoCollOut->push_back(std::move(dcoOut));
        // write to the EventStore
        if (eventStore() && eventStore()->writeT(dcoCollOut,m_cfg.outputClassOneCollection) == FW::ProcessCode::ABORT)
            return FW::ProcessCode::ABORT;

    }
    
    // Writing Class Two
    if (!m_cfg.outputClassTwoCollection.empty()){
        MSG_INFO("Writing ClassTwoCollection " << m_cfg.outputClassTwoCollection);
        // create a new collection
        DataClassTwoCollection* dctCollOut = new DataClassTwoCollection;
        std::unique_ptr<DataClassTwo> dctOut(new DataClassTwo("Two", eventNumber));
        // screen output
        MSG_VERBOSE("Written out DataClassTwo object as " << dctOut->data());
        dctCollOut->push_back(std::move(dctOut));
        // write to the EventStore
        if (eventStore() && eventStore()->writeT(dctCollOut,m_cfg.outputClassTwoCollection) == FW::ProcessCode::ABORT)
           return FW::ProcessCode::ABORT;
    }
    // Return with success
    return FW::ProcessCode::SUCCESS;
}

/** Framework finalize mehtod */
FW::ProcessCode FWE::WhiteBoardAlgorithm::finalize()
{
    // final screen output
    MSG_VERBOSE("finalize successful.");
    return FW::ProcessCode::SUCCESS;
}

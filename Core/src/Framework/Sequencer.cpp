#include "ACTFW/Framework/Sequencer.hpp"

FW::Sequencer::Sequencer(const Sequencer::Config& cfg):
  m_cfg(cfg)
{}

FW::Sequencer::~Sequencer(){}

FW::ProcessCode FW::Sequencer::initializeEventLoop()
{

    MSG_INFO("=================================================================");
    MSG_INFO("Initializing the event loop for:");
    MSG_INFO("      -> " << m_cfg.ioAlgorithms.size() << " IO Algorithms");
    MSG_INFO("      -> " << m_cfg.eventAlgorithms.size() << " Event Algorithms");

    // initialize the i/o algorithms
    for (auto& ioalg: m_cfg.ioAlgorithms){
        if (ioalg->initialize(m_cfg.eventBoard,m_cfg.jobBoard) != ProcessCode::SUCCESS)
            return ProcessCode::ABORT;
    }
    
    // initialize the event algorithms
    for (auto& alg: m_cfg.eventAlgorithms){
        if (alg->initialize(m_cfg.eventBoard,m_cfg.jobBoard) != ProcessCode::SUCCESS)
            return ProcessCode::ABORT;
    }
    // return with success
    return ProcessCode::SUCCESS;
}

FW::ProcessCode FW::Sequencer::processEventLoop(size_t nEvents, size_t skipEvents)
{
    MSG_INFO("=================================================================");
    MSG_INFO("Processing the event loop:");

    // skip the events if necessary
    if (skipEvents){
        MSG_INFO("==> SKIP   " << skipEvents << " events.");
        for (auto& ioalg: m_cfg.ioAlgorithms)
            if (ioalg->skip(skipEvents) != ProcessCode::SUCCESS)
                return ProcessCode::ABORT;
    }
    // execute the event loop
    for (size_t ievent = 0; ievent < nEvents; ++ievent){
        MSG_INFO("==> EVENT " << skipEvents+ievent << " <== start. ");
        // a) then call read on all io algoirhtms
        for (auto& ioalg: m_cfg.ioAlgorithms){
            if (ioalg->read(skipEvents+ievent) != ProcessCode::SUCCESS)
                return ProcessCode::ABORT;
        }
        // b) now call execute for all event algorithms
        for (auto& alg: m_cfg.eventAlgorithms){
            if (alg->execute(skipEvents+ievent) != ProcessCode::SUCCESS)
                return ProcessCode::ABORT;
        }
        // c) now call write to all io algoirhtms
        for (auto& ioalg: m_cfg.ioAlgorithms){
            if (ioalg->write(skipEvents+ievent) != ProcessCode::SUCCESS)
                return ProcessCode::ABORT;
        }
        MSG_INFO("<== EVENT " << skipEvents+ievent << " ==> done. ");
    }
    // return with success
    return ProcessCode::SUCCESS;

}

FW::ProcessCode FW::Sequencer::finalizeEventLoop()
{
    
    MSG_INFO("=================================================================");
    MSG_INFO("Finalize the event loop for:");
    MSG_INFO("      -> " << m_cfg.ioAlgorithms.size() << " IO Algorithms");
    MSG_INFO("      -> " << m_cfg.eventAlgorithms.size() << " Event Algorithms");

    // initialize the i/o algorithms
    for (auto& ioalg: m_cfg.ioAlgorithms){
        if (ioalg->finalize() != ProcessCode::SUCCESS)
            return ProcessCode::ABORT;
    }
    
    // initialize the event algorithms
    for (auto& alg: m_cfg.eventAlgorithms){
        if (alg->finalize() != ProcessCode::SUCCESS)
            return ProcessCode::ABORT;
    }
    // return with success
    return ProcessCode::SUCCESS;
}



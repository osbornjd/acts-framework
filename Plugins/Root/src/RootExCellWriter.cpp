
#include "ACTFW/Root/RootExCellWriter.hpp"
#include <iostream>

#include "TFile.h"

FWRoot::RootExCellWriter::RootExCellWriter(const FWRoot::RootExCellWriter::Config& cfg) :
FW::IExtrapolationCellWriter(),
m_cfg(cfg),
m_outputFile(nullptr),
m_outputTree(nullptr)
{

}

FWRoot::RootExCellWriter::~RootExCellWriter()
{}


/** Framework intialize method */
FW::ProcessCode FWRoot::RootExCellWriter::initialize()
{

    MSG_INFO("Registering new ROOT output File : " << m_cfg.fileName);
    // open the output file
    m_outputFile = new TFile(m_cfg.fileName.c_str(), "recreate");
    m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());
    
    m_positionX.reserve(MAXSTEPS);
    m_positionY.reserve(MAXSTEPS);
    m_positionZ.reserve(MAXSTEPS);
    
    m_outputTree->Branch("StepX",&m_positionX);
    m_outputTree->Branch("StepY",&m_positionY);
    m_outputTree->Branch("StepZ",&m_positionZ);
    
    return FW::ProcessCode::SUCCESS;
}

/** Framework finalize mehtod */
FW::ProcessCode FWRoot::RootExCellWriter::finalize()
{
    
    MSG_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
    m_outputFile->cd();
    m_outputTree->Write();
    m_outputFile->Close();
    
    return FW::ProcessCode::SUCCESS;
}

/** The write interface */
FW::ProcessCode FWRoot::RootExCellWriter::write(const Acts::ExCellCharged& eCell)
{
    return writeT<Acts::TrackParameters>(eCell);
}
                                                
FW::ProcessCode FWRoot::RootExCellWriter::write(const Acts::ExCellNeutral& eCell)
{

    return writeT<Acts::NeutralParameters>(eCell);
}


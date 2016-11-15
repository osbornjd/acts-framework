#include "ACTFW/Root/RootMaterialTrackRecWriter.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialStep.hpp"

#include <iostream>

#include "TFile.h"

FWRoot::RootMaterialTrackRecWriter::RootMaterialTrackRecWriter(const FWRoot::RootMaterialTrackRecWriter::Config& cfg) :
FW::IMaterialTrackRecWriter(),
m_cfg(cfg),
m_outputFile(nullptr),
m_outputTree(nullptr),
m_trackRecords()
{}

FW::ProcessCode FWRoot::RootMaterialTrackRecWriter::initialize()
{
    ACTS_INFO("Registering new ROOT output File : " << m_cfg.fileName);
    // open the output file
    m_outputFile = new TFile(m_cfg.fileName.c_str(), "recreate");
    // create the output tree
    m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());
    // create a branch with the MaterialTrackRecord entities
    m_outputTree->Branch("MaterialTrackRecords", &m_trackRecords);
    return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode FWRoot::RootMaterialTrackRecWriter::finalize()
{
    // write the tree and close the file
    ACTS_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
    m_outputFile->cd();
    m_outputTree->Write();
    m_outputFile->Close();
    return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode FWRoot::RootMaterialTrackRecWriter::write(const Acts::MaterialTrackRecord& mtrecord)
{
    // setting the parameters
    m_trackRecords = mtrecord;

    // write to
    m_outputTree->Fill();
    
    // return success
    return FW::ProcessCode::SUCCESS;
}
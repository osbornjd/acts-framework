#include "ACTFW/Root/RootMaterialTrackRecReader.hpp"
#include <iostream>
#include "TFile.h"
#include "TTreeReader.h"

FWRoot::RootMaterialTrackRecReader::RootMaterialTrackRecReader(const FWRoot::RootMaterialTrackRecReader::Config& cfg) :
FW::IMaterialTrackRecReader(),
m_cfg(cfg),
m_inputFile(nullptr),
m_inputTree(nullptr),
m_records()
{}

FW::ProcessCode FWRoot::RootMaterialTrackRecReader::initialize()
{
    return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode FWRoot::RootMaterialTrackRecReader::finalize()
{
    return FW::ProcessCode::SUCCESS;
}


FW::ProcessCode FWRoot::RootMaterialTrackRecReader::read()
{
    
    ACTS_INFO("Registering new ROOT input File : " << m_cfg.fileName);
    // open the input file
    m_inputFile = new TFile(m_cfg.fileName.c_str());
    if (!m_inputFile) ACTS_ERROR("Could not open input file : " << m_cfg.fileName);
    // access the input tree
    ACTS_VERBOSE("Accessing TTree with name :  " << m_cfg.treeName << ".");
    TTreeReader reader(m_cfg.treeName.c_str(),m_inputFile);
    // get the MaterialTrackRecord entities
    TTreeReaderValue<Acts::MaterialTrackRecord> mtRecord(reader, "MaterialTrackRecords");
    while(reader.Next()){
        m_records.push_back(*mtRecord);
    }
    /// set the tree @TODO see if needed
    m_inputTree = (TTree*)m_inputFile->Get(m_cfg.treeName.c_str());
    // return scuess
    return FW::ProcessCode::SUCCESS;
}
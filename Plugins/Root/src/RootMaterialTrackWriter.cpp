#include "ACTFW/Plugins/Root/RootMaterialTrackWriter.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialStep.hpp"

#include <iostream>

#include "TFile.h"

FW::Root::RootMaterialTrackWriter::RootMaterialTrackWriter(
    const FW::Root::RootMaterialTrackWriter::Config& cfg)
  : FW::IWriterT<Acts::MaterialTrack>()
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
  , m_trackRecord()
{
}

FW::ProcessCode
FW::Root::RootMaterialTrackWriter::initialize()
{
  
  ACTS_INFO("Registering new ROOT output File : " << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), "recreate");
  // create the output tree
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());
  // create a branch with the MaterialTrack entities
  m_outputTree->Branch("MaterialTrack", &m_trackRecord);
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::RootMaterialTrackWriter::finalize()
{
  // write the tree and close the file
  ACTS_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();
  
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::RootMaterialTrackWriter::write(
    const Acts::MaterialTrack& mtrecord)
{
  
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_write_mutex);
  
  // setting the parameters
  m_trackRecord = mtrecord;

  // write to
  m_outputTree->Fill();

  // return success
  return FW::ProcessCode::SUCCESS;
}

 
FW::ProcessCode
FW::Root::RootMaterialTrackWriter::write(const std::string&) 
{ 
  return FW::ProcessCode::SUCCESS;
}
 

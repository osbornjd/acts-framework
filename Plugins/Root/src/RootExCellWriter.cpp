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
{
}

FW::ProcessCode FWRoot::RootExCellWriter::initialize()
{
  
  ACTS_INFO("Registering new ROOT output File : " << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), "recreate");
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());
  
  m_positionX.reserve(MAXSTEPS);
  m_positionY.reserve(MAXSTEPS);
  m_positionZ.reserve(MAXSTEPS);
  
  m_material.reserve(MAXSTEPS);
  m_sensitive.reserve(MAXSTEPS);
  m_boundary.reserve(MAXSTEPS);

  m_volumeID.reserve(MAXSTEPS);
  m_layerID.reserve(MAXSTEPS);
  m_surfaceID.reserve(MAXSTEPS);
  
  m_localposition0.reserve(MAXSTEPS);
  m_localposition1.reserve(MAXSTEPS);
  
  m_outputTree->Branch("StepX",&m_positionX);
  m_outputTree->Branch("StepY",&m_positionY);
  m_outputTree->Branch("StepZ",&m_positionZ);
  
  m_outputTree->Branch("MaterialStep",  &m_material);
  m_outputTree->Branch("BoundaryStep",  &m_material);
  m_outputTree->Branch("SensitiveStep", &m_material);

  m_outputTree->Branch("VolumeID",  &m_volumeID);
  m_outputTree->Branch("LayerID",   &m_layerID);
  m_outputTree->Branch("SurfaceID", &m_surfaceID);
  
  m_outputTree->Branch("Local0",&m_localposition0);
  m_outputTree->Branch("Local1",&m_localposition1);
  
  
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode FWRoot::RootExCellWriter::finalize()
{
  
  ACTS_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
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


#include <iostream>

#include <TFile.h>

#include "ACTFW/Root/RootExCellWriter.hpp"

FWRoot::RootExCellWriter::RootExCellWriter(
    const FWRoot::RootExCellWriter::Config& cfg)
  : FW::IExtrapolationCellWriter()
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
}

FWRoot::RootExCellWriter::~RootExCellWriter()
{
}

FW::ProcessCode
FWRoot::RootExCellWriter::initialize()
{
  ACTS_INFO("Registering new ROOT output File : " << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), m_cfg.fileMode.c_str());
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());

  // initial parameters
  // @TODO implement smearing
  m_outputTree->Branch("Eta", &m_eta);
  m_outputTree->Branch("Phi", &m_phi);
  
  // output the step information
  m_outputTree->Branch("StepX", &m_s_positionX);
  m_outputTree->Branch("StepY", &m_s_positionY);
  m_outputTree->Branch("StepZ", &m_s_positionZ);
  m_outputTree->Branch("StepR", &m_s_positionR);

  // identification
  m_outputTree->Branch("StepVolumeID",  &m_s_volumeID);
  m_outputTree->Branch("StepLayerID",   &m_s_layerID);
  m_outputTree->Branch("StepSurfaceID", &m_s_surfaceID);
  // material section
  if (m_cfg.writeMaterial){
    m_outputTree->Branch("MaterialX0",     &m_materialX0);
    m_outputTree->Branch("MaterialL0",     &m_materialL0);
    m_outputTree->Branch("StepMaterialX0", &m_s_materialX0);
    m_outputTree->Branch("StepMaterialL0", &m_s_materialL0);
    m_outputTree->Branch("MaterialStep",   &m_s_material);
  }
  // sensitive section
  if (m_cfg.writeSensitive){
    m_outputTree->Branch("SensitiveStep", &m_s_sensitive);
    m_outputTree->Branch("StepLocal0",    &m_s_localposition0);
    m_outputTree->Branch("StepLocal1",    &m_s_localposition1);
  }
  // boundary section
  if (m_cfg.writeBoundary)
    m_outputTree->Branch("BoundaryStep", &m_s_boundary);
  // numer of sensitive hits
  m_outputTree->Branch("nHits", &m_hits);

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::RootExCellWriter::finalize()
{
  ACTS_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return FW::ProcessCode::SUCCESS;
}

/** The write interface */
FW::ProcessCode
FWRoot::RootExCellWriter::write(const Acts::ExCellCharged& eCell)
{
  return writeT<Acts::TrackParameters>(eCell);
}

FW::ProcessCode
FWRoot::RootExCellWriter::write(const Acts::ExCellNeutral& eCell)
{
  return writeT<Acts::NeutralParameters>(eCell);
}

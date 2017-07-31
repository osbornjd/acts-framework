#include "ACTFW/Plugins/Root/MaterialEffectsWriter.hpp"
#include <TFile.h>
#include <iostream>

FWRoot::MaterialEffectsWriter::MaterialEffectsWriter(
    const FWRoot::MaterialEffectsWriter::Config& cfg)
  : FW::IWriterT<std::pair<Acts::ParticleProperties,
                           Acts::ParticleProperties>>()
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
}

FWRoot::MaterialEffectsWriter::~MaterialEffectsWriter()
{
}

FW::ProcessCode
FWRoot::MaterialEffectsWriter::initialize()
{
  ACTS_INFO("Registering new ROOT output File : " << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), m_cfg.fileMode.c_str());
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());

  m_outputTree->Branch("r0", &m_r0);
  m_outputTree->Branch("r1", &m_r1);

  m_outputTree->Branch("pt0", &m_pt0);
  m_outputTree->Branch("pt1", &m_pt1);

  m_outputTree->Branch("dx", &m_dx);
  m_outputTree->Branch("dy", &m_dy);
  m_outputTree->Branch("dz", &m_dz);
  m_outputTree->Branch("dr", &m_dr);

  m_outputTree->Branch("dPx", &m_dPx);
  m_outputTree->Branch("dPy", &m_dPy);
  m_outputTree->Branch("dPz", &m_dPz);
  m_outputTree->Branch("dPt", &m_dPt);
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::MaterialEffectsWriter::finalize()
{
  ACTS_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::MaterialEffectsWriter::write(
    const std::pair<Acts::ParticleProperties, Acts::ParticleProperties>&
        pProperties)
{
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_write_mutex);

  // initial parameters
  if (!m_outputTree) std::cout << "!Tree" << std::endl;

  m_r0 = pProperties.first.vertex().perp();
  m_r1 = pProperties.second.vertex().perp();

  m_pt0 = pProperties.first.momentum().perp();
  m_pt1 = pProperties.second.momentum().perp();

  m_dx = fabs(pProperties.first.vertex().x() - pProperties.second.vertex().x());
  m_dy = fabs(pProperties.first.vertex().y() - pProperties.second.vertex().y());
  m_dz = fabs(pProperties.first.vertex().z() - pProperties.second.vertex().z());
  m_dr = fabs(m_r0 - m_r1);

  m_dPx = fabs(pProperties.first.momentum().x()
               - pProperties.second.momentum().x());
  m_dPy = fabs(pProperties.first.momentum().y()
               - pProperties.second.momentum().y());
  m_dPz = fabs(pProperties.first.momentum().z()
               - pProperties.second.momentum().z());
  m_dPt = fabs(pProperties.first.momentum().perp()
               - pProperties.second.momentum().perp());

  m_outputTree->Fill();

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::MaterialEffectsWriter::write(const std::string&)
{
  return FW::ProcessCode::SUCCESS;
}

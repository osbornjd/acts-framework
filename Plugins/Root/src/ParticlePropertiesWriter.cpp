#include <iostream>
#include <TFile.h>
#include "ACTFW/Root/ParticlePropertiesWriter.hpp"

FWRoot::ParticlePropertiesWriter::ParticlePropertiesWriter(
    const FWRoot::ParticlePropertiesWriter::Config& cfg)
  : FW::IParticlePropertiesWriter()
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
}

FWRoot::ParticlePropertiesWriter::~ParticlePropertiesWriter()
{
}

FW::ProcessCode
FWRoot::ParticlePropertiesWriter::initialize()
{
  ACTS_INFO("Registering new ROOT output File : " << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), "recreate");
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());

  // initial parameters
  m_outputTree->Branch("Eta",     &m_eta);
  m_outputTree->Branch("Phi",     &m_phi);
  m_outputTree->Branch("Vx",      &m_vx);
  m_outputTree->Branch("Vy",      &m_vy);
  m_outputTree->Branch("Vz",      &m_vz);
  m_outputTree->Branch("Px",      &m_px);
  m_outputTree->Branch("Py",      &m_py);
  m_outputTree->Branch("Pz",      &m_pz);
  m_outputTree->Branch("Pt",      &m_pT);
  m_outputTree->Branch("Charge",  &m_charge);
  m_outputTree->Branch("Mass",    &m_mass);
  m_outputTree->Branch("PdgCode", &m_pdgCode);

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::ParticlePropertiesWriter::finalize()
{
  ACTS_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::ParticlePropertiesWriter::write(const std::vector<Acts::ParticleProperties>& pProperties)
{
  size_t nParticles = pProperties.size();
    
  m_eta.clear();
  m_phi.clear();
  m_vx.clear();
  m_vy.clear();
  m_vz.clear();
  m_px.clear();
  m_py.clear();
  m_pz.clear();
  m_pT.clear();
  m_charge.clear();
  m_mass.clear();
  m_pdgCode.clear();
  m_eta.reserve(nParticles);
  m_phi.reserve(nParticles);
  m_vx.reserve(nParticles);
  m_vy.reserve(nParticles);
  m_vz.reserve(nParticles);
  m_px.reserve(nParticles);
  m_py.reserve(nParticles);
  m_pz.reserve(nParticles);
  m_pT.reserve(nParticles);
  m_charge.reserve(nParticles);
  m_mass.reserve(nParticles);
  m_pdgCode.reserve(nParticles);
  // loop and fill
  for (auto& particle : pProperties){
    /// collect the information
    m_vx.push_back(particle.vertex().x());
    m_vy.push_back(particle.vertex().y());
    m_vz.push_back(particle.vertex().z());
    m_eta.push_back(particle.momentum().eta());
    m_phi.push_back(particle.momentum().phi());
    m_px.push_back(particle.momentum().x());
    m_py.push_back(particle.momentum().y());
    m_pz.push_back(particle.momentum().z());
    m_pT.push_back(particle.momentum().perp());
    m_charge.push_back(particle.charge());
    m_mass.push_back(particle.mass());
    m_pdgCode.push_back(particle.pdgID());
  }
  // fill the tree
  if (m_outputTree) m_outputTree->Fill();
    
  return FW::ProcessCode::SUCCESS;
}



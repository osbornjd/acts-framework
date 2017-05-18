#include <iostream>
#include <TFile.h>
#include "ACTFW/Plugins/Root/ParticlePropertiesWriter.hpp"

FWRoot::ParticlePropertiesWriter::ParticlePropertiesWriter(
    const FWRoot::ParticlePropertiesWriter::Config& cfg)
  : FW::IWriterT<const std::vector<Acts::ParticleProperties> >()
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
  m_outputFile = new TFile(m_cfg.fileName.c_str(), m_cfg.fileMode.c_str());
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());

  // initial parameters
  m_outputTree->Branch("eta",        &m_eta);
  m_outputTree->Branch("phi",        &m_phi);
  m_outputTree->Branch("vx",         &m_vx);
  m_outputTree->Branch("vy",         &m_vy);
  m_outputTree->Branch("vz",         &m_vz);
  m_outputTree->Branch("px",         &m_px);
  m_outputTree->Branch("py",         &m_py);
  m_outputTree->Branch("pz",         &m_pz);
  m_outputTree->Branch("pt",         &m_pT);
  m_outputTree->Branch("charge",     &m_charge);
  m_outputTree->Branch("mass",       &m_mass);
  m_outputTree->Branch("pdg",        &m_pdgCode);
  m_outputTree->Branch("barcode",    &m_barcode);
  m_outputTree->Branch("vertex",     &m_vertex);
  m_outputTree->Branch("primary",    &m_primary);
  m_outputTree->Branch("generation", &m_generation);
  m_outputTree->Branch("secondary",  &m_secondary);
  m_outputTree->Branch("process",    &m_process);
  
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
FWRoot::ParticlePropertiesWriter::write(const std::vector<Acts::ParticleProperties>& particles)
{
  
  // the number of particles
  size_t nParticles = particles.size();
  // clear the branches    
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
  m_barcode.clear();
  m_vertex.clear();
  m_primary.clear();
  m_generation.clear();
  m_secondary.clear();
  m_process.clear();
  // and reserve the appropriately
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
  m_barcode.reserve(nParticles);
  m_pdgCode.reserve(nParticles);
  m_vertex.reserve(nParticles);
  m_primary.reserve(nParticles);
  m_generation.reserve(nParticles);
  m_secondary.reserve(nParticles);
  m_process.reserve(nParticles);
  // loop and fill
  for ( auto& particle : particles){
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
    m_barcode.push_back(particle.barcode());
    // decode using the barcode service
    if (m_cfg.barcodeSvc){
      // the barcode service
      m_vertex.push_back(m_cfg.barcodeSvc->vertex(particle.barcode()));
      m_primary.push_back(m_cfg.barcodeSvc->primary(particle.barcode()));
      m_generation.push_back(m_cfg.barcodeSvc->generate(particle.barcode()));
      m_secondary.push_back(m_cfg.barcodeSvc->secondary(particle.barcode()));
      m_process.push_back(m_cfg.barcodeSvc->process(particle.barcode()));
    }
  }
  // fill the tree
  if (m_outputTree)
    m_outputTree->Fill();
    
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::ParticlePropertiesWriter::write(const std::string& sinfo)
{
  return FW::ProcessCode::SUCCESS;  
}



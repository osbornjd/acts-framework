#include <ios>
#include <stdexcept>
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include <TFile.h>
#include <TTree.h>

FW::Root::RootParticleWriter::RootParticleWriter(
    const FW::Root::RootParticleWriter::Config& cfg,
    Acts::Logging::Level                        level)
  : Base(cfg.collection, "RootParticleWriter", level)
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
  // An input collection name and tree name must be specified
  if (m_cfg.collection.empty()) {
    throw std::invalid_argument("Missing input collection");
  } else if (m_cfg.treeName.empty()) {
    throw std::invalid_argument("Missing tree name");
  }

  // Setup ROOT I/O
  m_outputFile = TFile::Open(m_cfg.filePath.c_str(), m_cfg.fileMode.c_str());
  if (!m_outputFile) {
    throw std::ios_base::failure("Could not open '" + m_cfg.filePath);
  }
  m_outputFile->cd();
  m_outputTree = new TTree(m_cfg.treeName.c_str(),m_cfg.treeName.c_str());
  if (!m_outputTree) throw std::bad_alloc();

  // Initial parameters
  m_outputTree->Branch("eta", &m_eta);
  m_outputTree->Branch("phi", &m_phi);
  m_outputTree->Branch("vx", &m_vx);
  m_outputTree->Branch("vy", &m_vy);
  m_outputTree->Branch("vz", &m_vz);
  m_outputTree->Branch("px", &m_px);
  m_outputTree->Branch("py", &m_py);
  m_outputTree->Branch("pz", &m_pz);
  m_outputTree->Branch("pt", &m_pT);
  m_outputTree->Branch("charge", &m_charge);
  m_outputTree->Branch("mass", &m_mass);
  m_outputTree->Branch("pdg", &m_pdgCode);
  m_outputTree->Branch("barcode", &m_barcode);
  m_outputTree->Branch("vertex", &m_vertex);
  m_outputTree->Branch("primary", &m_primary);
  m_outputTree->Branch("generation", &m_generation);
  m_outputTree->Branch("secondary", &m_secondary);
  m_outputTree->Branch("process", &m_process);
}

FW::Root::RootParticleWriter::~RootParticleWriter()
{
  m_outputFile->Close();
}

FW::ProcessCode
FW::Root::RootParticleWriter::endRun()
{
  m_outputFile->cd();
  m_outputTree->Write();
  ACTS_INFO("Wrote particles to tree '" << m_cfg.treeName << "' in '"
                                        << m_cfg.filePath << "'");
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::RootParticleWriter::writeT(
    const AlgorithmContext&                      ctx,
    const std::vector<Acts::ProcessVertex>& vertices)
{
  // exclusive access to the tree
  std::lock_guard<std::mutex> lock(m_writeMutex);

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
  
  // loop over the process vertices
  for (auto& vertex: vertices){
    auto& vtx = vertex.position();
    for (auto& particle : vertex.outgoingParticles()) {
      /// collect the information
      m_vx.push_back(vtx.x());
      m_vy.push_back(vtx.y());
      m_vz.push_back(vtx.z());
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
      if (m_cfg.barcodeSvc) {
        // the barcode service
        m_vertex.push_back(m_cfg.barcodeSvc->vertex(particle.barcode()));
        m_primary.push_back(m_cfg.barcodeSvc->primary(particle.barcode()));
        m_generation.push_back(m_cfg.barcodeSvc->generate(particle.barcode()));
        m_secondary.push_back(m_cfg.barcodeSvc->secondary(particle.barcode()));
        m_process.push_back(m_cfg.barcodeSvc->process(particle.barcode()));
      }
    }
  }
  m_outputTree->Fill();

  return ProcessCode::SUCCESS;
}
#include "ACTFW/Plugins/Root/RootPositionWriter.hpp"
#include <TFile.h>
#include <TTree.h>
#include <ios>
#include <stdexcept>

FW::Root::RootPositionWriter::RootPositionWriter(
    const FW::Root::RootPositionWriter::Config& cfg,
    Acts::Logging::Level                        level)
  : FW::WriterT<std::vector<Acts::Vector3D>>(cfg.positions,
                                             "RootPositionWriter",
                                             level)
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
  // Validate the configuration
  if (m_cfg.positions.empty()) {
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
  m_outputTree
      = new TTree(m_cfg.treeName.c_str(), "TTree from RootPositionWriter");
  if (!m_outputTree) throw std::bad_alloc();

  // Initial parameters
  m_outputTree->Branch("x", &m_x);
  m_outputTree->Branch("y", &m_y);
  m_outputTree->Branch("z", &m_z);
  m_outputTree->Branch("r", &m_r);
}

FW::Root::RootPositionWriter::~RootPositionWriter()
{
  m_outputFile->Close();
}

FW::ProcessCode
FW::Root::RootPositionWriter::writeT(
    const FW::AlgorithmContext&        ctx,
    const std::vector<Acts::Vector3D>& positions)
{
  // exclusive access to the tree
  std::lock_guard<std::mutex> lock(m_writeMutex);

  // loop over all layers
  for (auto& pos : positions) {
    m_x = pos.x();
    m_y = pos.y();
    m_z = pos.z();
    m_r = pos.perp();

    m_outputTree->Fill();
  }

  // return success
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::RootPositionWriter::endRun()
{
  m_outputFile->cd();
  m_outputTree->Write();
  ACTS_INFO("Wrote distances per layer to tree '" << m_cfg.treeName << "' in '"
                                                  << m_cfg.filePath
                                                  << "'");
  return ProcessCode::SUCCESS;
}

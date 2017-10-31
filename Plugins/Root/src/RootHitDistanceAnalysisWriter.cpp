#include "ACTFW/Plugins/Root/RootHitDistanceAnalysisWriter.hpp"
#include <TFile.h>
#include <TTree.h>
#include <ios>
#include <stdexcept>

FW::Root::RootHitDistanceAnalysisWriter::RootHitDistanceAnalysisWriter(
    const FW::Root::RootHitDistanceAnalysisWriter::Config& cfg,
    Acts::Logging::Level                                   level)
  : FW::WriterT<std::map<Acts::GeometryID,
                         std::pair<FW::AnalysisParameters,
                                   FW::AnalysisParameters>>>(
        cfg.layerHitAnalysis,
        "RootHitDistanceAnalysisWriter",
        level)
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
  // Validate the configuration
  if (m_cfg.layerHitAnalysis.empty()) {
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
  m_outputTree = new TTree(m_cfg.treeName.c_str(),
                           "TTree from RootHitDistanceAnalysisWriter");
  if (!m_outputTree) throw std::bad_alloc();

  // Initial parameters
  m_outputTree->Branch("layerID", &m_layerID);

  m_outputTree->Branch("mean0", &m_mean0);
  m_outputTree->Branch("min0", &m_min0);
  m_outputTree->Branch("max0", &m_max0);

  m_outputTree->Branch("mean1", &m_mean1);
  m_outputTree->Branch("min1", &m_min1);
  m_outputTree->Branch("max1", &m_max1);
}

FW::Root::RootHitDistanceAnalysisWriter::~RootHitDistanceAnalysisWriter()
{
  m_outputFile->Close();
}

FW::ProcessCode
FW::Root::RootHitDistanceAnalysisWriter::writeT(
    const FW::AlgorithmContext& ctx,
    const std::map<Acts::GeometryID,
                   std::pair<FW::AnalysisParameters, FW::AnalysisParameters>>&
        layerDistanceParams)
{
  // exclusive access to the tree
  std::lock_guard<std::mutex> lock(m_writeMutex);

  // loop over all layers
  for (auto& layer : layerDistanceParams) {
    m_layerID                              = layer.first.value();
    FW::AnalysisParameters distanceParams0 = layer.second.first;
    FW::AnalysisParameters distanceParams1 = layer.second.second;
    m_mean0                                = distanceParams0.mean();
    m_min0                                 = distanceParams0.min();
    m_max0                                 = distanceParams0.max();
    m_mean1                                = distanceParams1.mean();
    m_min1                                 = distanceParams1.min();
    m_max1                                 = distanceParams1.max();

    m_outputTree->Fill();
  }

  // return success
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::RootHitDistanceAnalysisWriter::endRun()
{
  m_outputFile->cd();
  m_outputTree->Write();
  ACTS_INFO("Wrote distances per layer to tree '" << m_cfg.treeName << "' in '"
                                                  << m_cfg.filePath
                                                  << "'");
  return ProcessCode::SUCCESS;
}
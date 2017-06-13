#include "ACTFW/Plugins/Root/RootMaterialTrackReader.hpp"
#include <iostream>
#include "TChain.h"
#include "TFile.h"

FWRoot::RootMaterialTrackReader::RootMaterialTrackReader(
    const FWRoot::RootMaterialTrackReader::Config& cfg)
  : FW::IReaderT<Acts::MaterialTrack>()
  , m_cfg(cfg)
  , m_inputChain(nullptr)
  , m_trackRecord(nullptr)
  , m_event(0)
{
}

FW::ProcessCode
FWRoot::RootMaterialTrackReader::initialize()
{
  m_trackRecord = new Acts::MaterialTrack;

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::RootMaterialTrackReader::finalize()
{
  delete m_trackRecord;
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::RootMaterialTrackReader::read(Acts::MaterialTrack&        mtrc,
                                      size_t                      skip,
                                      const FW::AlgorithmContext* context)
{
  // load the input chain
  if (!m_inputChain) {
    // create the input Chain
    m_inputChain = new TChain(m_cfg.treeName.c_str());
    // set the branch
    int branchAddress = m_inputChain->SetBranchAddress<Acts::MaterialTrack>(
        "MaterialTrack", &m_trackRecord);
    ACTS_VERBOSE("Setting Branch address " << branchAddress);
    m_inputChain->SetBranchStatus("MaterialTrack", 1);
    // loop over the input files
    for (auto inputFile : m_cfg.fileList) {
      // add file to the input chain
      m_inputChain->Add(inputFile.c_str());
      ACTS_DEBUG("Adding File " << inputFile << " to tree '" << m_cfg.treeName
                                << "'.");
    }
    // Full event count
    ACTS_DEBUG("The full chain has " << m_inputChain->GetEntries()
                                     << " entries.");
  }

  // some screen output
  if (!(m_event % 1000))
    ACTS_DEBUG("Mapped " << m_event << " out of " << m_inputChain->GetEntries()
                         << " entries.");

  // read the entry and increase
  if (!m_inputChain->GetEntry(skip + m_event++)) {
    ACTS_VERBOSE("No bytes read from the File.");
    return FW::ProcessCode::ABORT;
  }

  // some screen printout
  ACTS_VERBOSE("Material Track Record read in with phi / theta = "
               << m_trackRecord->phi()
               << " / "
               << m_trackRecord->theta());

  // now assign
  mtrc = (*m_trackRecord);
  // return scuess
  return FW::ProcessCode::SUCCESS;
}

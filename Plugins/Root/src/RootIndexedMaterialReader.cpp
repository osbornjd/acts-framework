#include "ACTFW/Plugins/Root/RootIndexedMaterialReader.hpp"
#include <iostream>
#include "ACTS/Utilities/GeometryID.hpp"
#include "TFile.h"
#include "TH2F.h"

FWRoot::RootIndexedMaterialReader::RootIndexedMaterialReader(
    const FWRoot::RootIndexedMaterialReader::Config& cfg)
  : FW::IReaderT<Acts::IndexedSurfaceMaterial>()
  , m_cfg(cfg)
  , m_inputFile(nullptr)
{
}

FW::ProcessCode
FWRoot::RootIndexedMaterialReader::initialize()
{
  // open the file name
  m_inputFile = TFile::Open(m_cfg.fileName.c_str());
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::RootIndexedMaterialReader::finalize()
{
  m_inputFile->Close();
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::RootIndexedMaterialReader::read(Acts::IndexedSurfaceMaterial& ism,
                                        size_t                        skip,
                                        const FW::AlgorithmContext*   context)
{
  // only if you have file
  if (m_inputFile) {
    // get the geometry ID
    Acts::GeometryID geoID = ism.first;
    // decode the geometryID
    geo_id_value gvolID = geoID.value(Acts::GeometryID::volume_mask);
    geo_id_value glayID = geoID.value(Acts::GeometryID::layer_mask);
    geo_id_value gappID = geoID.value(Acts::GeometryID::approach_mask);
    geo_id_value gsenID = geoID.value(Acts::GeometryID::sensitive_mask);
    // create the directory
    std::string tdName = m_cfg.folderNameBase.c_str();
    tdName += "_vol" + std::to_string(gvolID);
    tdName += "_lay" + std::to_string(glayID);
    tdName += "_app" + std::to_string(gappID);
    tdName += "_sen" + std::to_string(gsenID);
    // construct the names
    std::string tName   = tdName + "/t";
    std::string x0Name  = tdName + "/X0";
    std::string l0Name  = tdName + "/L0";
    std::string aName   = tdName + "/A";
    std::string zName   = tdName + "/Z";
    std::string rhoName = tdName + "/rho";
    // get the histograms
    TH2F* t   = dynamic_cast<TH2F*>(m_inputFile->Get(tName.c_str()));
    TH2F* x0  = dynamic_cast<TH2F*>(m_inputFile->Get(x0Name.c_str()));
    TH2F* l0  = dynamic_cast<TH2F*>(m_inputFile->Get(l0Name.c_str()));
    TH2F* A   = dynamic_cast<TH2F*>(m_inputFile->Get(aName.c_str()));
    TH2F* Z   = dynamic_cast<TH2F*>(m_inputFile->Get(zName.c_str()));
    TH2F* rho = dynamic_cast<TH2F*>(m_inputFile->Get(rhoName.c_str()));
    // only go on when you have all histograms
    if (t and x0 and l0 and A and Z and rho) {
      // Get the number of bins
      int nbins0 = t->GetNbinsX();
      int nbins1 = t->GetNbinsY();
      //

      for (int ib0 = 0; ib0 < nbins0; ++ib0) {
        for (int ib1 = 0; ib1 < nbins1; ++ib1) {
          // get the values
          double dt   = t->GetBinContent(ib0 + 1, ib1 + 1);
          double dx0  = x0->GetBinContent(ib0 + 1, ib1 + 1);
          double dl0  = l0->GetBinContent(ib0 + 1, ib1 + 1);
          double da   = A->GetBinContent(ib0 + 1, ib1 + 1);
          double dz   = Z->GetBinContent(ib0 + 1, ib1 + 1);
          double drho = rho->GetBinContent(ib0 + 1, ib1 + 1);
          //
        }
      }
    }
  }
  // return scuess
  return FW::ProcessCode::SUCCESS;
}

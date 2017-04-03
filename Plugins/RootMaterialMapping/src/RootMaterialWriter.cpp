#include "ACTFW/RootMaterialMapping/RootMaterialWriter.hpp"
#include <iostream>
#include "ACTS/Utilities/BinUtility.hpp"
#include "ACTS/Utilities/BinningData.hpp"
#include "TFile.h"
#include "TProfile.h"

FWRoot::RootMaterialWriter::RootMaterialWriter(
    const FWRoot::RootMaterialWriter::Config& cfg)
  : FW::IMaterialWriter(), m_cfg(cfg), m_outputFile(nullptr)
{
}

FW::ProcessCode
FWRoot::RootMaterialWriter::initialize()
{
  ACTS_INFO("Registering new ROOT output File to Material Writer : "
            << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), "recreate");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::RootMaterialWriter::finalize()
{
  // write the tree and close the file
  ACTS_INFO("Closing and Writing ROOT output File of Material Writer : "
            << m_cfg.fileName);
  m_outputFile->Close();
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::RootMaterialWriter::write(
    std::shared_ptr<const Acts::BinnedSurfaceMaterial> surfMaterial,
    const Acts::GeometryID                             geoID,
    std::string                                        name)
{
  ///
  std::string surfID   = name;  //+ "_" + std::to_string(geoID.value());
  TTree*      surfTree = new TTree(surfID.c_str(), surfID.c_str());
  // etxract the binUtility
  const Acts::BinUtility& binUtility = surfMaterial->binUtility();
  // access the material matrix
  const Acts::MaterialPropertiesMatrix materialMat
      = surfMaterial->fullMaterial();
  // get the bin size
  int bins0 = binUtility.bins(0);
  int bins1 = binUtility.bins(1);
  // position vectors
  std::vector<float> loc0;
  std::vector<float> loc1;
  // material proerties
  std::vector<float> A;
  std::vector<float> Z;
  std::vector<float> x0;
  std::vector<float> l0;
  std::vector<float> thickness;
  std::vector<float> rho;
  std::vector<float> tInL0;
  std::vector<float> tInX0;
  // prepare
  loc0.reserve(bins0 * bins1);
  loc1.reserve(bins0 * bins1);
  A.reserve(bins0 * bins1);
  Z.reserve(bins0 * bins1);
  x0.reserve(bins0 * bins1);
  l0.reserve(bins0 * bins1);
  thickness.reserve(bins0 * bins1);
  rho.reserve(bins0 * bins1);
  tInX0.reserve(bins0 * bins1);
  tInL0.reserve(bins0 * bins1);

  // create the branches
  surfTree->Branch("loc0", &loc0);
  surfTree->Branch("loc1", &loc1);
  surfTree->Branch("A", &A);
  surfTree->Branch("Z", &Z);
  surfTree->Branch("x0", &x0);
  surfTree->Branch("l0", &l0);
  surfTree->Branch("thickness", &thickness);
  surfTree->Branch("rho", &rho);
  surfTree->Branch("tInX0", &tInX0);
  surfTree->Branch("tInL0", &tInL0);

  const std::vector<Acts::BinningData> binningData = binUtility.binningData();
  // loop through the material matrix
  for (size_t i = 0; i < bins0; i++) {
    for (size_t j = 0; j < bins1; j++) {
      const Acts::MaterialProperties* materialProperties = materialMat.at(j).at(i);
      loc0.push_back(binningData.at(0).centerValue(i));
      loc1.push_back(binningData.at(1).centerValue(j));
      A.push_back(materialProperties->averageA());
      Z.push_back(materialProperties->averageZ());
      x0.push_back(materialProperties->material().X0());
      l0.push_back(materialProperties->material().L0());
      thickness.push_back(materialProperties->thickness());
      rho.push_back(materialProperties->averageRho());
      tInX0.push_back(materialProperties->thicknessInX0());
      tInL0.push_back(materialProperties->thicknessInL0());
    }
  }
  ACTS_VERBOSE("Writing out material of object: " << surfID);
  surfTree->Fill();
  surfTree->Write();
  delete surfTree;

  return FW::ProcessCode::SUCCESS;
}

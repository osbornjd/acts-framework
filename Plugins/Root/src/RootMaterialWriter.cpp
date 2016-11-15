#include "ACTFW/Root/RootMaterialWriter.hpp"
#include <iostream>
#include "ACTS/Utilities/BinUtility.hpp"
#include "ACTS/Utilities/BinningData.hpp"
#include "TFile.h"
#include "TProfile.h"

FWRoot::RootMaterialWriter::RootMaterialWriter(
    const FWRoot::RootMaterialWriter::Config& cfg)
    : FW::IMaterialWriter(), m_cfg(cfg), m_outputFile(nullptr) {}

FW::ProcessCode FWRoot::RootMaterialWriter::initialize() {
  ACTS_INFO("Registering new ROOT output File to Material Writer : "
            << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), "recreate");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode FWRoot::RootMaterialWriter::finalize() {
  // write the tree and close the file
  ACTS_INFO("Closing and Writing ROOT output File of Material Writer : "
            << m_cfg.fileName);
  m_outputFile->Close();
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode FWRoot::RootMaterialWriter::write(
    std::shared_ptr<const Acts::BinnedSurfaceMaterial> surfMaterial,
    const Acts::GeometryID geoID, std::string name) {
  ///
  std::string surfID = name + "_" + std::to_string(geoID.value());
  TTree* surfTree = new TTree(surfID.c_str(), surfID.c_str());
  // etxract the binUtility
  const Acts::BinUtility* binUtility = surfMaterial->binUtility();
  // access the material matrix
  const Acts::MaterialPropertiesMatrix materialMat =
      surfMaterial->fullMaterial();
  // get the bin size
  int bins1 = binUtility->bins(0);
  int bins2 = binUtility->bins(1);
  // position vectors
  std::vector<float> phi;
  std::vector<float> z;
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
  phi.reserve(bins1 * bins2);
  z.reserve(bins1 * bins2);
  A.reserve(bins1 * bins2);
  Z.reserve(bins1 * bins2);
  x0.reserve(bins1 * bins2);
  l0.reserve(bins1 * bins2);
  thickness.reserve(bins1 * bins2);
  rho.reserve(bins1 * bins2);
  tInX0.reserve(bins1 * bins2);
  tInL0.reserve(bins1 * bins2);

  // create the branches
  surfTree->Branch("phi", &phi);
  surfTree->Branch("z", &z);
  surfTree->Branch("A", &A);
  surfTree->Branch("Z", &Z);
  surfTree->Branch("x0", &x0);
  surfTree->Branch("l0", &l0);
  surfTree->Branch("thickness", &thickness);
  surfTree->Branch("rho", &rho);
  surfTree->Branch("tInX0", &tInX0);
  surfTree->Branch("tInL0", &tInL0);

  const std::vector<Acts::BinningData> binningData = binUtility->binningData();
  // loop through the material matrix
  for (int i = 0; i < bins1; i++) {
    for (int j = 0; j < bins2; j++) {
      const Acts::MaterialProperties* material = materialMat.at(j).at(i);
      phi.push_back(binningData.at(0).centerValue(i));
      z.push_back(binningData.at(1).centerValue(j));
      A.push_back(material->averageA());
      Z.push_back(material->averageZ());
      x0.push_back(material->x0());
      l0.push_back(material->l0());
      thickness.push_back(material->thickness());
      rho.push_back(material->averageRho());
      tInX0.push_back(material->thicknessInX0());
      tInL0.push_back(material->thicknessInL0());
      /*
        ACTS_INFO("Fill: [" << i << ", " << j << "] at pos: ("
                            << binningData.at(0).centerValue(i) << ","
                            << binningData.at(1).centerValue(j) << ")"
                            << "tInX0: " << material->thicknessInX0());
      */
    }
  }
  ACTS_VERBOSE("Writing out material of object: " << surfID);
  surfTree->Fill();
  surfTree->Write();
  delete surfTree;

  return FW::ProcessCode::SUCCESS;
}

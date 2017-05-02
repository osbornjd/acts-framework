#include "ACTFW/RootMaterialMapping/RootMaterialWriter.hpp"
#include <iostream>
#include "ACTS/Utilities/BinUtility.hpp"
#include "ACTS/Utilities/BinningData.hpp"
#include "ACTS/Material/BinnedSurfaceMaterial.hpp"
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
    const Acts::SurfaceMaterial& surfaceMaterial,
    const Acts::GeometryID& geoID,
    const std::string& name)
{
  // for the moment - cast to BinnedSurfaceMaterial
  auto binnedSurfaceMaterial
  = dynamic_cast<const Acts::BinnedSurfaceMaterial*>(&surfaceMaterial);
  // only if the cast works
  if (binnedSurfaceMaterial){
    // volume 
    auto vID = std::to_string(geoID.value(Acts::GeometryID::volume_mask));
    auto bID = std::to_string(geoID.value(Acts::GeometryID::boundary_mask));
    auto lID = std::to_string(geoID.value(Acts::GeometryID::layer_mask));
    auto aID = std::to_string(geoID.value(Acts::GeometryID::approach_mask));
    auto sID = std::to_string(geoID.value(Acts::GeometryID::sensitive_mask));
    // create the unique name
    std::string surfID   = "v"+vID+"_b"+bID+"_l"+lID+"_a"+aID+"_s"+sID;
    // change to the output file
    m_outputFile->cd();
    TTree*      surfTree = new TTree(surfID.c_str(), surfID.c_str());
    // etxract the binUtility
    const Acts::BinUtility& binUtility = binnedSurfaceMaterial->binUtility();
    // access the material matrix
    const Acts::MaterialPropertiesMatrix materialMat
        = binnedSurfaceMaterial->fullMaterial();
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
        // get the material
        const Acts::MaterialProperties* materialProperties = materialMat.at(j).at(i);
        loc0.push_back(binningData.at(0).centerValue(i));
        loc1.push_back(binningData.at(1).centerValue(j));
        // prepare
        double bA, bZ, bx0, bl0, bthickness, brho, btInX0, btInL0 = 0.;
        //  
        if (materialProperties){
          bA         = materialProperties->averageA();
          bZ         = materialProperties->averageZ();
          bx0        = materialProperties->material().X0();
          bl0        = materialProperties->material().L0();
          bthickness = materialProperties->thickness();
          brho       = materialProperties->averageRho();
          btInX0     = materialProperties->thicknessInX0();
          btInL0     = materialProperties->thicknessInL0();
        }
        // and fill it
        A.push_back(bA);
        Z.push_back(bZ);
        x0.push_back(bx0);
        l0.push_back(bl0);
        thickness.push_back(bthickness);
        rho.push_back(brho);
        tInX0.push_back(btInX0);
        tInL0.push_back(btInL0);
      }
    }
    ACTS_VERBOSE("Writing out material of object: " << surfID);
    surfTree->Fill();
    surfTree->Write();
    delete surfTree;
  }
  return FW::ProcessCode::SUCCESS;
}

#include "ACTFW/Root/RootMaterialStepWriter.hpp"
#include "ACTS/Material/BinnedSurfaceMaterial.hpp"
#include "ACTS/Utilities/BinUtility.hpp"
#include "ACTS/Utilities/BinningData.hpp"
#include "TFile.h"
#include "TTree.h"

FWRoot::RootMaterialStepWriter::RootMaterialStepWriter(
    const FWRoot::RootMaterialStepWriter::Config& cfg)
  : FW::IMaterialStepWriter(), m_cfg(cfg), m_outputFile(nullptr)
{
}

FW::ProcessCode
FWRoot::RootMaterialStepWriter::initialize()
{
  ACTS_INFO("Registering new ROOT output File : " << m_cfg.fileName);
  // open the output file
  m_outputFile = new TFile(m_cfg.fileName.c_str(), "recreate");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::RootMaterialStepWriter::finalize()
{
  // write the tree and close the file
  ACTS_INFO("Closing and Writing ROOT output File : " << m_cfg.fileName);
  m_outputFile->Close();
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWRoot::RootMaterialStepWriter::write(
    std::string          name,
    const Acts::Surface* surface,
    const std::vector<std::pair<const Acts::MaterialStep, const Acts::Vector3D>>
        stepsAndLayerPos)
{
  // create the tree
  TTree* surfTree = new TTree(name.c_str(), name.c_str());
  // check if Binned surface material is present
  const Acts::BinnedSurfaceMaterial* material
      = dynamic_cast<const Acts::BinnedSurfaceMaterial*>(
          surface->associatedMaterial());
  if (material) {
    // acces the bin utility
    const Acts::BinUtility* binUtility = material->binUtility();
    // get the bin size
    int bins1 = binUtility->bins(0);
    int bins2 = binUtility->bins(1);
    // real global positions of the material
    std::vector<float> globR;
    std::vector<float> globZ;
    // assigned global positions of the material
    std::vector<float> assignedGlobR;
    std::vector<float> assignedGlobZ;
    // local coordinates
    std::vector<float> phi;
    std::vector<float> z;
    // material properties
    std::vector<float> A;
    std::vector<float> Z;
    std::vector<float> x0;
    std::vector<float> l0;
    std::vector<float> thickness;
    std::vector<float> rho;
    std::vector<float> tInL0;
    std::vector<float> tInX0;
    // prepare
    assignedGlobR.reserve(stepsAndLayerPos.size());
    assignedGlobZ.reserve(stepsAndLayerPos.size());
    globR.reserve(stepsAndLayerPos.size());
    globZ.reserve(stepsAndLayerPos.size());

    phi.reserve(stepsAndLayerPos.size());
    z.reserve(stepsAndLayerPos.size());
    A.reserve(stepsAndLayerPos.size());
    Z.reserve(stepsAndLayerPos.size());
    x0.reserve(stepsAndLayerPos.size());
    l0.reserve(stepsAndLayerPos.size());
    thickness.reserve(stepsAndLayerPos.size());
    rho.reserve(stepsAndLayerPos.size());
    tInX0.reserve(stepsAndLayerPos.size());
    tInL0.reserve(stepsAndLayerPos.size());

    // create the branches
    surfTree->Branch("assignedGlobR", &assignedGlobR);
    surfTree->Branch("assignedGlobZ", &assignedGlobZ);
    surfTree->Branch("globR", &globR);
    surfTree->Branch("globZ", &globZ);

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

    // now loop through the materialsteps
    for (auto step : stepsAndLayerPos) {
      // access material properties
      const Acts::MaterialProperties mprop = step.first.material();
      // access position and convert to position on surface
      const Acts::Vector3D position(step.first.position().x,
                                    step.first.position().y,
                                    step.first.position().z);
      // convert to local position
      // @TODO probably change later for not cylindrical surfaces
      const Acts::Transform3D& surfaceTrans = surface->transform();
      Acts::Transform3D        inverseTrans(surfaceTrans.inverse());
      Acts::Vector3D           loc3D(inverseTrans * position);
      Acts::Vector2D           lposition(loc3D.phi(), loc3D.z());

      // fill the histogram
      globR.push_back(
          sqrt(position.x() * position.x() + position.y() * position.y()));
      globZ.push_back(position.z());
      assignedGlobR.push_back(sqrt(step.second.x() * step.second.x()
                                   + step.second.y() * step.second.y()));
      assignedGlobZ.push_back(step.second.z());
      phi.push_back(lposition.x());
      z.push_back(lposition.y());
      A.push_back(mprop.averageA());
      Z.push_back(mprop.averageZ());
      x0.push_back(mprop.x0());
      l0.push_back(mprop.l0());
      thickness.push_back(mprop.thickness());
      rho.push_back(mprop.averageRho());
      tInX0.push_back(mprop.thicknessInX0());
      tInL0.push_back(mprop.thicknessInL0());
    }

    surfTree->Fill();
    // write the histogram onto the file
    surfTree->Write();
    delete surfTree;
    // return success
    return FW::ProcessCode::SUCCESS;
  } else
    ACTS_ERROR(
        "Surface has not material assigned - Can not write out its material!");
}

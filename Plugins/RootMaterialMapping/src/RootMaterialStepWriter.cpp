#include "ACTFW/RootMaterialMapping/RootMaterialStepWriter.hpp"
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
    std::string                           name,
    const Acts::Surface*                  surface,
    const std::vector<Acts::MaterialStep> steps,
    const std::vector<std::pair<const Acts::Vector3D, const Acts::Vector3D>>
        realAndAssignedPos)
{
  // create the tree
  TTree* surfTree = new TTree(name.c_str(), name.c_str());
  // check if Binned surface material is present
  const Acts::BinnedSurfaceMaterial* material
      = dynamic_cast<const Acts::BinnedSurfaceMaterial*>(
          surface->associatedMaterial());
  if (material) {
    // acces the bin utility
    const Acts::BinUtility& binUtility = material->binUtility();
    // real global positions of the material
    std::vector<float> globX;
    std::vector<float> globY;
    std::vector<float> globZ;
    std::vector<float> globR;
    // assigned global positions of the material
    std::vector<float> assignedGlobX;
    std::vector<float> assignedGlobY;
    std::vector<float> assignedGlobZ;
    std::vector<float> assignedGlobR;
    // local coordinates
    std::vector<float> loc0;
    std::vector<float> loc1;
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
    globX.reserve(realAndAssignedPos.size());
    globY.reserve(realAndAssignedPos.size());
    globZ.reserve(realAndAssignedPos.size());
    globR.reserve(realAndAssignedPos.size());

    assignedGlobX.reserve(realAndAssignedPos.size());
    assignedGlobY.reserve(realAndAssignedPos.size());
    assignedGlobZ.reserve(realAndAssignedPos.size());
    assignedGlobR.reserve(realAndAssignedPos.size());

    loc0.reserve(steps.size());
    loc1.reserve(steps.size());
    A.reserve(steps.size());
    Z.reserve(steps.size());
    x0.reserve(steps.size());
    l0.reserve(steps.size());
    thickness.reserve(steps.size());
    rho.reserve(steps.size());
    tInX0.reserve(steps.size());
    tInL0.reserve(steps.size());

    // create the branches

    surfTree->Branch("globX", &globX);
    surfTree->Branch("globY", &globY);
    surfTree->Branch("globZ", &globZ);
    surfTree->Branch("globR", &globR);

    surfTree->Branch("assignedGlobX", &assignedGlobX);
    surfTree->Branch("assignedGlobY", &assignedGlobY);
    surfTree->Branch("assignedGlobZ", &assignedGlobZ);
    surfTree->Branch("assignedGlobR", &assignedGlobR);

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

    // now loop through the materialsteps
    for (auto step : steps) {
      // access material properties
      const Acts::MaterialProperties mprop = step.materialProperties();
      // access position and convert to position on surface
      const Acts::Vector3D position(
          step.position().x, step.position().y, step.position().z);

      // convert to local position
      Acts::Vector2D lposition(0., 0.);

      // distinguish cylinder surface (local coordinates are (rphi,z) and we
      // wanr phi,z)
      if (surface->type() == Acts::Surface::SurfaceType::Cylinder) {
        Acts::Transform3D inverseTrans(surface->transform().inverse());
        Acts::Vector3D    loc3D(inverseTrans * position);
        lposition = Acts::Vector2D(loc3D.phi(), loc3D.z());
      } else
        surface->globalToLocal(position, Acts::Vector3D(0., 0., 0.), lposition);

      loc0.push_back(lposition.x());
      loc1.push_back(lposition.y());
      A.push_back(mprop.averageA());
      Z.push_back(mprop.averageZ());
      x0.push_back(mprop.material().X0());
      l0.push_back(mprop.material().L0());
      thickness.push_back(mprop.thickness());
      rho.push_back(mprop.averageRho());
      tInX0.push_back(mprop.thicknessInX0());
      tInL0.push_back(mprop.thicknessInL0());
    }

    for (auto pos : realAndAssignedPos) {
      // fill the histogram
      globX.push_back(pos.first.x());
      globY.push_back(pos.first.y());
      globZ.push_back(pos.first.z());
      globR.push_back(
          sqrt(pos.first.x() * pos.first.x() + pos.first.y() * pos.first.y()));

      assignedGlobX.push_back(pos.second.x());
      assignedGlobY.push_back(pos.second.y());
      assignedGlobZ.push_back(pos.second.z());
      assignedGlobR.push_back(sqrt(pos.second.x() * pos.second.x()
                                   + pos.second.y() * pos.second.y()));
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

   return FW::ProcessCode::ABORT; 
}

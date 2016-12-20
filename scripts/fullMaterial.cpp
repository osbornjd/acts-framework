/*
 * fullMaterial.cxx
 *
 *  Created on: 22 Aug 2016
 *      Author: jhrdinka
 */

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TTree.h"

void
fullMaterial(std::string inFile,
             std::string treename,
             std::string outFile,
             int         binsZ,
             float       minZ,
             float       maxZ,
             int         binsPhi,
             float       minPhi,
             float       maxPhi)
{
  std::cout << "Opening file: " << inFile << std::endl;
  TFile inputFile(inFile.c_str());
  std::cout << "Reading tree: " << treename << std::endl;
  TTreeReader reader(treename.c_str(), &inputFile);

  // get the MaterialTrackRecord entities
  std::vector<Acts::MaterialTrackRecord> mrecords;
  std::cout << "Accessing Branch 'MaterialTrackRecords'" << std::endl;
  TTreeReaderValue<Acts::MaterialTrackRecord> mtRecord(reader,
                                                       "MaterialTrackRecords");
  while (reader.Next()) {
    mrecords.push_back(*mtRecord);
  }
  inputFile.Close();

  std::cout << "Creating new output file: " << outFile << " and writing "
                                                          "material maps"
            << std::endl;
  TFile outputFile(outFile.c_str(), "recreate");
  // thickness
  TH1F* t_z   = new TH1F("t_z", "t_z", binsZ, minZ, maxZ);
  TH1F* t_phi = new TH1F("t_phi", "t_phi", binsPhi, minPhi, maxPhi);
  TH2F* t     = new TH2F(
      "thickness", "thickness", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);

  // thickness in X0
  TH1F* dInX0_z   = new TH1F("dInX0_z", "dInX0_z", binsZ, minZ, maxZ);
  TH1F* dInX0_phi = new TH1F("dInX0_phi", "dInX0_phi", binsPhi, minPhi, maxPhi);
  TH2F* dInX0
      = new TH2F("dInX0", "dInX0", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // thickness in L0
  TH1F* dInL0_z   = new TH1F("dInL0_z", "dInL0_z", binsZ, minZ, maxZ);
  TH1F* dInL0_phi = new TH1F("dInL0_phi", "dInL0_phi", binsPhi, minPhi, maxPhi);
  TH2F* dInL0
      = new TH2F("dInL0", "dInL0", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // A
  TH1F* A_z   = new TH1F("A_z", "A_z", binsZ, minZ, maxZ);
  TH1F* A_phi = new TH1F("A_phi", "A_phi", binsPhi, minPhi, maxPhi);
  TH2F* A     = new TH2F("A", "A", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // Z
  TH1F* Z_z   = new TH1F("Z_z", "Z_z", binsZ, minZ, maxZ);
  TH1F* Z_phi = new TH1F("Z_phi", "Z_phi", binsPhi, minPhi, maxPhi);
  TH2F* Z     = new TH2F("Z", "Z", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // Rho
  TH1F* rho_z   = new TH1F("rho_z", "rho_z", binsZ, minZ, maxZ);
  TH1F* rho_phi = new TH1F("rho_phi", "rho_phi", binsPhi, minPhi, maxPhi);
  TH2F* rho
      = new TH2F("rho", "rho", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // x0
  TH1F* x0_z   = new TH1F("x0_z", "x0_z", binsZ, minZ, maxZ);
  TH1F* x0_phi = new TH1F("x0_phi", "x0_phi", binsPhi, minPhi, maxPhi);
  TH2F* x0_map
      = new TH2F("x0", "x0", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // l0
  TH1F* l0_z   = new TH1F("l0_z", "l0_z", binsZ, minZ, maxZ);
  TH1F* l0_phi = new TH1F("l0_phi", "l0_phi", binsPhi, minPhi, maxPhi);
  TH2F* l0_map
      = new TH2F("l0", "l0", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);

  for (auto& mrecord : mrecords) {
    std::vector<Acts::MaterialStep> steps = mrecord.materialSteps();
    for (auto& step : steps) {
      // thickness
      t_z->Fill(step.position().z, step.material().thickness());
      t_phi->Fill(atan2(step.position().y, step.position().x),
                  step.material().thickness());
      t->Fill(step.position().z,
              atan2(step.position().y, step.position().x),
              step.material().thickness());

      // thickness in X0
      dInX0_z->Fill(step.position().z, step.material().thicknessInX0());
      dInX0_phi->Fill(atan2(step.position().y, step.position().x),
                      step.material().thicknessInX0());
      dInX0->Fill(step.position().z,
                  atan2(step.position().y, step.position().x),
                  step.material().thicknessInX0());
      // thickness in L0
      dInL0_z->Fill(step.position().z, step.material().thicknessInL0());
      dInL0_phi->Fill(atan2(step.position().y, step.position().x),
                      step.material().thicknessInL0());
      dInL0->Fill(step.position().z,
                  atan2(step.position().y, step.position().x),
                  step.material().thicknessInL0());
      // A
      A_z->Fill(step.position().z, step.material().averageA());
      A_phi->Fill(atan2(step.position().y, step.position().x),
                  step.material().averageA());
      A->Fill(step.position().z,
              atan2(step.position().y, step.position().x),
              step.material().averageA());
      // Z
      Z_z->Fill(step.position().z, step.material().averageZ());
      Z_phi->Fill(atan2(step.position().y, step.position().x),
                  step.material().averageZ());
      Z->Fill(step.position().z,
              atan2(step.position().y, step.position().x),
              step.material().averageZ());
      // rho
      rho_z->Fill(step.position().z, step.material().averageRho());
      rho_phi->Fill(atan2(step.position().y, step.position().x),
                    step.material().averageRho());
      rho->Fill(step.position().z,
                atan2(step.position().y, step.position().x),
                step.material().averageRho());
      // x0
      x0_z->Fill(step.position().z, step.material().x0());
      x0_phi->Fill(atan2(step.position().y, step.position().x),
                   step.material().x0());
      x0_map->Fill(step.position().z,
                   atan2(step.position().y, step.position().x),
                   step.material().x0());
      // l0
      l0_z->Fill(step.position().z, step.material().l0());
      l0_phi->Fill(atan2(step.position().y, step.position().x),
                   step.material().l0());
      l0_map->Fill(step.position().z,
                   atan2(step.position().y, step.position().x),
                   step.material().l0());
    }
  }
  // thickness
  t_z->Write();
  t_phi->Write();
  t->Write();
  delete t_z;
  delete t_phi;
  delete t;
  // thickness in X0
  dInX0_z->Write();
  dInX0_phi->Write();
  dInX0->Write();
  delete dInX0_z;
  delete dInX0_phi;
  delete dInX0;
  // thickness in L0
  dInL0_z->Write();
  dInL0_phi->Write();
  dInL0->Write();
  delete dInL0_z;
  delete dInL0_phi;
  delete dInL0;
  // A
  A_z->Write();
  A_phi->Write();
  A->Write();
  delete A_z;
  delete A_phi;
  delete A;
  // Z
  Z_z->Write();
  Z_phi->Write();
  Z->Write();
  delete Z_z;
  delete Z_phi;
  delete Z;
  // rho
  rho_z->Write();
  rho_phi->Write();
  rho->Write();
  delete rho_z;
  delete rho_phi;
  delete rho;
  // x0
  x0_z->Write();
  x0_phi->Write();
  x0_map->Write();
  delete x0_z;
  delete x0_phi;
  delete x0_map;
  // l0
  l0_z->Write();
  l0_phi->Write();
  l0_map->Write();
  delete l0_z;
  delete l0_phi;
  delete l0_map;

  outputFile.Close();
}

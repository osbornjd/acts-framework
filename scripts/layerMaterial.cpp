//
//  layerMaterial.cpp
//
//
//  Created by Julia Hrdinka on 18/08/16.
//
//

#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TObject.h"
#include "TROOT.h"
#include "TTree.h"

// This root script generates

void
layerMaterial(std::string inFile,
              std::string layerName,
              std::string outFile,
              int         binsZ,
              float       minZ,
              float       maxZ,
              int         binsPhi,
              float       minPhi,
              float       maxPhi,
              int         binsR,
              float       minR,
              float       maxR)
{
  std::cout << "Opening file: " << inFile << std::endl;
  TFile  inputFile(inFile.c_str());
  TTree* layer = (TTree*)inputFile.Get(layerName.c_str());
  std::cout << "Reading tree: " << layerName << std::endl;

  std::vector<float>* z             = new std::vector<float>;
  std::vector<float>* phi           = new std::vector<float>;
  std::vector<float>* A             = new std::vector<float>;
  std::vector<float>* Z             = new std::vector<float>;
  std::vector<float>* x0            = new std::vector<float>;
  std::vector<float>* l0            = new std::vector<float>;
  std::vector<float>* d             = new std::vector<float>;
  std::vector<float>* rho           = new std::vector<float>;
  std::vector<float>* dInX0         = new std::vector<float>;
  std::vector<float>* dInL0         = new std::vector<float>;
  std::vector<float>* globR         = new std::vector<float>;
  std::vector<float>* globZ         = new std::vector<float>;
  std::vector<float>* assignedGlobR = new std::vector<float>;
  std::vector<float>* assignedGlobZ = new std::vector<float>;

  layer->SetBranchAddress("phi", &phi);
  layer->SetBranchAddress("z", &z);
  layer->SetBranchAddress("A", &A);
  layer->SetBranchAddress("Z", &Z);
  layer->SetBranchAddress("x0", &x0);
  layer->SetBranchAddress("l0", &l0);
  layer->SetBranchAddress("thickness", &d);
  layer->SetBranchAddress("rho", &rho);
  layer->SetBranchAddress("tInX0", &dInX0);
  layer->SetBranchAddress("tInL0", &dInL0);
  if (layer->FindBranch("globR")) {
    layer->SetBranchAddress("globR", &globR);
  }
  if (layer->FindBranch("globZ")) {
    layer->SetBranchAddress("globZ", &globZ);
  }
  if (layer->FindBranch("assignedGlobR"))
    layer->SetBranchAddress("assignedGlobR", &assignedGlobR);
  if (layer->FindBranch("assignedGlobZ"))
    layer->SetBranchAddress("assignedGlobZ", &assignedGlobZ);
  layer->GetEntry(0);

  inputFile.Close();
  std::cout << "Creating new output file: " << outFile << " and writing "
                                                          "material maps"
            << std::endl;
  TFile       outputFile(outFile.c_str(), "update");
  TDirectory* dir = outputFile.mkdir(layerName.c_str());
  dir->cd();
  // thickness in X0
  TH1F* dInX0_z   = new TH1F("dInX0_z", "dInX0_z", binsZ, minZ, maxZ);
  TH1F* dInX0_phi = new TH1F("dInX0_phi", "dInX0_phi", binsPhi, minPhi, maxPhi);
  TH2F* dInX0_map
      = new TH2F("dInX0", "dInX0", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // thickness in L0
  TH1F* dInL0_z   = new TH1F("dInL0_z", "dInL0_z", binsZ, minZ, maxZ);
  TH1F* dInL0_phi = new TH1F("dInL0_phi", "dInL0_phi", binsPhi, minPhi, maxPhi);
  TH2F* dInL0_map
      = new TH2F("dInL0", "dInL0", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // A
  TH1F* A_z   = new TH1F("A_z", "A_z", binsZ, minZ, maxZ);
  TH1F* A_phi = new TH1F("A_phi", "A_phi", binsPhi, minPhi, maxPhi);
  TH2F* A_map = new TH2F("A", "A", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // Z
  TH1F* Z_z   = new TH1F("Z_z", "Z_z", binsZ, minZ, maxZ);
  TH1F* Z_phi = new TH1F("Z_phi", "Z_phi", binsPhi, minPhi, maxPhi);
  TH2F* Z_map = new TH2F("Z", "Z", binsZ, minZ, maxZ, binsPhi, minPhi, maxPhi);
  // Rho
  TH1F* rho_z   = new TH1F("rho_z", "rho_z", binsZ, minZ, maxZ);
  TH1F* rho_phi = new TH1F("rho_phi", "rho_phi", binsPhi, minPhi, maxPhi);
  TH2F* rho_map
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

  // r/z
  TH2F* glob_r_z = new TH2F("r_z", "r_z", binsZ, minZ, maxZ, binsR, minR, maxR);
  TH2F* assigned_r_z = new TH2F(
      "r_z_assigned", "r_z_assigned", binsZ, minZ, maxZ, binsR, minR, maxR);

  size_t nEntries = z->size();
  for (int i = 0; i < nEntries; i++) {
    // A
    A_z->Fill(z->at(i), A->at(i));
    A_phi->Fill(phi->at(i), A->at(i));
    A_map->Fill(z->at(i), phi->at(i), A->at(i));
    // Z
    Z_z->Fill(z->at(i), Z->at(i));
    Z_phi->Fill(phi->at(i), Z->at(i));
    Z_map->Fill(z->at(i), phi->at(i), Z->at(i));
    // x0
    x0_z->Fill(z->at(i), x0->at(i));
    x0_phi->Fill(phi->at(i), x0->at(i));
    x0_map->Fill(z->at(i), phi->at(i), x0->at(i));
    // l0
    l0_z->Fill(z->at(i), l0->at(i));
    l0_phi->Fill(phi->at(i), l0->at(i));
    l0_map->Fill(z->at(i), phi->at(i), l0->at(i));
    // d
    /*   d_z->Fill(z->at(i), d->at(i));
       d_phi->Fill(phi->at(i), d->at(i));
       d_map->Fill(z->at(i), phi->at(i), d->at(i));*/
    // rho
    rho_z->Fill(z->at(i), rho->at(i));
    rho_phi->Fill(phi->at(i), rho->at(i));
    rho_map->Fill(z->at(i), phi->at(i), rho->at(i));
    // thickness in X0
    dInX0_z->Fill(z->at(i), dInX0->at(i));
    dInX0_phi->Fill(phi->at(i), dInX0->at(i));
    dInX0_map->Fill(z->at(i), phi->at(i), dInX0->at(i));
    // thickness in L0
    dInL0_z->Fill(z->at(i), dInL0->at(i));
    dInL0_phi->Fill(phi->at(i), dInL0->at(i));
    dInL0_map->Fill(z->at(i), phi->at(i), dInL0->at(i));

    // fill r/z
    if (globR->size() && globZ->size())
      glob_r_z->Fill(globZ->at(i), globR->at(i));

    if (assignedGlobZ->size() && assignedGlobR->size())
      assigned_r_z->Fill(assignedGlobZ->at(i), assignedGlobR->at(i));
  }

  // A
  A_z->Write();
  A_phi->Write();
  A_map->Write();
  delete A_z;
  delete A_phi;
  delete A_map;
  // Z
  Z_z->Write();
  Z_phi->Write();
  Z_map->Write();
  delete Z_z;
  delete Z_phi;
  delete Z_map;
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
  // d
  /*d_z->Write();
  d_phi->Write();
  d_map->Write();
  delete d_z;
  delete d_phi;
  delete d_map;*/
  // rho
  rho_z->Write();
  rho_phi->Write();
  rho_map->Write();
  delete rho_z;
  delete rho_phi;
  delete rho_map;
  // thickness in X0
  dInX0_z->Write();
  dInX0_phi->Write();
  dInX0_map->Write();
  delete dInX0_z;
  delete dInX0_phi;
  delete dInX0_map;
  // thickness in L0
  dInL0_z->Write();
  dInL0_phi->Write();
  dInL0_map->Write();
  delete dInL0_z;
  delete dInL0_phi;
  delete dInL0_map;

  delete z;
  delete phi;
  delete A;
  delete Z;
  delete x0;
  delete l0;
  delete d;
  delete rho;
  delete dInX0;
  delete dInL0;

  glob_r_z->SetMarkerColor(45);
  assigned_r_z->SetMarkerColor(46);
  glob_r_z->Write();
  assigned_r_z->Write();
  delete glob_r_z;
  delete assigned_r_z;
  outputFile.Close();
}

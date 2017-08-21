
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TTree.h"

// This script prints a magnetic field map.
// To be used with the Output of the RootInterpolatedBFieldWriter.

void
printBField(std::string inFile,
            std::string treeName,
            std::string outFile,
            float       rmin,
            float       rmax,
            float       zmin,
            float       zmax,
            int         nBins)
{
  const Int_t NRGBs        = 5;
  const Int_t NCont        = 255;
  Double_t    stops[NRGBs] = {0.00, 0.34, 0.61, 0.84, 1.00};
  Double_t    red[NRGBs]   = {0.00, 0.00, 0.87, 1.00, 0.51};
  Double_t    green[NRGBs] = {0.00, 0.81, 1.00, 0.20, 0.00};
  Double_t    blue[NRGBs]  = {0.51, 1.00, 0.12, 0.00, 0.00};
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
  gStyle->SetOptStat(0);

  std::cout << "Opening file: " << inFile << std::endl;
  TFile inputFile(inFile.c_str());
  std::cout << "Reading tree: " << treeName << std::endl;
  TTree* tree = (TTree*)inputFile.Get(treeName.c_str());

  TTreeReader reader(treeName.c_str(), &inputFile);

  double x, y, z, r;
  double Bx, By, Bz, Br;

  tree->SetBranchAddress("x", &x);
  tree->SetBranchAddress("y", &y);
  tree->SetBranchAddress("z", &z);
  tree->SetBranchAddress("r", &r);
  tree->SetBranchAddress("Bx", &Bx);
  tree->SetBranchAddress("By", &By);
  tree->SetBranchAddress("Bz", &Bz);
  tree->SetBranchAddress("Br", &Br);

  Int_t entries = tree->GetEntries();
  std::cout << "Creating new output file: " << outFile
            << " and writing out histograms. " << std::endl;
  TFile outputFile(outFile.c_str(), "recreate");

  TProfile2D* bField_rz = new TProfile2D(
      "BField_rz", "Magnetic Field", nBins, zmin, zmax, nBins * 0.5, 0., rmax);
  bField_rz->GetXaxis()->SetTitle("z [m]");
  bField_rz->GetYaxis()->SetTitle("r [m]");
  TProfile2D* bField_xy = new TProfile2D(
      "BField_xy", "Magnetic Field", nBins, rmin, rmax, nBins, rmin, rmax);
  bField_xy->GetXaxis()->SetTitle("x [m]");
  bField_xy->GetYaxis()->SetTitle("y [m]");
  TProfile2D* bField_yz = new TProfile2D(
      "BField_yz", "Magnetic Field", nBins, zmin, zmax, nBins, rmin, rmax);
  bField_yz->GetXaxis()->SetTitle("z [m]");
  bField_yz->GetYaxis()->SetTitle("y [m]");
  TProfile2D* bField_xz = new TProfile2D(
      "BField_xz", "Magnetic Field", nBins, zmin, zmax, nBins, rmin, rmax);
  bField_xz->GetXaxis()->SetTitle("z [m]");
  bField_xz->GetYaxis()->SetTitle("x [m]");

  for (int i = 0; i < entries; i++) {
    tree->GetEvent(i);

    float bFieldValue = sqrt(Bx * Bx + By * By + Bz * Bz);
    bField_rz->Fill(
        z / 1000., r / 1000., bFieldValue / (1000. * 1.60217733e-19));
    bField_xy->Fill(
        x / 1000., y / 1000., bFieldValue / (1000. * 1.60217733e-19));
    bField_yz->Fill(
        z / 1000., y / 1000., bFieldValue / (1000. * 1.60217733e-19));
    bField_xz->Fill(
        z / 1000., x / 1000., bFieldValue / (1000. * 1.60217733e-19));
  }
  inputFile.Close();

  bField_rz->Write();
  delete bField_rz;
  bField_xy->Write();
  delete bField_xy;
  bField_yz->Write();
  delete bField_yz;
  bField_xz->Write();
  delete bField_xz;

  outputFile.Close();
}

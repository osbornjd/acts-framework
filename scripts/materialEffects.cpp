#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TTree.h"

/// This script prints histograms of the displacement, change in momentum and
/// energy loss.
/// It was created in order to validate Fatras simulation against Geant4.
/// It should can used with the Output of the RootExCellWriter and the
/// MaterialEffectsWriter.

void
materialEffects(std::string inFile,
                std::string treeName,
                std::string outFile,
                int         nBins,
                float       xMin,
                float       xMax,
                float       yMin,
                float       yMax,
                float       zMin,
                float       zMax)
{
  std::cout << "Opening file: " << inFile << std::endl;
  TFile inputFile(inFile.c_str());
  std::cout << "Reading tree: " << treeName << std::endl;
  TTree* tree = (TTree*)inputFile.Get(treeName.c_str());

  TTreeReader reader(treeName.c_str(), &inputFile);

  float dx;
  float dy;
  float dz;

  float dPx;
  float dPy;
  float dPz;

  tree->SetBranchAddress("dx", &dx);
  tree->SetBranchAddress("dy", &dy);
  tree->SetBranchAddress("dz", &dz);

  tree->SetBranchAddress("dPx", &dPx);
  tree->SetBranchAddress("dPy", &dPy);
  tree->SetBranchAddress("dPz", &dPz);

  std::cout << "Creating new output file: " << outFile
            << " and writing out histograms. " << std::endl;
  TFile outputFile(outFile.c_str(), "recreate");

  TH1F* dx_hist = new TH1F("dx_hist", "Displacement in x", nBins, xMin, xMax);
  TH1F* dy_hist = new TH1F("dy_hist", "Displacement in y", nBins, yMin, yMax);
  TH1F* dz_hist = new TH1F("dz_hist", "Displacement in z", nBins, zMin, zMax);

  TH1F* dE_hist = new TH1F("dE", "EnergyLoss", nBins, 0., 5.);

  Int_t entries = tree->GetEntries();

  for (int i = 0; i < entries; i++) {
    tree->GetEvent(i);
    dx_hist->Fill(dx);
    dy_hist->Fill(dy);
    dz_hist->Fill(dz);
    float dE = sqrt(dPx * dPx + dPy * dPy + dPz * dPz);
    dE_hist->Fill(dE);
  }

  dx_hist->GetXaxis()->SetTitle("dx");
  dx_hist->GetYaxis()->SetTitle("#particles");

  dy_hist->GetXaxis()->SetTitle("dy");
  dy_hist->GetYaxis()->SetTitle("#particles");

  dz_hist->GetXaxis()->SetTitle("dz");
  dz_hist->GetYaxis()->SetTitle("#particles");

  dE_hist->GetXaxis()->SetTitle("dE");
  dE_hist->GetYaxis()->SetTitle("#particles");

  dx_hist->Write();
  dy_hist->Write();
  dz_hist->Write();

  dE_hist->Write();

  delete dx_hist;
  delete dy_hist;
  delete dz_hist;

  delete dE_hist;

  inputFile.Close();
  outputFile.Close();
}

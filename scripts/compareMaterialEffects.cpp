#include <iterator>
#include <tuple>
#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TTree.h"

/// For comparison of the material effects for different pT.
/// It was created in order to validate Fatras simulation against Geant4.
/// It should can used with the Output of the materialEffects.cpp script.

void
compareMaterialEffects(std::vector<std::string> dEHistFile_g4,
                       std::vector<std::string> dEHistFile_acts,
                       std::vector<std::string> momenta,
                       std::string              branchName,
                       std::string              histogramTitle)
{
  if (dEHistFile_g4.size() != dEHistFile_acts.size()
      || dEHistFile_g4.size() != momenta.size()
      || dEHistFile_acts.size() != momenta.size()) {
    std::cout << "ERROR, Vectors need to have same size!" << std::endl;
    return;
  }

  TLegend* leg1 = new TLegend(0.2, 0.2, .4, .4);
  leg1->SetHeader("pT");
  gStyle->SetOptStat(0);

  for (size_t it = 0; it < momenta.size(); it++) {
    std::cout << "Opening file: " << dEHistFile_g4.at(it).c_str() << std::endl;
    TFile inputFile_g4(dEHistFile_g4.at(it).c_str());
    TH1F* input_g4 = (TH1F*)inputFile_g4.Get(branchName.c_str());

    std::cout << "Opening file: " << dEHistFile_acts.at(it).c_str()
              << std::endl;
    TFile inputFile_acts(dEHistFile_acts.at(it).c_str());
    TH1F* input_acts = (TH1F*)inputFile_acts.Get(branchName.c_str());

    // set the colors
    input_g4->SetMarkerColor(1);
    //  input_g4->SetFillColor(1);
    input_g4->SetLineColor(1);
    input_g4->SetLineWidth(3);

    input_acts->SetMarkerColor(kOrange + 7);
    //  input_acts->SetFillColor(kOrange);
    input_acts->SetLineColor(kOrange + 7);
    input_acts->SetLineWidth(3);

    input_g4->SetLineStyle(it + 1);
    input_acts->SetLineStyle(it + 1);

    input_acts->GetXaxis()->SetTitle("dz");

    input_g4->GetXaxis()->SetTitle("dz");

    input_acts->SetTitle(histogramTitle.c_str());
    input_g4->SetTitle(histogramTitle.c_str());

    input_acts->Draw("same");
    input_g4->Draw("same");

    if (it == 0) {
      TLegend* leg = new TLegend(0.2, 0.2, .8, .8);
      leg->AddEntry(input_g4, "Geant4");
      leg->AddEntry(input_acts, "ACTS");
      leg->Draw("same");
    }

    leg1->AddEntry(input_g4, momenta.at(it).c_str());

    input_acts->SetDirectory(0);
    input_g4->SetDirectory(0);

    inputFile_g4.Close();
    inputFile_acts.Close();
  }

  // make a common canvas

  leg1->Draw();
}

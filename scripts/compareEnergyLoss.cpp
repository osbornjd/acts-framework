// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iterator>
#include <tuple>
#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TTree.h"

/// For comparison of the energy loss for different pT.
/// It was created in order to validate Fatras simulation against Geant4.
/// It should can used with the Output of the materialEffects.cpp script.
/// Example:
/// std::vector<std::string> actsFiles =
/// {"actsMaterialEffects_500MeV.root","actsMaterialEffects_1GeV.root","actsMaterialEffects_10GeV.root","actsMaterialEffects_20GeV.root","actsMaterialEffects_30GeV.root","actsMaterialEffects_40GeV.root","actsMaterialEffects_50GeV.root","actsMaterialEffects_60GeV.root","actsMaterialEffects_70GeV.root","actsMaterialEffects_80GeV.root","actsMaterialEffects_90GeV.root","actsMaterialEffects_100GeV.root"}
/// std::vector<std::string> g4Files =
/// {"g4MaterialEffects_500MeV.root","g4MaterialEffects_1GeV.root","g4MaterialEffects_10GeV.root","g4MaterialEffects_20GeV.root","g4MaterialEffects_30GeV.root","g4MaterialEffects_40GeV.root","g4MaterialEffects_50GeV.root","g4MaterialEffects_60GeV.root","g4MaterialEffects_70GeV.root","g4MaterialEffects_80GeV.root","g4MaterialEffects_90GeV.root","g4MaterialEffects_100GeV.root"}
/// std::vector<float> momenta = {500,1000,10000,
/// 20000,30000,40000,50000,60000,70000,80000,90000,100000}
/// .L scripts/compareEnergyLoss.cpp
/// compareEnergyLoss(g4Files,actsFiles,momenta,1000,0.,0.5,1000,0.,0.1)

void
compareEnergyLoss(std::vector<std::string> dEHistFile_g4,
                  std::vector<std::string> dEHistFile_acts,
                  std::vector<float>       momenta,
                  size_t                   mp_nBins,
                  float                    mp_min,
                  float                    mp_max,
                  size_t                   s_nBins,
                  float                    s_min,
                  float                    s_max)
{
  // G4
  TH2F* mp_g4 = new TH2F("mp_g4",
                         "Most Probable Value of Energyloss",
                         1000,
                         100.,
                         100100.,
                         mp_nBins,
                         mp_min,
                         mp_max);
  TH2F* s_g4 = new TH2F("s_g4",
                        "Standard Deviation of Energyloss",
                        1000,
                        100.,
                        100100.,
                        s_nBins,
                        s_min,
                        s_max);

  TH2F* mp_acts = new TH2F("mp_acts",
                           "Most Probable Value of Energyloss",
                           1000,
                           100.,
                           100100.,
                           mp_nBins,
                           mp_min,
                           mp_max);
  TH2F* s_acts = new TH2F("s_acts",
                          "Standard Deviation of Energyloss",
                          1000,
                          100.,
                          100100.,
                          s_nBins,
                          s_min,
                          s_max);

  TH2F* scalor = new TH2F("scalor",
                          "Scalor from Fatras to Geant4 for #sigma(dE)",
                          1000,
                          100.,
                          100100.,
                          s_nBins,
                          0.,
                          1.);

  TH2F* scalorMOP = new TH2F("scalor",
                             "Scalor from Fatras to Geant4 for MOP(dE)",
                             1000,
                             100.,
                             100100.,
                             s_nBins,
                             0.,
                             1.);

  if (dEHistFile_g4.size() != dEHistFile_acts.size()
      || dEHistFile_g4.size() != momenta.size()
      || dEHistFile_acts.size() != momenta.size()) {
    std::cout << "ERROR, Vectors need to have same size!" << std::endl;
    return;
  }

  for (size_t it = 0; it < momenta.size(); it++) {
    std::cout << "Opening file: " << dEHistFile_g4.at(it).c_str() << std::endl;
    TFile inputFile_g4(dEHistFile_g4.at(it).c_str());
    TH1F* input_g4 = (TH1F*)inputFile_g4.Get("dE");

    std::cout << "Opening file: " << dEHistFile_acts.at(it).c_str()
              << std::endl;
    TFile inputFile_acts(dEHistFile_acts.at(it).c_str());
    TH1F* input_acts = (TH1F*)inputFile_acts.Get("dE");

    input_g4->Fit("landau");
    input_acts->Fit("landau");

    float mostPropG4   = input_g4->GetFunction("landau")->GetParameter(1);
    float mostPropActs = input_acts->GetFunction("landau")->GetParameter(1);

    float sigmaG4   = input_g4->GetFunction("landau")->GetParameter(2);
    float sigmaActs = input_acts->GetFunction("landau")->GetParameter(2);
    float momentum  = momenta.at(it);

    float scalFactor = sigmaG4 / sigmaActs;
    float scalMean   = mostPropG4 / mostPropActs;

    std::cout << "----Geant4----" << std::endl;
    std::cout << "mp: " << mostPropG4 << ", sigma: " << sigmaG4 << std::endl;
    std::cout << "----Acts----" << std::endl;
    std::cout << "Scalor: " << scalFactor << std::endl;
    std::cout << "momentum: " << momentum << std::endl;

    mp_g4->Fill(momentum, mostPropG4);
    s_g4->Fill(momentum, sigmaG4);

    mp_acts->Fill(momentum, mostPropActs);
    s_acts->Fill(momentum, sigmaActs);

    scalor->Fill(momentum, scalFactor);
    scalorMOP->Fill(momentum, scalMean);

    inputFile_g4.Close();
    inputFile_acts.Close();
  }

  // make a common canvas
  TCanvas* c1 = new TCanvas();
  gStyle->SetOptStat(0);
  c1->Divide(1, 2);
  c1->cd(1);

  mp_g4->SetMarkerColor(1);
  mp_g4->SetLineColor(1);
  mp_g4->SetMarkerStyle(34);
  mp_g4->GetXaxis()->SetTitle("momentum [MeV]");
  mp_g4->GetYaxis()->SetTitle("MOP of dE");
  mp_g4->Draw("");

  mp_acts->SetMarkerColor(2);
  mp_acts->SetLineColor(2);
  mp_acts->SetMarkerStyle(34);
  mp_acts->GetXaxis()->SetTitle("momentum [MeV]");
  mp_acts->GetYaxis()->SetTitle("MOP of dE");
  mp_acts->Draw("same");

  TLegend* leg = new TLegend(0.72, 0.696, 0.99, 0.936);
  leg->AddEntry(mp_g4, "g4");
  leg->AddEntry(mp_acts, "acts");
  leg->Draw();
  mp_g4->SetDirectory(0);
  mp_acts->SetDirectory(0);

  // second pad
  c1->cd(2);
  scalorMOP->Fit("pol0");
  // scalor->Divide(s_g4);
  scalorMOP->SetMarkerColor(4);
  scalorMOP->SetMarkerStyle(34);
  scalorMOP->GetXaxis()->SetTitle("momentum [MeV]");
  scalorMOP->GetYaxis()->SetTitle("scalor");
  scalorMOP->Draw("same");
  scalorMOP->SetDirectory(0);

  TCanvas* c2 = new TCanvas();
  c2->Divide(1, 2);
  c2->cd(1);
  s_g4->SetMarkerColor(1);
  s_g4->SetLineColor(1);
  s_g4->SetMarkerStyle(34);
  s_g4->GetXaxis()->SetTitle("momentum [MeV]");
  s_g4->GetYaxis()->SetTitle("#sigma(dE)");
  s_g4->Draw("");
  s_acts->SetMarkerColor(2);
  s_acts->SetLineColor(2);
  s_acts->SetMarkerStyle(34);
  s_acts->GetXaxis()->SetTitle("momentum [MeV]");
  s_acts->GetYaxis()->SetTitle("#sigma(dE)");
  s_acts->Draw("same");
  TLegend* leg2 = new TLegend(0.72, 0.696, 0.99, 0.936);
  leg2->AddEntry(s_g4, "g4");
  leg2->AddEntry(s_acts, "acts");
  leg2->Draw();

  c2->cd(2);

  scalor->Fit("pol0");
  scalor->SetMarkerColor(4);
  scalor->SetMarkerStyle(34);
  scalor->GetXaxis()->SetTitle("momentum [MeV]");
  scalor->GetYaxis()->SetTitle("scalor");
  scalor->Draw("same");
  scalor->SetDirectory(0);
  s_g4->SetDirectory(0);
  s_acts->SetDirectory(0);
}

/*
 * compareAssignedRealPos.cpp
 *
 *  Created on: 16 Dec 2016
 *      Author: jhrdinka
 */

#include <tuple>
#include "TFile.h"
#include "TH1F.h"
#include "TIterator.h"
#include "TROOT.h"
#include "TTree.h"

// This root script prints global real position of a cylinder layers in darker
// color and the assigned positions in corresponding lighter color.
// All the layers which should be printed into one Canvas need to be in the same
// file. Every layer has its own directory with the contained material
// histograms.
// This script is foreseen to use the input of scripts/layerMaterial.cpp

void
compareAssignedRealPos(std::string inFile)
{
  std::cout << "Opening file: " << inFile << std::endl;
  TFile  inputFile(inFile.c_str());
  TList* layers = inputFile.GetListOfKeys();
  std::cout << "Layers to print: " << std::endl;
  layers->Print();
  TIter    next(layers);
  TObject* obj = 0;

  int      entry  = 2;
  TCanvas* canvas = new TCanvas();
  while ((obj = next())) {
    TDirectory* dir          = inputFile.GetDirectory(obj->GetName());
    TH2F*       r_z          = (TH2F*)dir->Get("r_z");
    TH2F*       r_z_assigned = (TH2F*)dir->Get("r_z_assigned");
    if (r_z) {
      r_z->SetMarkerColor(TColor::GetColorDark(entry));
      r_z->Draw("same");
      r_z_assigned->SetMarkerColor(TColor::GetColorBright(entry));
      r_z_assigned->Draw("same");
      r_z->SetDirectory(0);
      r_z_assigned->SetDirectory(0);
    }
    entry++;
  }

  inputFile.Close();
}

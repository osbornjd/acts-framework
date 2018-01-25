// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_ROOTGRIDUTILS_H
#define ACTFW_PLUGINS_ROOTGRIDUTILS_H

#include "ACTS/Utilities/detail/Axis.hpp"
#include "ACTS/Utilities/detail/Grid.hpp"
#include <TH2F.h>


namespace FW {

namespace Root {

  typedef Acts::detail::EquidistantAxis EAxis;
  typedef Acts::detail::Grid<double, EAxis, EAxis> Grid2D;
  
  /// transform a TAxis into an equidist Grid axis
  /// @param tAxis the root TAxis 
  static
  EAxis 
  taxisToEAxis(const TAxis& taxis){
    // get & create the axes
    size_t bins = taxis.GetNbins();
    double xmin = taxis.GetXmin();
    double xmax = taxis.GetXmax();
    // return the axis
    return EAxis(xmin,xmax,bins);
  }
  
  /// return a grid from a histogram in 2d
  /// @param hist the root hist
  /// @param defval directive and potential defaul value
  /// @param filluo directive to fill underfow/overflow
  static  
  Grid2D 
  histToGrid(const TH2F& hist, 
             std::pair<bool,double> defval = {false, 0.}, 
             bool filluo = false){
    // convert the two exis
    auto axis0 = taxisToEAxis(*hist.GetXaxis());
    auto axis1 = taxisToEAxis(*hist.GetYaxis());
    // 
    Grid2D grid(
        std::make_tuple(std::move(axis0), std::move(axis1)));
    // number bins including overflow
    size_t nbins0o = grid.getNBins().at(0)+1;
    size_t nbins1o = grid.getNBins().at(1)+1;
    // do the loop   
    for (size_t i0 = 0; i0 <= nbins0o; ++i0)
      for (size_t i1 = 0; i1 <= nbins1o; ++i1){
        // create an index s        
        Grid2D::index_t i01 = {{i0, i1}};
        // the request bins
        size_t ri0 = i0;
        size_t ri1 = i1;
        // fill the grid - check if you want to force overwrite
        // under and overflow bins
        if (filluo && (!i0 || !i1 || i0 == nbins0o || i1 == nbins1o)){
          // correct underflow/overflow request to inside bins 
          ri0 = (!i0) ? 1 : ((i0 == nbins0o) ? size_t(nbins0o-1) : i0);
          ri1 = (!i1) ? 1 : ((i1 == nbins1o) ? size_t(nbins1o-1) : i1);
        } 
        // now the the value with the request bins
        double tbc = hist.GetBinContent(ri0,ri1);
        // check if default value is required
        if (defval.first and tbc == 0.)
          tbc = defval.second;
        grid.at(i01) = tbc;
    }  
    // now return the grid
    return grid;   
  }
  
  } // namespace Root
} // namespace FW

#endif
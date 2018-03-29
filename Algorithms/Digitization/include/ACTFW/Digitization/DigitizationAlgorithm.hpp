// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_ALGORITHMS_DIGITIZATIONALGORITHM_H
#define ACTFW_ALGORITHMS_DIGITIZATIONALGORITHM_H

#include <memory>
#include <map>
#include <algorithm>    // std::random_shuffle
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/detail/Axis.hpp"
#include "ACTS/Utilities/detail/Grid.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/EventData/TrackParameters.hpp"

namespace Acts {
  class PlanarModuleStepper;
  class TrackingGeometry;
  typedef detail::Grid<double, detail::EquidistantAxis, detail::EquidistantAxis> ResolutionGrid;
  typedef std::pair<ResolutionGrid,ResolutionGrid> LayerResolution;
  typedef std::map<Acts::GeometryID, std::shared_ptr<Acts::LayerResolution> > ResolutionMap;
}

namespace FW {

class RandomNumbersSvc;

class DigitizationAlgorithm : public FW::BareAlgorithm
{
    
public:
  /// Nested configuration Config
  struct Config
  {
    /// input hit collection
    std::string simulatedHitsCollection;
    /// output space point collection
    std::string spacePointsCollection;
    /// output clusters collection
    std::string clustersCollection;
    /// output hits per particle Collection
    std::string hitsPerParticleCollection;
    /// FW random number service
    std::shared_ptr<RandomNumbersSvc> randomNumbers = nullptr;
    /// module stepper
    std::shared_ptr<Acts::PlanarModuleStepper> planarModuleStepper = nullptr;
    /// the reader for the resolution file, fixed to 2D for now
    std::shared_ptr<Acts::ResolutionMap> layerResolutions = nullptr;
    /// the tracking geometry to make the module map
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;
    /// read in by hand if the layer is of r-type and z-type     
    std::map< Acts::GeometryID, int > layerTypes;
    /// amount of smearing
    double smearParameter = 0.1; // accounts for n % gaussian width
    /// cut parameter - percentage of thickness to be traversed;
    double cutParameter   = 0.; //15; //  
    /// inefficiency for full hits
    double hitInefficiency = 0.01;
    /// maximum incidence angle cut-off 
    double cosThetaLocMin = 0.1;
    /// random/noise hits production probability per module
    // double randomHitsProb  = 0.5;
        
  };
  
  /// Constructor
  /// @param cnf the configuration class
  /// @param level the log output level
  DigitizationAlgorithm(const Config&        cnf,
                        Acts::Logging::Level level = Acts::Logging::INFO);

  /// Framework execute method
  /// @param ctx the algorithm context to respect thread safety
  /// @return a process code 
  FW::ProcessCode
  execute(FW::AlgorithmContext ctx) const final override;
  
  /// Templated method to create extra hits
  template <typename DataContainer, 
            typename RandomGen, 
            typename DistP, 
            typename DistF >
  
  /// Create extra/sprurious hits
  size_t
  createExtraHits(DataContainer& dc, 
                  RandomGen& rng, 
                  DistP& pDist, 
                  DistF& fDist,
                  const Acts::TrackingVolume& tvolume) const;
  

private:
  Config m_cfg;
};

template <typename DataContainer, 
          typename RandomGen, 
          typename DistP, 
          typename DistF >
size_t
DigitizationAlgorithm::createExtraHits(DataContainer& dc, 
                                       RandomGen& rng, 
                                       DistP& pDist, 
                                       DistF& fDist,
                                       const Acts::TrackingVolume& tvolume) const 
{
  size_t ehits = 0;
  // process sub volumes first 
  if (tvolume.confinedVolumes()){
    for (auto& tv : tvolume.confinedVolumes()->arrayObjects())
      ehits += createExtraHits(dc,rng,pDist,fDist,*tv);
  }
  // 
  if (tvolume.confinedLayers()){
    for (auto& lay : tvolume.confinedLayers()->arrayObjects()){
      if (lay->surfaceArray()){
        for (auto& moduleSurface : lay->surfaceArray()->arrayObjects()){
          // get the geo_id to fill into the collections
          
          // (2) add random some random hits
          size_t nHits = moduleSurface ? pDist(rng) : 0;
          if (!nHits) continue;

          // let's create a random hit
          auto vStore = moduleSurface->bounds().valueStore();
          /// volume, layer and module id
          geo_id_value volumeID
              = moduleSurface->geoID().value(Acts::GeometryID::volume_mask);
          geo_id_value layerID 
             = moduleSurface->geoID().value(Acts::GeometryID::layer_mask);
          geo_id_value moduleID
             = moduleSurface->geoID().value(Acts::GeometryID::sensitive_mask);
          // loop over the random hits to be produced
          for (size_t ih = 0; ih < nHits; ++ih){
            // throw a local position in Y
            double locX = 0.;          
            double locY = 0.;          
            if (vStore.size() == 2){
              // rectangle case
              locX = (-1 + 2*fDist(rng))*vStore[0];
              locY = (-1 + 2*fDist(rng))*vStore[1];
            } else if (vStore.size() == 3){
              // trapezoidal case
              double maxY = vStore[2];
              locY = (-1 + 2*fDist(rng))*maxY;
              double minX = vStore[0];
              double maxX = vStore[1];
              double kXY = (maxX-minX)/(2*maxY);
              double maxXatY = minX + kXY*(locY+maxY); 
              locX = (-1 + 2*fDist(rng))*maxXatY;
            }
          
            // we have the two local coordinates
            // now generate phi and theta
            // we want them to loosely correlate with the 
            // phi / theta direction where the surface sits
            // this is to avoid randomly big clusters again
            double cphi   = moduleSurface->center().phi();
            double ctheta = moduleSurface->center().theta();
            double rphi   = cphi + (-1+2*fDist(rng))*0.25*M_PI;
            double rtheta = ctheta + (-1+2*fDist(rng))*0.25*M_PI;
            double qop = 0.000001;
          
            Acts::ActsVector<double, Acts::NGlobalPars> rpvec;
            rpvec << locX,locY,rphi,rtheta,qop;
            Acts::BoundParameters* rParameters
               = new Acts::BoundParameters(nullptr,rpvec,*moduleSurface);
            // fill into the new vector
            std::pair<std::unique_ptr<const Acts::TrackParameters>, barcode_type> eHit(
                std::move(rParameters), barcode_type(0));
            FW::Data::insert(
                dc, volumeID, layerID, moduleID, std::move(eHit));
            // increase the random hit counter 
            ++ehits;
          } // loop over random hits
          
          
        }
      }
    }
  }
  return ehits;
  
}

}  // namespace FW

#endif

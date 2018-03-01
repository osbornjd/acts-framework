// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>    // std::random_shuffle
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTS/Detector/DetectorElementBase.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/PlanarModuleStepper.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/ParameterDefinitions.hpp"

FW::DigitizationAlgorithm::DigitizationAlgorithm(
    const FW::DigitizationAlgorithm::Config& cfg,
    Acts::Logging::Level                     level)
  : FW::BareAlgorithm("DigitizationAlgorithm", level), m_cfg(cfg)
{
  if (m_cfg.simulatedHitsCollection.empty()) {
    throw std::invalid_argument("Missing input hits collection");
  } else if (m_cfg.spacePointsCollection.empty()) {
    throw std::invalid_argument("Missing output space points collection");
  } else if (m_cfg.clustersCollection.empty()) {
    throw std::invalid_argument("Missing output clusters collection");
  } else if (!m_cfg.randomNumbers) {
    throw std::invalid_argument("Missing random numbers service");
  } else if (!m_cfg.planarModuleStepper) {
    throw std::invalid_argument("Missing planar module stepper");
  }
}

FW::ProcessCode
FW::DigitizationAlgorithm::execute(FW::AlgorithmContext ctx) const
{
  
  size_t simulated_hits   = 0;
  size_t dropped_hits     = 0;
  size_t random_hits      = 0;
  size_t skipped_modules  = 0;
  
  // prepare the input data
  const FW::DetectorData<geo_id_value,
                         std::pair<std::unique_ptr<const Acts::TrackParameters>,
                                   barcode_type>>* hitData
      = nullptr;
  // read and go
  if (ctx.eventStore.get(m_cfg.simulatedHitsCollection, hitData)
      == FW::ProcessCode::ABORT)
    return FW::ProcessCode::ABORT;

  ACTS_DEBUG("Retrieved hit data '" << m_cfg.simulatedHitsCollection
                                    << "' from event store.");

  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(ctx);

  // Setup random number distributions for some quantities
  FW::GaussDist   gDist(0.,1.);
  FW::UniformDist fDist(0.,1.);
  FW::PoissonDist pDist(1);
      
  // the particle mass table
  Acts::ParticleMasses pMasses;

  // prepare the output data: Clusters
  FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster> planarClusters;
  // perpare the second output data : SpacePoints
  FW::DetectorData<geo_id_value, Acts::Vector3D> spacePoints;

  // now digitise
  for (auto& vData : (*hitData)) {
    
    auto volumeKey = vData.first;
    ACTS_DEBUG("- Processing Volume Data collection for volume with ID "
               << volumeKey);
    for (auto& lData : vData.second) {
      auto layerKey = lData.first;
      ACTS_DEBUG("-- Processing Layer Data collection for layer with ID "
                 << layerKey);
      
      // create the indetifier for the resolution file
      // this only depends on volume and layer 
      Acts::GeometryID resMapID(0);
      resMapID.add(volumeKey, Acts::GeometryID::volume_mask);
      resMapID.add(layerKey, Acts::GeometryID::layer_mask);
      // resolution map
      std::shared_ptr<Acts::LayerResolution> lResolutions = nullptr;
      int lType = -1;
      // check if you have resolution maps 
      if (m_cfg.layerResolutions){
        // try to find the map from the provided funtions
        // @TODO change into a reader structure at initialize
        auto rMap  = m_cfg.layerResolutions->find(resMapID);
        auto rType = m_cfg.layerTypes.find(resMapID);
          
        if (rMap != m_cfg.layerResolutions->end()
          && rType != m_cfg.layerTypes.end()){
          // assign the layer Map  
          lResolutions = rMap->second;
          // assign the layer Type
          lType = rType->second;
        }
      }
      
      for (auto& sData : lData.second) {

        // for memory cleanup
        std::vector<Acts::BoundParameters*> mcleanup;

        
        auto moduleKey = sData.first;
        ACTS_DEBUG("-- Processing Module Data collection for module with ID "
                   << moduleKey);
        
        // @HACK for TrackML
        typedef std::pair<const Acts::TrackParameters*, barcode_type > HitAndBc;
        std::vector< HitAndBc > processedHits;
        processedHits.reserve(sData.second.size()+10);
  
        const Acts::Surface* moduleSurface = 0;
  
        // (1) fill the hits from simulation parameters
        for (auto& hit : sData.second) {
          // throw a certain number of hit away if configured
          if (m_cfg.hitInefficiency != 0. 
              && fDist(rng) < m_cfg.hitInefficiency) {
                ++dropped_hits;
                continue;
          }
          // one simulated hit
          ++simulated_hits;
          // get the hit parametes and the barcode
          auto hitPars   = hit.first.get();
          auto partBc = hit.second;
          // fill into the new vector
          processedHits.push_back(HitAndBc(hitPars,partBc)); 
          // story the module surface
          moduleSurface = &hitPars->referenceSurface();
        }
        
        // (2) add random some random hits
        size_t nHits = moduleSurface ? pDist(rng) : 0;
        if (!nHits) ++skipped_modules;
        
        for (size_t ih = 0; ih < nHits; ++ih){
          
          // let's create a random hit
          // 
          auto vStore = moduleSurface->bounds().valueStore();
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
          
          // we have the two local coorinates
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
          processedHits.push_back(HitAndBc(rParameters,0)); 
          mcleanup.push_back(rParameters);
          // increase the random hit counter 
          ++random_hits;
        } // loop over random hits
        
        // randomize the position in the processedHits vector
        std::random_shuffle(processedHits.begin(),processedHits.end());
                
        // get the hit parameters
        for (auto& hit : processedHits) {
          // process as is
          auto hitParameters   = hit.first;
          auto particleBarcode = hit.second;
          // get the surface
          const Acts::Surface& hitSurface = hitParameters->referenceSurface();
          // get the DetectorElement
          auto hitDetElement = hitSurface.associatedDetectorElement();
          if (hitDetElement) {
            // get the digitization module
            auto hitDigitizationModule = hitDetElement->digitizationModule();
            if (hitDigitizationModule) {
              // get the lorentz angle
              double lorentzAngle = hitDigitizationModule->lorentzAngle();
              double thickness    = hitDetElement->thickness();
              double lorentzShift = thickness * tan(lorentzAngle);
              lorentzShift *= -(hitDigitizationModule->readoutDirection());
              // parameters
              auto           pars     = hitParameters->parameters();
              auto           position = hitParameters->position();
              auto           momentum = hitParameters->momentum();
              Acts::Vector2D localIntersection(pars[Acts::ParDef::eLOC_0],
                                               pars[Acts::ParDef::eLOC_1]);
              Acts::Vector3D localDirection(
                  hitSurface.transform().inverse().linear() * momentum.unit());

              // we ignore extreme incident angles
              if (std::abs(localDirection.z())<m_cfg.cosThetaLocMin) continue;
              
              // position
              std::vector<Acts::DigitizationStep> dSteps
                  = m_cfg.planarModuleStepper->cellSteps(*hitDigitizationModule,
                                                         localIntersection,
                                                         localDirection);
              // everything under threshold or edge effects
              if (!dSteps.size()) continue;
              /// let' create a cluster position - centroid method  
              double localXana  = 0.;
              double localYana  = 0.;
              double totalPath  = 0.;
              /// let's create a cluster position - digitial
              double localXdig  = 0.;
              double localYdig  = 0.;
              // min max bins for the cluster size
              std::vector<int> channels0;
              std::vector<int> channels1;
              // the cells to be used
              std::vector<Acts::DigitizationCell> usedCells;
              usedCells.reserve(dSteps.size());
              // loop over the steps
              for (auto dStep : dSteps) {
                // smearing of the step length
                double sLength = dStep.stepLength;
                if (m_cfg.smearParameter != 0.){
                  // smear the path length with (1 +/- epsilon)
                  sLength *= (1.+m_cfg.smearParameter * gDist(rng) );
                }
                // smear witht the total step length
                localXana += sLength * dStep.stepCellCenter.x();
                localYana += sLength * dStep.stepCellCenter.y();
                // the digital cluster position
                localXdig += dStep.stepCellCenter.x();
                localYdig += dStep.stepCellCenter.y();
                // check if it falls below threshold
                if (m_cfg.cutParameter != 0. 
                    && sLength < m_cfg.cutParameter*thickness)
                  continue;
                // smeared, passed and taken
                totalPath += sLength;
                // write the path length : digital for strips
                double wLength = (volumeKey < 10) ? sLength : 1.;
                // introduce cut value for step size 
                usedCells.push_back(
                    std::move(Acts::DigitizationCell(dStep.stepCell.channel0,
                                                     dStep.stepCell.channel1,
                                                     wLength)));
                // recorod to get the min/max
                channels0.push_back(dStep.stepCell.channel0);
                channels1.push_back(dStep.stepCell.channel1);
              }
              
              // check if any cell has been used
              if (!usedCells.size())
                continue;
              
              // divide by the total path - analog clustering
              localXana /= totalPath;
              localXana += lorentzShift;
              localYana /= totalPath;
              
              // divide by the number of pixel - digital clustering
              localXdig /= double(usedCells.size());
              localXdig += lorentzShift;
              localYdig /= double(usedCells.size());

              // get the segmentation & find the corresponding cell id
              const Acts::Segmentation& segmentation
                  = hitDigitizationModule->segmentation();
              auto           binUtility = segmentation.binUtility();
              
              // use digital clustering ? @HACK for Tracking ML 
              // only Pixel detector has analog clustering
              bool analog = ( (volumeKey < 10) && usedCells.size() > 1 );
              Acts::Vector2D localPosition =  analog ?
                 Acts::Vector2D(localXana, localYana) 
              :  Acts::Vector2D(localXdig, localYdig);
              // @todo remove unneccesary conversion
              size_t bin0          = binUtility.bin(localPosition, 0);
              size_t bin1          = binUtility.bin(localPosition, 1);
              size_t binSerialized = binUtility.serialize({bin0, bin1, 0});
              int    diff0 =  (*std::max_element(channels0.begin(),channels0.end()))
                            -(*std::min_element(channels0.begin(),channels0.end()));
              int    diff1 =  (*std::max_element(channels1.begin(),channels1.end()))
                            -(*std::min_element(channels1.begin(),channels1.end()));
              // the covariance is currently set to 0.
              Acts::ActsSymMatrixD<2> cov;
              double resL0 = 0.;
              double resL1 = 0.;
              if (lResolutions){
                // we need to get the position of the cluster, apply local2global
                Acts::Vector3D globalPosition(0.,0.,0.);
                hitSurface.localToGlobal(localPosition, 
                                         Acts::Vector3D(1.,1.,0.),
                                         globalPosition);
                // z position / r position
                // cylinder = 1
                // disc     = 2                         
                double rz = lType == 1 ? globalPosition.z() : 
                                         globalPosition.perp();
                // the lookup                  
                std::array<double,2> l0Lookup = { rz , diff0+0.5 };  
                std::array<double,2> l1Lookup = { rz , diff1+0.5 };  
                // the resolution maps for first and second coordinate                            
                // @TODO solve later for ND coordinates                            
                auto l0Resolution = lResolutions->first;
                auto l1Resolution = lResolutions->second;
                // get the resolutions
                resL0 =  l0Resolution.interpolate(l0Lookup);
                resL1 =  l1Resolution.interpolate(l1Lookup); 
              } 
              // fill the covaraiance matrix 
              cov << resL0*resL0, 0., 0., resL1*resL1;

              // create the indetifier
              Acts::GeometryID geoID(0);
              geoID.add(volumeKey, Acts::GeometryID::volume_mask);
              geoID.add(layerKey, Acts::GeometryID::layer_mask);
              geoID.add(moduleKey, Acts::GeometryID::sensitive_mask);
              geoID.add(binSerialized, Acts::GeometryID::channel_mask);
              // create the truth for this - assume here muons
              Acts::ParticleProperties pProperties(
                  momentum, pMasses.mass[Acts::muon], 1., 13, particleBarcode);
              // the associated process vertex
              Acts::ProcessVertex pVertex(position, 0., 0, {pProperties}, {});

              // create the planar cluster
              Acts::PlanarModuleCluster pCluster(hitSurface,
                                                 Identifier(geoID.value()),
                                                 std::move(cov),
                                                 localPosition.x(),
                                                 localPosition.y(),
                                                 std::move(usedCells),
                                                 {pVertex});

              // insert into the space point map
              FW::Data::insert(spacePoints,
                               volumeKey,
                               layerKey,
                               moduleKey,
                               hitParameters->position());

              // insert into the cluster map
              FW::Data::insert(planarClusters,
                               volumeKey,
                               layerKey,
                               moduleKey,
                               std::move(pCluster));

            }  // hit moulde protection
            
            
            
          }    // hit element protection
        }      // hit loop
        
        // memory cleanup per module
        for (auto& mparams : mcleanup)
          delete mparams;
        
      }        // moudle loop
    }          // layer loop
  }            // volume loop

  // write the SpacePoints to the EventStore
  if (ctx.eventStore.add(m_cfg.spacePointsCollection, std::move(spacePoints))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  // write the clusters to the EventStore
  if (ctx.eventStore.add(m_cfg.clustersCollection, std::move(planarClusters))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  ACTS_INFO("This event had " << simulated_hits << " simulated vs. " << random_hits << " injected random hits.");
  ACTS_INFO("Hits dropped tue to inefficiencies         : " << dropped_hits );
  ACTS_INFO("Modules skipped due to poisson fluctuation : " << skipped_modules );

  return FW::ProcessCode::SUCCESS;
}

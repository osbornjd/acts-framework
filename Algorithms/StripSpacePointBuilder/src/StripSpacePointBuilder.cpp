// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/StripSpacePointBuilder/StripSpacePointBuilder.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include <fstream>

///
/// @note Used abbreviation: "Strip Detector Element" -> SDE
///

FW::StripSpacePointBuilder::StripSpacePointBuilder(const Config& cfg,
    Acts::Logging::Level                      level)
  :  m_cfg(cfg), FW::BareAlgorithm("SpacePointBuilder", level)
{
  // Check that all mandatory configuration parameters are present
  if (m_cfg.collectionIn.empty()) {
    throw std::invalid_argument("Missing input collection");
  }

  if (m_cfg.collectionOut.empty()) {
    throw std::invalid_argument("Missing output collection");
  }
}

FW::ProcessCode
FW::StripSpacePointBuilder::clusterReading(AlgorithmContext& ctx,
                                           const DetData*&   detData) const
{
  // Load hit data from Whiteboard
  if (ctx.eventStore.get(m_cfg.collectionIn, detData)
      != FW::ProcessCode::SUCCESS) {
    ACTS_DEBUG("Unable to receive event data");
    return FW::ProcessCode::ABORT;
  }

  ACTS_DEBUG("Event data successfully received");
  return FW::ProcessCode::SUCCESS;
}

void
FW::StripSpacePointBuilder::storeSpacePoint(const Acts::SingleHitSpacePoint& spacePoint,
                                            DetData& stripClusters) const
{
  // Receive the identification of the digitized hits on the first surface
  Identifier       id(spacePoint.hitModule->identifier());
  Acts::GeometryID geoID(id.value());

  // The covariance is currently set to 0.
  Acts::ActsSymMatrixD<2> cov;
  cov << 0., 0., 0., 0.;

  // Get the local coordinates of the space point
  Acts::Vector2D local;
  spacePoint.hitModule->referenceSurface().globalToLocal(
      spacePoint.spacePoint, {0., 0., 0.}, local);

  // Build the space point
  Acts::PlanarModuleCluster pCluster(
      spacePoint.hitModule->referenceSurface(),
      Identifier(geoID.value()),
      std::move(cov),
      local[0],
      local[1],
      std::move(spacePoint.hitModule->digitizationCells()),
      {spacePoint.hitModule->truthVertices()});

  // Insert into the cluster map
  FW::Data::insert(stripClusters,
                   geoID.value(Acts::GeometryID::volume_mask),
                   geoID.value(Acts::GeometryID::layer_mask),
                   geoID.value(Acts::GeometryID::sensitive_mask),
                   std::move(pCluster));
}

void
FW::StripSpacePointBuilder::storeSpacePoint(const Acts::DoubleHitSpacePoint& spacePoint,
                                            DetData& stripClusters) const
{
  // Receive the identification of the digitized hits on the first surface
  Identifier       id(spacePoint.hitModuleFront[0]->identifier());
  Acts::GeometryID geoID(id.value());

  // The covariance is currently set to 0.
  Acts::ActsSymMatrixD<2> cov;
  cov << 0., 0., 0., 0.;

  // Get the local coordinates of the space point
  Acts::Vector2D local;
  spacePoint.hitModuleFront[0]->referenceSurface().globalToLocal(
      spacePoint.spacePoint, {0., 0., 0.}, local);
      
  std::vector<Acts::ProcessVertex> vertices;
  for(auto& vertex : spacePoint.hitModuleFront)
	vertices.insert(vertices.end(), vertex->truthVertices().begin(), vertex->truthVertices().end());

  // Build the space point
  Acts::PlanarModuleCluster pCluster(
      spacePoint.hitModuleFront[0]->referenceSurface(),
      Identifier(geoID.value()),
      std::move(cov),
      local[0],
      local[1],
      std::move(spacePoint.hitModuleFront[0]->digitizationCells()),
      vertices);

  // Insert into the cluster map
  FW::Data::insert(stripClusters,
                   geoID.value(Acts::GeometryID::volume_mask),
                   geoID.value(Acts::GeometryID::layer_mask),
                   geoID.value(Acts::GeometryID::sensitive_mask),
                   std::move(pCluster));
}

void
FW::StripSpacePointBuilder::writeData(const std::vector<Acts::SingleHitSpacePoint>& pixelSP, const std::vector<Acts::DoubleHitSpacePoint>& sctSP) const
{
	std::ofstream ofs("summary.txt");
	
	for(auto& pixel : pixelSP)
		for(auto& vertex : pixel.hitModule->truthVertices())
		{
			Acts::Vector3D trueHit, resolvedHit, digHit;
			trueHit = vertex.position();
			resolvedHit = pixel.spacePoint;
			ofs << "Pixel " << trueHit(0) << " " << trueHit(1) << " " << trueHit(2) << " " << resolvedHit(0) << " " << resolvedHit(1) << " " << resolvedHit(2) << " ";
			
			auto par = pixel.hitModule->parameters();
			Acts::Vector2D local(par[Acts::ParDef::eLOC_0], par[Acts::ParDef::eLOC_1]);
			Acts::Vector3D pos, mom;
			pixel.hitModule->referenceSurface().localToGlobal(par, mom, pos);	
			
			ofs << pos(0) << " " << pos(1) << " " << pos(2) << "\n";		
		}
	
	for(auto& sct : sctSP)
		for(auto& hit : sct.hitModuleFront)
			for(auto& vertex : hit->truthVertices())
			{
				Acts::Vector3D trueHit, resolvedHit;
				trueHit = vertex.position();
				resolvedHit = sct.spacePoint;
				ofs << "SCT " << trueHit(0) << " " << trueHit(1) << " " << trueHit(2) << " " << resolvedHit(0) << " " << resolvedHit(1) << " " << resolvedHit(2) << " ";
				
				Acts::Vector3D pos = {0, 0, 0};
				
				for(auto& hit2 : sct.hitModuleFront)
				{
					auto par = hit2->parameters();
					Acts::Vector2D local(par[Acts::ParDef::eLOC_0], par[Acts::ParDef::eLOC_1]);
					Acts::Vector3D pos2, mom;
					hit2->referenceSurface().localToGlobal(par, mom, pos2);
					pos += pos2;
				}
					
				pos /= sct.hitModuleFront.size();
				
				ofs << pos(0) << " " << pos(1) << " " << pos(2) << "\n";	
			}	
	ofs.close();
}
  
void
FW::StripSpacePointBuilder::postProcess(DetData& clusters, const DetData*& detData) const
{
	std::vector<Acts::SingleHitSpacePoint> pixelSP;
	std::vector<Acts::DoubleHitSpacePoint> sctSP;
	std::vector<Acts::Vector3D> digSP;

	for (auto& volumeData : *detData)
		for (auto& layerData : volumeData.second) {
			std::cout << "//////////////////////////////// (" << volumeData.first << ", " << layerData.first << ") ////////////////////////////////////" << std::endl;
			std::vector<Acts::PlanarModuleCluster const*> pixelHits;
			std::vector<std::vector<Acts::PlanarModuleCluster const*>> sctHitsLayerFront, sctHitsLayerBack;
			for (auto& moduleData : layerData.second)
				for(auto& hit : moduleData.second)
				{
						auto par = hit.parameters();
						Acts::Vector2D local(par[Acts::ParDef::eLOC_0], par[Acts::ParDef::eLOC_1]);
						Acts::Vector3D pos, mom;
						hit.referenceSurface().localToGlobal(par, mom, pos);
						Acts::Vector3D post = hit.truthVertices()[0].position();
						std::cout << "(" << pos(0) << ", " << pos(1) << ", " << pos(2) << ")";
						if(sqrt(pos(0) * pos(0) + pos(1) * pos(1)) < 149.6)
						{
							pixelHits.push_back(&hit);
							std::cout << " = Pixel (" << moduleData.first << ")";
							std::cout << " | True position: (" <<  post(0) << ", " << post(1) << ", " << post(2) << ")";
						}
						else
						{
							if(moduleData.first % 2 == 0)
							{
								if(sctHitsLayerFront.size() < moduleData.first + 1)
									sctHitsLayerFront.resize(moduleData.first + 1);
								sctHitsLayerFront[moduleData.first].push_back(&hit);
							}
							else
							{		
								if(sctHitsLayerBack.size() < moduleData.first + 1)
									sctHitsLayerBack.resize(moduleData.first + 1);
								sctHitsLayerBack[moduleData.first].push_back(&hit);
							}
							std::cout << " = SCT (" << moduleData.first << ")";
							std::cout << " | True position: (" <<  post(0) << ", " << post(1) << ", " << post(2) << ")";
						}
						
						  auto& sur     = hit.referenceSurface();
						  auto  segment = dynamic_cast<const Acts::CartesianSegmentation*>(
						      &(sur.associatedDetectorElement()->digitizationModule()->segmentation()));
						  auto binData = segment->binUtility().binningData();
						  size_t binX = binData[0].searchLocal(local);
						  size_t binY = binData[1].searchLocal(local);
						  std::cout << " | Bin: (" << binX << ", " << binY << ")" << std::endl;
				}
			if(!pixelHits.empty())
				Acts::SPB::addHits<Acts::SingleHitSpacePoint>(pixelSP, pixelHits);
			std::cout << "Pixel Cluster: " << pixelSP.size() << std::endl;
			
			if(!sctHitsLayerFront.empty() && !sctHitsLayerBack.empty())
			{
				for(int i = 0; i < sctHitsLayerFront.size(); i++)
				{
					if(sctHitsLayerFront[i].empty())
						continue;
					for(int j = i - 1; j < i; j++)
						if(j < sctHitsLayerBack.size() && j >= 0)
							Acts::SPB::addHits<Acts::DoubleHitSpacePoint, Acts::DoubleHitSpacePointConfig>(sctSP, sctHitsLayerFront[i], sctHitsLayerBack[j]);
				}
			}
			std::cout << "SCT Cluster: " << sctSP.size() << std::endl;
		}
	Acts::SPB::calculateSpacePoints<Acts::SingleHitSpacePoint>(pixelSP);
	
	std::shared_ptr<Acts::DoubleHitSpacePointConfig> dhcfg(new Acts::DoubleHitSpacePointConfig);
	dhcfg->stripLengthTolerance = 0.01;
	dhcfg->stripLengthGapTolerance = 2.09;
	dhcfg->usePerpProj = true;
	Acts::SPB::calculateSpacePoints<Acts::DoubleHitSpacePoint, Acts::DoubleHitSpacePointConfig>(sctSP, dhcfg);
	
	for(auto& pixel : pixelSP)
		storeSpacePoint(pixel, clusters);
	for(auto& sct : sctSP)
		storeSpacePoint(sct, clusters);
	
	writeData(pixelSP, sctSP);
}

FW::ProcessCode
FW::StripSpacePointBuilder::execute(AlgorithmContext ctx) const
{
  ACTS_DEBUG("::execute() called for event " << ctx.eventNumber);

  // DetData is typename of DetectorData<geo_id_value,
  // Acts::PlanarModuleCluster>
  const DetData* detData;

  // Receive all hits from the Whiteboard
  clusterReading(ctx, detData);
  
  DetData clusters;
  postProcess(clusters, detData);

  // Write to Whiteboard
  if (ctx.eventStore.add(m_cfg.collectionOut, std::move(clusters))
      != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}

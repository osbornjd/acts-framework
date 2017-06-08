#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"
#include <iostream>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/ParameterDefinitions.hpp"
#include "ACTS/Digitization/PlanarModuleStepper.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTS/Detector/DetectorElementBase.hpp"
#include "ACTS/Surfaces/Surface.hpp"

FWA::DigitizationAlgorithm::DigitizationAlgorithm(
    const FWA::DigitizationAlgorithm::Config& cfg,
    std::unique_ptr<const Acts::Logger>       logger)
  : FW::Algorithm(cfg, std::move(logger)), m_cfg(cfg)
{
}

FWA::DigitizationAlgorithm::~DigitizationAlgorithm()
{
}

FW::ProcessCode
FWA::DigitizationAlgorithm::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::DigitizationAlgorithm::execute(const FW::AlgorithmContext context) const
{
  // we read from a collection
  if (m_cfg.planarModuleStepper && m_cfg.clustersCollection != ""
      && m_cfg.simulatedHitsCollection != "") {
    // Retrieve relevant information from the execution context
    auto eventStore = context.eventContext->eventStore;

    ACTS_DEBUG("Retrieved event store from algorithm context.");

    // prepare the input data
    FW::DetectorData<geo_id_value,
                     std::pair<std::unique_ptr<const Acts::TrackParameters>,
                               barcode_type>>* hitData
        = nullptr;
    // read and go
    if (eventStore
        && eventStore->readT(hitData, m_cfg.simulatedHitsCollection)
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;

    ACTS_DEBUG("Retrieved hit data '" << m_cfg.simulatedHitsCollection << "' from event store.");

    // prepare the output data: Clusters
    FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>* planarClusters
        = new FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>;

    // perpare the second output data : SpacePoints
    FW::DetectorData<geo_id_value, Acts::Vector3D>* spacePoints
      = new FW::DetectorData<geo_id_value, Acts::Vector3D>;

    // now digitise
    for (auto& vData : (*hitData)) {
      auto volumeKey = vData.first;
      ACTS_DEBUG("- Processing Volume Data collection for volume with ID "
                 << volumeKey);
      for (auto& lData : vData.second) {
        auto layerKey = lData.first;
        ACTS_DEBUG("-- Processing Layer Data collection for layer with ID "
                   << layerKey);
        for (auto& sData : lData.second) {
          auto moduleKey = sData.first;
          ACTS_DEBUG("-- Processing Module Data collection for module with ID "
                     << moduleKey);
          // get the hit parameters
          for (auto& hit : sData.second) {
            auto hitParameters   = hit.first.get();
            auto particleBarcode = hit.second;
            // get the surface
            const Acts::Surface& hitSurface = hitParameters->referenceSurface();
            // get the DetectorElement
            auto hitDetElement = hitSurface.associatedDetectorElement();
            if (hitDetElement) {
              // get the digitization module
              auto hitDigitizationModule = hitDetElement->digitizationModule();
              if (hitDigitizationModule) {
                // parameters
                auto           pars = hitParameters->parameters();
                Acts::Vector2D localIntersection(pars[Acts::ParDef::eLOC_0],
                                                 pars[Acts::ParDef::eLOC_1]);
                Acts::Vector3D localDirection(
                    hitSurface.transform().inverse().linear()
                    * hitParameters->momentum());
                // position
                std::vector<Acts::DigitizationStep> dSteps = m_cfg.planarModuleStepper
                    ->cellSteps(*hitDigitizationModule,
                                localIntersection,
                                localDirection.unit());
                // everything under threshold or edge effects
                if (!dSteps.size()) continue;
                /// let' create a cluster - centroid method
                double   localX    = 0.;
                double   localY    = 0.;
                double   totalPath = 0.;
                // the cells to be used
                std::vector<Acts::DigitizationCell> usedCells;
                usedCells.reserve(dSteps.size());
                // loop over the steps
                for (auto dStep : dSteps){
                  // @todo implement smearing
                  localX        += dStep.stepLength * dStep.stepCellCenter.x();
                  localY        += dStep.stepLength * dStep.stepCellCenter.y();
                  totalPath     += dStep.stepLength;
                  usedCells.push_back(std::move(Acts::DigitizationCell(
                                                      dStep.stepCell.channel0,
                                                      dStep.stepCell.channel1,
                                                      dStep.stepLength)));
                }                                
                // divide by the total path                                 
                localX /= totalPath;
                localY /= totalPath;
                
                // get the segmentation & find the corresponding cell id
                const Acts::Segmentation& segmentation = hitDigitizationModule->segmentation();
                auto binUtility   = segmentation.binUtility();
                Acts::Vector2D localPosition(localX,localY); 
                // @todo remove unneccesary conversion
                size_t bin0 = binUtility.bin(localPosition, 0);
                size_t bin1 = binUtility.bin(localPosition, 1);
                size_t binSerialized = binUtility.serialize({bin0, bin1, 0});
                
                
                // the covariance is currently set to 0.
                Acts::ActsSymMatrixD<2> cov;
                cov << 0., 0., 0., 0.; 
                
                // create the indetifier
                Acts::GeometryID geoID(0);
                geoID.add(volumeKey, Acts::GeometryID::volume_mask);
                geoID.add(layerKey, Acts::GeometryID::layer_mask);
                geoID.add(moduleKey, Acts::GeometryID::sensitive_mask);
                geoID.add(binSerialized, Acts::GeometryID::channel_mask);
                
                // create the planar cluster
                Acts::PlanarModuleCluster pCluster(hitSurface,
                                                   Identifier(geoID.value()), 
                                                   std::move(cov),
                                                   localX, localY,
                                                   std::move(usedCells),
                                                   {particleBarcode});
                
                // insert into the space point map
                FW::Data::insert(*spacePoints,  volumeKey, layerKey, moduleKey, hitParameters->position());
                
                // insert into the cluster map
                FW::Data::insert(*planarClusters, volumeKey, layerKey, moduleKey, std::move(pCluster) );
                                                   
              } // hit moulde proection
            } // hit element protection
          } // hit loop 
        } // moudle loop 
      } // layer loop 
    } // volume loop
  
    // write the SpacePoints to the EventStore
    if (eventStore
        && spacePoints
        && eventStore->writeT(spacePoints, m_cfg.spacePointCollection)
            == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not write collection " << m_cfg.spacePointCollection << " to event store.");
      return FW::ProcessCode::ABORT;
    } 

    // write the clusters to the EventStore
    if (eventStore
        && planarClusters
        && eventStore->writeT(planarClusters, m_cfg.clustersCollection)
            == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not write collection " << m_cfg.clustersCollection << " to event store.");
      return FW::ProcessCode::ABORT;
    } 
  
  } // module stepper exists
  // return SUCCESS to the frameword
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::DigitizationAlgorithm::finalize()
{
  ACTS_VERBOSE("finalize successful.");
  return FW::ProcessCode::SUCCESS;
}


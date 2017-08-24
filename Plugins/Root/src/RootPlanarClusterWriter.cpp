#include <fstream>
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTFW/Plugins/Root/RootPlanarClusterWriter.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"

FW::Root::RootPlanarClusterWriter::RootPlanarClusterWriter(
    const FW::Root::RootPlanarClusterWriter::Config& cfg,
    Acts::Logging::Level                           level)
  : Base(cfg.collection, "RootPlanarClusterWriter", level)
  , m_cfg(cfg)
{}


FW::ProcessCode
FW::Root::RootPlanarClusterWriter::initialize()
{
  m_outputFile = TFile::Open(m_cfg.filePath.c_str(), m_cfg.fileMode.c_str());
  if (!m_outputFile) {
    ACTS_ERROR("Could not open ROOT file'" << m_cfg.filePath << "' to write");
    return ProcessCode::ABORT;
  }
  m_outputFile->cd();
  m_outputTree = new TTree(m_cfg.treeName.c_str(), 
                           "TTree from RootPlanarClusterWriter");
  // set the branches
  m_outputTree->Branch("event_nr",      &m_eventNr);    
  m_outputTree->Branch("volumeID",      &m_volumeID);   
  m_outputTree->Branch("layerID",       &m_layerID);    
  m_outputTree->Branch("surfaceID",     &m_surfaceID);  
  m_outputTree->Branch("g_x",           &m_x);          
  m_outputTree->Branch("g_y",           &m_y);          
  m_outputTree->Branch("g_z",           &m_z);          
  m_outputTree->Branch("l_x",           &m_lx);         
  m_outputTree->Branch("l_y",           &m_lx);         
  m_outputTree->Branch("cov_l_x",       &m_cov_lx);      
  m_outputTree->Branch("cov_l_y",       &m_cov_ly);      
  m_outputTree->Branch("cell_ID_x",     &m_cell_IDx);    
  m_outputTree->Branch("cell_ID_y",     &m_cell_IDy);    
  m_outputTree->Branch("cell_l_x",      &m_cell_lx);     
  m_outputTree->Branch("cell_l_y",      &m_cell_ly);     
  m_outputTree->Branch("cell_data",     &m_cell_data);
  m_outputTree->Branch("truth_g_x",     &m_t_gx);        
  m_outputTree->Branch("truth_g_y",     &m_t_gy);        
  m_outputTree->Branch("truth_g_z",     &m_t_gz);        
  m_outputTree->Branch("truth_l_x",     &m_t_lx);        
  m_outputTree->Branch("truth_l_y",     &m_t_ly);        
  m_outputTree->Branch("truth_barcode", &m_t_barcode); 
  
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::RootPlanarClusterWriter::finalize()
{
  if (m_outputFile) {
    m_outputFile->cd();
    m_outputTree->Write();
    m_outputFile->Close();
    ACTS_INFO("Wrote particles to tree '" << m_cfg.treeName << "' in '"
                                          << m_cfg.filePath << "'");
  }
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::RootPlanarClusterWriter::writeT(
    const AlgorithmContext&                                          ctx,
    const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>& clusters)
{
  // exclusive access to the tree
  std::lock_guard<std::mutex> lock(m_writeMutex);
  
  // loop over the planar clusters in this event
  for (auto& volumeData : clusters) {
    for (auto& layerData : volumeData.second) {
      for (auto& moduleData : layerData.second) {
        for (auto& cluster : moduleData.second) {
          // local cluster information: position, @todo coveraiance
          auto           parameters = cluster.parameters();
          Acts::Vector2D local(parameters[Acts::ParDef::eLOC_0],
                               parameters[Acts::ParDef::eLOC_1]);
                               
          /// prepare for calculating the                      
          Acts::Vector3D pos(0, 0, 0);
          Acts::Vector3D mom(1, 1, 1);
          // the cluster surface
          auto& clusterSurface = cluster.referenceSurface();
          // transform local into global position information
          clusterSurface.localToGlobal(local, mom, pos);
          // identification
          m_eventNr   = ctx.eventNumber;
          m_volumeID  = volumeData.first;       
          m_layerID   = layerData.first;     
          m_surfaceID = moduleData.first;     
          m_x         = pos.x();              
          m_y         = pos.y();              
          m_z         = pos.z();              
          m_lx        = local.x();             
          m_ly        = local.y();             
          m_cov_lx     = 0.; // @todo fill in         
          m_cov_ly     = 0.; // @todo fill in   
          // get the cells and run through them
          const auto& cells = cluster.digitizationCells();
          auto detectorElement = clusterSurface.associatedDetectorElement();
          for (auto& cell : cells) {
            // cell identification
            m_cell_IDx.push_back(cell.channel0);        
            m_cell_IDy.push_back(cell.channel1);         
            m_cell_data.push_back(cell.data);
            // for more we need the digitization module
            if (detectorElement && detectorElement->digitizationModule()){
              auto digitationModule = detectorElement->digitizationModule();
              const Acts::Segmentation& segmentation = digitationModule->segmentation();
              // get the cell positions
              auto cellLocalPosition = segmentation.cellPosition(cell);
              m_cell_lx.push_back(cellLocalPosition.x());
              m_cell_ly.push_back(cellLocalPosition.y());
            }
          }
          // the truth parameters - @todo update to new truth structure   
          // m_g_t;            
          // m_gty;            
          // m_gtz;            
          // m_g_lx;            
          // m_g_ly;            
          // m_barcode;        
          // each hit can have multiple particles, e.g. in a dense environment
          for (auto& barcode : cluster.barcodes()) {
            m_t_barcode.push_back(barcode);
          }
          // one per cluster
          m_outputTree->Fill();
          // now reset 
          m_cell_IDx.clear();        
          m_cell_IDy.clear();        
          m_cell_lx.clear();         
          m_cell_ly.clear();         
          m_cell_data.clear();     
          m_t_gx.clear();            
          m_t_gy.clear();            
          m_t_gz.clear();            
          m_t_lx.clear();            
          m_t_ly.clear();            
          m_t_barcode.clear();        
        }
      }
    }
  }
  return FW::ProcessCode::SUCCESS;
}



/// @file
/// @date 2017-08-07 Rewrite with new interfaces
/// @autor Andreas Salzburger based on the Csv writer 
/// @author Moritz Kiehnn <msmk@cern.ch>

#ifndef ACTFW_ROOTPLANARCLUSTERWRITER_H
#define ACTFW_ROOTPLANARCLUSTERWRITER_H

#include <ACTS/Digitization/PlanarModuleCluster.hpp>
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "TFile.h"
#include "TTree.h"

namespace FW {
  
namespace Root {

  /// @class RootPlanarClusterWriter
  ///
  /// Write out a planar cluster collection into a root file 
  /// to avoid immense long vectors, each cluster is one entry 
  /// in the root file
  class RootPlanarClusterWriter
    : public WriterT<DetectorData<geo_id_value, Acts::PlanarModuleCluster>>
  {
  public:
    using Base = WriterT<DetectorData<geo_id_value, Acts::PlanarModuleCluster>>;
    struct Config
    {
      std::string collection;              ///< particle collection to write
      std::string filePath;                ///< path of the output file
      std::string fileMode = "RECREATE";   ///< file access mode
      std::string treeName = "clusters";  ///< name of the output tree
    };

    /// Constructor with
    /// @param cfg configuration struct 
    /// @param output logging level
    RootPlanarClusterWriter(const Config&        cfg,
                           Acts::Logging::Level level = Acts::Logging::INFO);
    /// default destructor                       
    ~RootPlanarClusterWriter() = default;

    ProcessCode
    initialize() final;

    ProcessCode
    finalize() final;


  protected:
    /// This implementation holds the actual writing method
    /// and is called by the WriterT<>::write interface
    ProcessCode
    writeT(const AlgorithmContext& ctx,
           const DetectorData<geo_id_value, Acts::PlanarModuleCluster>&
               clusters) final;

  private:
    Config             m_cfg;               ///< the configuration object
    std::mutex         m_writeMutex;        ///< protect multi-threaded writes
    TFile*             m_outputFile;        ///< the output file 
    TTree*             m_outputTree;        ///< the output tree 
    int                m_eventNr;           ///< the event number of
    int                m_volumeID;          ///< volume identifier
    int                m_layerID;           ///< layer identifier
    int                m_surfaceID;         ///< surface identifier
    float              m_x;                 ///< global x
    float              m_y;                 ///< global y
    float              m_z;                 ///< global z
    float              m_lx;                ///< local lx
    float              m_ly;                ///< local ly
    float              m_cov_lx;            ///< local covariance lx
    float              m_cov_ly;            ///< local covariance ly
    std::vector<int>   m_cell_IDx;          ///< cell ID in lx
    std::vector<int>   m_cell_IDy;          ///< cell ID in ly
    std::vector<float> m_cell_lx;           ///< local cell position x
    std::vector<float> m_cell_ly;           ///< local cell position y
    std::vector<float> m_cell_data;         ///< local cell position y
    
    // optional the truth position          
    std::vector<float> m_t_gx;              ///< truth position global x
    std::vector<float> m_t_gy;              ///< truth position global y
    std::vector<float> m_t_gz;              ///< truth position global z
    std::vector<float> m_t_lx;              ///< truth position local x
    std::vector<float> m_t_ly;              ///< truth position local y
    std::vector<int>   m_t_barcode;         ///< associated truth particle barcode 
    
    
  };

}  // namespace Root
}  // namespace FW

#endif  // ACTFW_CSVPLANARCLUSTERWRITER_H

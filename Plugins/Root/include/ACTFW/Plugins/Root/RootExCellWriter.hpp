//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef ACTFW_PLUGINS_ROOTEXCELLWRITER_H
#define ACTFW_PLUGINS_ROOTEXCELLWRITER_H 1

#include <mutex>
#include <TTree.h>
#include <TFile.h>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

#ifndef MAXSTEPS
#define MAXSTEPS 100
#endif

namespace FW {

namespace Root {

/// @class ExtrapolationCellWriter
///
/// A root based implementation to write out extrapolation steps.
///
/// Safe to use from multiple writer threads.
///
template <class T> class RootExCellWriter
  : public FW::WriterT< std::vector< Acts::ExtrapolationCell<T> > >
{
public:
  
  using Base = FW::WriterT< std::vector< Acts::ExtrapolationCell<T> > >;
  
  ///  @struct ExtrapolationStep
  ///  this holds the information to be written out
  struct ExtrapolationStep
  {
    float x, y, z;     ///< position (global)
    float px, py, pz;  ///< momentum
    float type;        ///< type of the step
  };

  // @struct Config
  //
  // The nested config class
  struct Config
  {
  public:
    std::string collection;                        ///< particle collection to write
    std::string filePath;                          ///< path of the output file
    std::string fileMode = "RECREATE";             ///< file access mode
    std::string treeName = "extrapolation_cells";  ///< name of the output tree
    bool writeSensitive;
    bool writeMaterial;
    bool writePassive;
    bool writeBoundary;

  };

  /// Constructor
  /// @param cfg is the configuration class
  RootExCellWriter(const Config& cfg,
                   Acts::Logging::Level level = Acts::Logging::INFO);

  /// End-of-run hook
  ProcessCode
  endRun() final;

protected:
  /// The protected writeT method, called by the WriterT base
  /// @param [in] ctx is the algorithm context for event consistency
  /// @param [in] ecells are the celss to be written out 
  ProcessCode
  writeT(const FW::AlgorithmContext&  ctx,
         const std::vector< Acts::ExtrapolationCell<T> > & ecells) final;
  
  Config             m_cfg;               ///< the config class
  std::mutex         m_writeMutex;        ///< protect multi-threaded writes
  TFile*             m_outputFile;        ///< the output file 
  TTree*             m_outputTree;        ///< the output tree 
  float              m_eta;               ///< global eta start
  float              m_phi;               ///< global phi start
  float              m_materialX0;        ///< material in X0
  float              m_materialL0;        ///< material in L0
  std::vector<float> m_s_positionX;       ///< global position x of the step
  std::vector<float> m_s_positionY;       ///< global position y of the step
  std::vector<float> m_s_positionZ;       ///< global position z of the step
  std::vector<float> m_s_positionR;       ///< global position z of the step
  std::vector<float> m_s_materialX0;      ///< step material X0
  std::vector<float> m_s_materialL0;      ///< step material L0
  std::vector<int>   m_s_material;        ///< type of the step: material
  std::vector<int>   m_s_boundary;        ///< type of the step: boundary
  std::vector<int>   m_s_sensitive;       ///< type of the step: sensitive
  std::vector<int>   m_s_volumeID;        ///< volume identification
  std::vector<int>   m_s_layerID;         ///< layer identification
  std::vector<int>   m_s_surfaceID;       ///< surface identification
  std::vector<float> m_s_localposition0;  ///< local position - first coordinate
  std::vector<float> m_s_localposition1;  ///< local position - second coordinate
  int                m_hits;              ///< number of hits in sensitive material

};

} // namespace Root
} // namespace FW

#include "RootExCellWriter.ipp"

#endif  // ACTFW_PLUGINS_ROOTEXCELLWRITER_H

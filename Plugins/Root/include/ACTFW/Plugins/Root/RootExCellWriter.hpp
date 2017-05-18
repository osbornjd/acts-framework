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
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

#ifndef MAXSTEPS
#define MAXSTEPS 100
#endif

namespace FWRoot {
/// @class ExtrapolationCellWriter
///
/// A root based implementation to write out extrapolation steps.
///
/// Safe to use from multiple writer threads.
///
template <class T> class RootExCellWriter 
  : public FW::IWriterT<Acts::ExtrapolationCell<T> > 
{
public:
  ///  @struct ExtrapolationStep
  ///  this holds the information to be written out
  struct ExtrapolationStep
  {
    float x, y, z;     ///< position (global)
    float px, py, pz;  ///< momentum
    float type;        ///< type of the step
  };

  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    std::shared_ptr<Acts::Logger> logger;                ///< the default logger
    std::string                   treeName;              ///< the name of the output tree
    std::string                   fileName;              ///< the name of the output file
    std::string                   name;                  ///< the name of the algorithm
    std::string                   fileMode = "recreate"; //!< recreating the file

    bool writeSensitive;
    bool writeMaterial;
    bool writePassive;
    bool writeBoundary;

    Config(const std::string&   lname = "RootExCellWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , treeName("TTree")
      , fileName("TFile.root")
      , name(lname)
      , writeSensitive(true)
      , writeMaterial(true)
      , writePassive(true)
      , writeBoundary(true)
    {
    }
  };

  /// Constructor
  /// @param cfg is the configuration class
  RootExCellWriter(const Config& cfg);

  /// Destructor
  virtual ~RootExCellWriter();

  /// Framework intialize method
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  initialize() override final;

  /// Framework finalize mehtod
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  finalize() override final;

  /// The write interface
  /// @param eCell is the extrapolation cell that is parsed and written
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  write(const Acts::ExtrapolationCell<T>& eCell) override final;

  /// write a bit of string
  /// @param sinfo is some string info to be written
  /// @return is a ProcessCode indicating return/failure
  FW::ProcessCode
  write(const std::string& sinfo) override final;

  /// Framework name() method
  /// @return name of the tool 
  const std::string&
  name() const override final;

private:
  Config             m_cfg;               ///< the config class

  std::mutex         m_write_mutex;       ///< mutex used to protect multi-threaded writes
    
  TFile*             m_outputFile;        ///< the output file name
  
  // this is the steering tree
  TTree*             m_steeringTree;
    
  // this is the main tree for outputting
  TTree*             m_outputTree;        ///< the output tree name
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

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

template <class T>
const std::string&
RootExCellWriter<T>::name() const
{
  return m_cfg.name;
}

}

#include "RootExCellWriter.ipp"

#endif  // ACTFW_PLUGINS_ROOTEXCELLWRITER_H

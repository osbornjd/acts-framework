//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_PLUGINS_ROOT_ROOTPARTICLEWRITER_H
#define ACTFW_PLUGINS_ROOT_ROOTPARTICLEWRITER_H

#include <mutex>

#include <TTree.h>

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

namespace FWRoot {

/// @class RootParticleWriter
///
/// A root based implementation to write out particleproperties vector
///
class RootParticleWriter 
  : public FW::IWriterT< std::vector<Acts::ParticleProperties> >
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    std::shared_ptr<Acts::Logger> logger;    ///< the default logger
    std::string                   treeName;  ///< the name of the output tree
    std::string                   fileName;  ///< the name of the output file
    std::string                   fileMode;   
    std::string                   name;      ///< the name of the algorithm
    
    std::shared_ptr<FW::BarcodeSvc>
    barcodeSvc;  ///< the barcode service to decode

    /// Constructor
    Config(const std::string&   lname = "RootParticleWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , treeName("TTree")
      , fileName("TFile.root")
      , fileMode("recreate")
      , name(lname)
      , barcodeSvc(nullptr)
    {
    }
    
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  RootParticleWriter(const Config& cfg);

  /// Destructor
  virtual ~RootParticleWriter();

  /// Framework intialize method
  /// @return a ProcessCode to indicate success/failure
  FW::ProcessCode
  initialize() override final;

  /// Framework finalize mehtod
  /// @return a ProcessCode to indicate success/failure
  FW::ProcessCode
  finalize() override final;

  /// The write method
  /// @param ppBegin is the begin iterator of the container
  /// @param ppEnd is the end iterator of the container
  /// @return a ProcessCode to indicate success/failure
  FW::ProcessCode
  write(const std::vector<Acts::ParticleProperties>& particles) override final;

  /// write a bit of string
  /// @param sinfo is some string info to be written
  /// @return is a ProcessCode indicating return/failure
  FW::ProcessCode
  write(const std::string& sinfo) override final;
  
  /// Framework name() method
  // @return the name of the tool      
  const std::string&
  name() const final;

private:
  Config m_cfg;  ///< the config class

  std::mutex m_write_mutex;  ///< mutex used to protect multi-threaded writes

  TFile* m_outputFile;  ///< the output file name

  // this is the main tree for outputting
  TTree*             m_outputTree;  ///< the output tree name
  std::vector<float> m_vx;
  std::vector<float> m_vy;
  std::vector<float> m_vz;
  std::vector<float> m_px;
  std::vector<float> m_py;
  std::vector<float> m_pz;
  std::vector<float> m_pT;
  std::vector<float> m_eta;
  std::vector<float> m_phi;
  std::vector<float> m_mass;
  std::vector<int>   m_charge;
  std::vector<int>   m_pdgCode;
  std::vector<int>   m_barcode;
  std::vector<int>   m_vertex;
  std::vector<int>   m_primary;
  std::vector<int>   m_generation;
  std::vector<int>   m_secondary;
  std::vector<int>   m_process;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

const std::string&
RootParticleWriter::name() const
{
  return m_cfg.name;
}

}

#endif  // ACTFW_PLUGINS_ROOT_ROOTPARTICLEWRITER_H

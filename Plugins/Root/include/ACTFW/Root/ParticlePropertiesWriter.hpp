//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H
#define ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H 1

#include <mutex>

#include <TTree.h>

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Writers/IParticlePropertiesWriter.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

namespace FWRoot {

/// @class ParticlePropertiesWriter
///
/// A root based implementation to write out particleproperties vector
///
class ParticlePropertiesWriter : public FW::IParticlePropertiesWriter
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

    Config(const std::string&   lname = "ParticlePropertiesWriter",
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
  ParticlePropertiesWriter(const Config& cfg);

  /// Destructor
  virtual ~ParticlePropertiesWriter();

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// The write interface
  /// @param pProperties is the vector of particle properties
  FW::ProcessCode
  write(const std::vector<Acts::ParticleProperties>& pProperties);

  /// Framework name() method
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
ParticlePropertiesWriter::name() const
{
  return m_cfg.name;
}
}

#endif  // ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H

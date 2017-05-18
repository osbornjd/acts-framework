//  JsonSpacePointWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_JSON_PLUGINS_SPACEPOINTWRITER_H
#define ACTFW_JSON_PLUGINS_SPACEPOINTWRITER_H 1

#include <mutex>

#include <iostream>
#include <fstream>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IEventDataWriterT.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FWJson {

/// @class JsonSpacePointWriter
///
/// A Json based implementation
///
template <class T> class JsonSpacePointWriter : public FW::IEventDataWriterT<T>
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    std::shared_ptr<Acts::Logger>  logger;           ///< the default logger
    std::string                    name;             ///< the name of the algorithm
    size_t                         outputPrecision   = 4;
    std::shared_ptr<std::ofstream> outputStream      = nullptr;

    Config(const std::string&   lname = "JsonSpacePointWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
    {
    }
        
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  JsonSpacePointWriter(const Config& cfg);

  /// Destructor
  virtual ~JsonSpacePointWriter() = default;

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// The write interface
  /// @param eData is the DetectorData to be written out
  FW::ProcessCode
  write(const FW::DetectorData<geo_id_value, T>& eData) final;

  /// Framework name() method
  const std::string&
  name() const final;

private:
  Config   m_cfg; ///< the config class

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

template <class T> const std::string&
JsonSpacePointWriter<T>::name() const
{
  return m_cfg.name;
}

template <class T> JsonSpacePointWriter<T>::JsonSpacePointWriter(
    const FWJson::JsonSpacePointWriter<T>::Config& cfg)
  : FW::IEventDataWriterT<T>()
  , m_cfg(cfg)
{}


template <class T> FW::ProcessCode
JsonSpacePointWriter<T>::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

template <class T> FW::ProcessCode
JsonSpacePointWriter<T>::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

template <class T> FW::ProcessCode
JsonSpacePointWriter<T>::write(const FW::DetectorData<geo_id_value, T>& eData)
{
 if (!m_cfg.outputStream)   return FW::ProcessCode::SUCCESS;
    
  (*(m_cfg.outputStream)) << std::endl;
  (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);
  
  // loop and fill the space point data
  for (auto& volumeData : eData){
    // get the volume id for the naming
    geo_id_value volumeID = volumeData.first;
    // 
    (*(m_cfg.outputStream)) << "{ \"SpacePoints_" << volumeID <<  "\" : [";
    // initialize the virgule
    bool comma = false;
    for (auto& layerData : volumeData.second)
      for (auto& moduleData : layerData.second)
        for (auto& data : moduleData.second){
          // set the virugle correctly
          if (comma) (*(m_cfg.outputStream)) << ", ";
          comma = true;
          // write the space point
          (*(m_cfg.outputStream)) << "[" << data.x() << ", " << data.y() << ", " << data.z() << "]";
        }
        
    (*(m_cfg.outputStream)) << "] }" << std::endl;
  }    
  // return success
  return FW::ProcessCode::SUCCESS;
}

}

#endif  // ACTFW_JSON_PLUGINS_SPACEPOINTWRITER_H

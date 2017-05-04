//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_OBJ_PLUGINS_SPACEPOINTWRITER_H
#define ACTFW_OBJ_PLUGINS_SPACEPOINTWRITER_H

#include <mutex>

#include <iostream>
#include <fstream>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IEventDataWriter.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FWObj {

/// @class ObjSpacePointWriter
///
/// An Obj writer for the geometry
///
template <class T> class ObjSpacePointWriter : public FW::IEventDataWriter<T>
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    std::shared_ptr<Acts::Logger>  logger;                      ///< the default logger
    std::string                    name;                        ///< the name of the algorithm
    double                         outputScalor      = 1.;      ///< output scalor
    size_t                         outputPrecision   = 4;       ///< precision for out
    std::shared_ptr<std::ofstream> outputStream      = nullptr; ///< the output stream

    Config(const std::string&   lname = "ObjSpacePointWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
    {
    }
        
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  ObjSpacePointWriter(const Config& cfg);

  /// Destructor
  virtual ~ObjSpacePointWriter() = default;

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// The write interface
  /// @param eData is the event data to be written out
  FW::ProcessCode
  write(const FW::DetectorData<geo_id_value, T>& eData);

  /// Framework name() method
  const std::string&
  name() const final;

private:
  Config         m_cfg;         ///< the config class

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

template <class T> const std::string&
ObjSpacePointWriter<T>::name() const
{
  return m_cfg.name;
}

template <class T> ObjSpacePointWriter<T>::ObjSpacePointWriter(
    const ObjSpacePointWriter<T>::Config& cfg)
  : FW::IEventDataWriter<T>()
  , m_cfg(cfg)
{}


template <class T> FW::ProcessCode
ObjSpacePointWriter<T>::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

template <class T> FW::ProcessCode
ObjSpacePointWriter<T>::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

template <class T> FW::ProcessCode
ObjSpacePointWriter<T>::write(const FW::DetectorData<geo_id_value, T>& eData)
{
 if (!m_cfg.outputStream)   return FW::ProcessCode::SUCCESS;
 (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);
 // count the vertex
 size_t vertex = 0;
 // loop and fill the space point data
 for (auto& volumeData : eData){
    // initialize the virgule
    for (auto& layerData : volumeData.second)
      for (auto& moduleData : layerData.second)
        for (auto& data : moduleData.second){
          // write the space point
          (*(m_cfg.outputStream)) << "v " << m_cfg.outputScalor*data.x() 
                                  << ", " << m_cfg.outputScalor*data.y() 
                                  << ", " << m_cfg.outputScalor*data.z() << '\n';
          (*(m_cfg.outputStream)) << "p " << ++vertex <<'\n';
        }
 }    
 // return success
 return FW::ProcessCode::SUCCESS;
}

}

#endif  // ACTFW_OBJ_PLUGINS_SPACEPOINTWRITER_H
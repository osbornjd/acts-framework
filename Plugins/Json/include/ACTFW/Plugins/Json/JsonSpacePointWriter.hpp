//  JsonSpacePointWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_JSON_PLUGINS_SPACEPOINTWRITER_H
#define ACTFW_JSON_PLUGINS_SPACEPOINTWRITER_H 1

#include <fstream>
#include <mutex>

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IEventDataWriterT.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FWJson {

/// @class JsonSpacePointWriter
///
/// A Json based implementation
///
template <class T>
class JsonSpacePointWriter : public FW::IEventDataWriterT<T>
{
public:
  struct Config
  {
    std::string outputPath;
    size_t      outputPrecision = 4;
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  JsonSpacePointWriter(const Config&                       cfg,
                       std::unique_ptr<const Acts::Logger> logger
                       = Acts::getDefaultLogger("JsonSpacePointWriter",
                                                Acts::Logging::INFO));
  virtual ~JsonSpacePointWriter() = default;

  /// Framework name() method
  std::string
  name() const final;

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

private:
  Config                              m_cfg;  ///< the config class
  std::unique_ptr<const Acts::Logger> m_logger;
  std::ofstream                       m_file;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

template <class T>
JsonSpacePointWriter<T>::JsonSpacePointWriter(
    const FWJson::JsonSpacePointWriter<T>::Config& cfg,
    std::unique_ptr<const Acts::Logger>            logger)
  : FW::IEventDataWriterT<T>(), m_cfg(cfg), m_logger(std::move(logger))
{
}

template <class T>
std::string
JsonSpacePointWriter<T>::name() const
{
  return "JsonSpacePointWriter";
}

template <class T>
FW::ProcessCode
JsonSpacePointWriter<T>::initialize()
{
  m_file.open(m_cfg.outputPath, std::ofstream::out | std::ofstream::trunc);
  if (!m_file) {
    ACTS_ERROR("Could not open file '" << m_cfg.outputPath << "'");
    return FW::ProcessCode::ABORT;
  }
  return FW::ProcessCode::SUCCESS;
}

template <class T>
FW::ProcessCode
JsonSpacePointWriter<T>::finalize()
{
  m_file.close();
  return FW::ProcessCode::SUCCESS;
}

template <class T>
FW::ProcessCode
JsonSpacePointWriter<T>::write(const FW::DetectorData<geo_id_value, T>& eData)
{
  std::ostream& os = m_file;

  if (!os)
    return FW::ProcessCode::ABORT;

  os << std::endl;
  os << std::setprecision(m_cfg.outputPrecision);

  // loop and fill the space point data
  for (auto& volumeData : eData) {
    // get the volume id for the naming
    geo_id_value volumeID = volumeData.first;
    //
    os << "{ \"SpacePoints_" << volumeID << "\" : [";
    // initialize the virgule
    bool comma = false;
    for (auto& layerData : volumeData.second)
      for (auto& moduleData : layerData.second)
        for (auto& data : moduleData.second) {
          // set the virugle correctly
          if (comma) os << ", ";
          comma = true;
          // write the space point
          os
              << "[" << data.x() << ", " << data.y() << ", " << data.z() << "]";
        }

    os << "] }" << std::endl;
  }
  // return success
  return FW::ProcessCode::SUCCESS;
}

}  // namespace FWJson

#endif  // ACTFW_JSON_PLUGINS_SPACEPOINTWRITER_H

//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_OBJ_PLUGINS_TRACKINGGEOMETRYWRITER_H
#define ACTFW_OBJ_PLUGINS_TRACKINGGEOMETRYWRITER_H 1

#include <mutex>

#include <iostream>
#include <fstream>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/ITrackingGeometryWriter.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace Acts {
  class TrackingGeometry;
  class TrackingVolume;
}


namespace FW {
  class ISurfaceWriter;
}

namespace FWObj {

/// @class ObjTrackingGeometryWriter
///
/// An Obj writer for the geometry
///
class ObjTrackingGeometryWriter : public FW::ITrackingGeometryWriter
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    /// the default logger
    std::shared_ptr<Acts::Logger>                      logger;
    /// the name of the writer
    std::string                                         name;
    /// surfaceWriters 
    std::vector< std::shared_ptr<FW::ISurfaceWriter > > surfaceWriters;
  
    Config(const std::string&   lname = "ObjTrackingGeometryWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
      , surfaceWriters()
    {}
        
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  ObjTrackingGeometryWriter(const Config& cfg);

  /// Destructor
  virtual ~ObjTrackingGeometryWriter();

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// The write interface
  /// @param surface to be written out
  FW::ProcessCode
  write(const Acts::TrackingGeometry& tGeometry);

  /// Framework name() method
  const std::string&
  name() const final;

private:
  Config         m_cfg;         ///< the config class

  /// process this volume
  ///
  /// @param tVolume the volume to be processed
  void
  write(const Acts::TrackingVolume& tVolume);

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

const std::string&
ObjTrackingGeometryWriter::name() const
{
  return m_cfg.name;
}
}

#endif  // ACTFW_OBJ_PLUGINS_TRACKINGGEOMETRYWRITER_H

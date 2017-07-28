//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_OBJ_PLUGINS_TRACKINGGEOMETRYWRITER_H
#define ACTFW_OBJ_PLUGINS_TRACKINGGEOMETRYWRITER_H

#include <mutex>

#include <iostream>
#include <fstream>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"

namespace Acts {
  class TrackingVolume;
}
namespace FW {
  class ISurfaceWriter;
}

namespace FWObj {

/// @class ObjTrackingGeometryWriter
///
/// An Obj writer for the geometry
/// It delegates the writing of surfaces to the surface writers
  class ObjTrackingGeometryWriter : public FW::IWriterT<Acts::TrackingGeometry>
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    /// the default logger
    std::shared_ptr<const Acts::Logger>                          logger;
    /// the name of the writer
    std::string                                                  name = "";
    /// surfaceWriters
    std::vector< std::shared_ptr<FW::IWriterT<Acts::Surface> > > surfaceWriters;
    std::string                                                  filePrefix = "";
    std::string                                                  sensitiveGroupPrefix = "";
    std::string                                                  layerPrefix = "";
  
    Config(const std::string&   lname = "ObjTrackingGeometryWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
      , surfaceWriters()
    {}
  };

  /// Constructor
  /// @param cfg is the configuration class
  ObjTrackingGeometryWriter(const Config& cfg);

  /// Destructor
  virtual ~ObjTrackingGeometryWriter();
  
  /// Framework name() method
  /// @return the name of the tool
  std::string
  name() const final;
  
  /// Framework intialize method
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  finalize() final;

  /// The write interface
  /// @param tGeometry is the geometry to be written out
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  write(const Acts::TrackingGeometry& tGeometry) final override;
  
  /// write a bit of string
  /// @param is the string to be written out
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  write(const std::string& sinfo) final override;

private:
  Config         m_cfg;         ///< the config class

  /// process this volume
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

FW::ProcessCode
ObjTrackingGeometryWriter::write(const std::string&)
{
  return FW::ProcessCode::SUCCESS;
}

}

#endif  // ACTFW_OBJ_PLUGINS_TRACKINGGEOMETRYWRITER_H

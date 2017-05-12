//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_OBJ_PLUGINS_SURFACEWRITER_H
#define ACTFW_OBJ_PLUGINS_SURFACEWRITER_H

#include <mutex>
#include <iostream>
#include <fstream>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTFW/Obj/ObjHelper.hpp"

namespace FWObj {

/// @class ObjSurfaceWriter
///
/// An Obj writer for the geometry
///
class ObjSurfaceWriter : public FW::IWriterT<Acts::Surface>
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    std::shared_ptr<Acts::Logger>  logger;                       ///< the default logger
    std::string                    name;                         ///< the name of the algorithm
    unsigned int                   outputPhiSegemnts  = 72;      ///< approximate cyinders by that
    double                         outputThickness    = 2.;      ///< write thickness if available   
    bool                           outputSensitive    = true;    ///< write sensitive surfaces
    bool                           outputLayerSurface = true;    ///< write the layer surface out
    double                         outputScalor       = 1.;      ///< output scalor
    unsigned int                   outputPrecision    = 6;       ///< precision for out
    std::string                    filePrefix         = "";      ///< file prefix to be written out
    std::string                    planarPrefix       = "";      ///< prefixes
    std::string                    cylinderPrefix     = "";      ///< prefixes
    std::string                    diskPrefix         = "";      ///< prefixes
    std::shared_ptr<std::ofstream> outputStream       = nullptr; ///< the output stream

    Config(const std::string&   lname = "ObjSurfaceWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
    {}
        
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  ObjSurfaceWriter(const Config& cfg);

  /// Destructor
  virtual ~ObjSurfaceWriter();

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// The write interface
  /// @param surface to be written out
  FW::ProcessCode
  write(const Acts::Surface& surface) final override;

  /// write a bit of string
  /// @param is the string to be written
  FW::ProcessCode
  write(const std::string& sinfo) final override;

  /// Framework name() method
  const std::string&
  name() const final;

private:
  Config                    m_cfg;        ///< the config class
  FWObjHelper::VtnCounter   m_vtnCounter; ///< vertex, texture, normal
  std::mutex                m_write_mutex;///< mutex to protect multi-threaded writes
  

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

const std::string&
ObjSurfaceWriter::name() const
{
  return m_cfg.name;
}

FW::ProcessCode
ObjSurfaceWriter::write(const std::string& sinfo)
{
  
  // abort if you don't have a stream
  if (!m_cfg.outputStream)   return FW::ProcessCode::ABORT;
  // lock the mutex for writing
  std::lock_guard<std::mutex> lock(m_write_mutex);
  // and write
  (*m_cfg.outputStream) << sinfo;
  return FW::ProcessCode::SUCCESS;
}

}

#endif  // ACTFW_OBJ_PLUGINS_SURFACEWRITER_H

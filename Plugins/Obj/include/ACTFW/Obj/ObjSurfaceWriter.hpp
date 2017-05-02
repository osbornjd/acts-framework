//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_OBJ_PLUGINS_SURFACEWRITER_H
#define ACTFW_OBJ_PLUGINS_SURFACEWRITER_H 1

#include <mutex>

#include <iostream>
#include <fstream>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/ISurfaceWriter.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace Acts {
  class Surface;
}

namespace FWObj {

/// @class ObjSurfaceWriter
///
/// An Obj writer for the geometry
///
class ObjSurfaceWriter : public FW::ISurfaceWriter
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

    Config(const std::string&   lname = "ObjSurfaceWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
    {
    }
        
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
  write(const Acts::Surface& surface);

  /// Framework name() method
  const std::string&
  name() const final;

private:
  Config         m_cfg;         ///< the config class
  unsigned int   m_nvertices;   ///< written vertices 

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
}

#endif  // ACTFW_OBJ_PLUGINS_SURFACEWRITER_H
///////////////////////////////////////////////////////////////////
// GeometryService.hpp
///////////////////////////////////////////////////////////////////

#ifndef DD4HEP_GEOMETRYSERVICE_H
#define DD4HEP_GEOMETRYSERVICE_H 1

#include <memory>
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/GeometryInterfaces/IDD4hepService.hpp"
#include "ACTFW/GeometryInterfaces/ITGeoService.hpp"
#include "ACTFW/GeometryInterfaces/ITrackingGeometryService.hpp"
#include "ACTS/Utilities/BinningType.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "DD4hep/LCDD.h"
#include "TGeoNode.h"

namespace DD4hepPlugin {

/// @class GeometryService
///
/// @brief service creating geometries from dd4hep input
///
/// The GeometryService creates the DD4hep, the TGeo and the ACTS
/// TrackingGeometry
/// from DD4hep xml input. The geometries are created only on demand.

class GeometryService : public FW::IDD4hepService,
                        public FW::ITGeoService,
                        public FW::ITrackingGeometryService
{
public:
  /// @class Config
  /// nested config file of the GeometryService
  class Config
  {
  public:
    /// The default logger
    std::shared_ptr<Acts::Logger> logger;
    /// XML-file with the detector description
    std::string xmlFileName;
    /// Logger for the geometry transformation
    Acts::Logging::Level lvl;
    /// The name of the service
    std::string name;
    /// Binningtype in phi
    Acts::BinningType bTypePhi;
    /// Binningtype in r
    Acts::BinningType bTypeR;
    /// Binningtype in z
    Acts::BinningType bTypeZ;
    //
    double envelopeR;
    double envelopeZ;

    Config(const std::string&   lname = "GeometryService",
           Acts::Logging::Level level = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, level))
      , xmlFileName("")
      , lvl(level)
      , name(lname)
      , bTypePhi(Acts::equidistant)
      , bTypeR(Acts::equidistant)
      , bTypeZ(Acts::equidistant)
      , envelopeR(0.)
      , envelopeZ(0.)
    {
    }
  };
  /// Constructor
  GeometryService(const Config& cfg);

  /// Virtual destructor
  ~GeometryService();

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// Framework name() method
  const std::string&
  name() const final;

  /// Interface method to access the DD4hep geometry
  /// @return The world DD4hep DetElement
  virtual DD4hep::Geometry::DetElement
  dd4hepGeometry() final;
  /// Interface method to Access the TGeo geometry
  /// @return The world TGeoNode (physical volume)
  virtual TGeoNode*
  tgeoGeometry() final;
  /// Interface method to access to the interface of the DD4hep geometry
  virtual DD4hep::Geometry::LCDD*
  lcdd() final;
  /// Interface method to access the ACTS TrackingGeometry
  virtual std::unique_ptr<const Acts::TrackingGeometry>
  trackingGeometry() final;

private:
  /// Private method to initiate building of the DD4hep geometry
  FW::ProcessCode
  buildDD4hepGeometry();
  /// Private method to initiate building of the ACTS tracking geometry
  FW::ProcessCode
  buildTrackingGeometry();

  /// The config class
  Config m_cfg;
  /// Pointer to the interface to the DD4hep geometry
  DD4hep::Geometry::LCDD* m_lcdd;
  /// The world DD4hep DetElement
  DD4hep::Geometry::DetElement m_dd4hepGeometry;
  /// The ACTS TrackingGeometry
  std::unique_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

inline const std::string&
GeometryService::name() const
{
  return m_cfg.name;
}
}
#endif  // DD4HEP_GEOMETRYSERVICE_H

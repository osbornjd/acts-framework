#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Plugins/DD4hepPlugins/ConvertDD4hepDetector.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/SurfaceArrayCreator.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Utilities/BinningType.hpp"

FW::DD4hep::GeometryService::GeometryService(
    const FW::DD4hep::GeometryService::Config& cfg)
  : m_cfg(cfg)
  , m_lcdd(nullptr)
  , m_dd4hepGeometry(nullptr)
  , m_trackingGeometry(nullptr)
{
}

FW::DD4hep::GeometryService::~GeometryService()
{
  if (m_lcdd) m_lcdd->destroyInstance();
}

std::string
FW::DD4hep::GeometryService::name() const
{
  return m_cfg.name;
}

/** Framework intialize method */
FW::ProcessCode
FW::DD4hep::GeometryService::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

/** Framework finalize mehtod */
FW::ProcessCode
FW::DD4hep::GeometryService::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::DD4hep::GeometryService::buildDD4hepGeometry()
{
  if (!m_cfg.xmlFileName.empty()) {
    m_lcdd = &(DD4hep::Geometry::LCDD::getInstance());
    m_lcdd->fromCompact(m_cfg.xmlFileName.c_str());
    m_lcdd->volumeManager();
    m_lcdd->apply("DD4hepVolumeManager", 0, 0);
    m_dd4hepGeometry = m_lcdd->world();
  }
  return FW::ProcessCode::SUCCESS;
}

DD4hep::Geometry::DetElement
FW::DD4hep::GeometryService::dd4hepGeometry()
{
  if (!m_dd4hepGeometry) buildDD4hepGeometry();
  return m_dd4hepGeometry;
}

DD4hep::Geometry::LCDD*
FW::DD4hep::GeometryService::GeometryService::lcdd()
{
  if (!m_lcdd) buildDD4hepGeometry();
  return m_lcdd;
}

TGeoNode*
FW::DD4hep::GeometryService::tgeoGeometry()
{
  if (!m_dd4hepGeometry) buildDD4hepGeometry();
  return m_dd4hepGeometry.placement().ptr();
}

FW::ProcessCode
FW::DD4hep::GeometryService::buildTrackingGeometry()
{
  // set the tracking geometry
  m_trackingGeometry
      = std::move(Acts::convertDD4hepDetector(dd4hepGeometry(),
                                              m_cfg.lvl,
                                              m_cfg.bTypePhi,
                                              m_cfg.bTypeR,
                                              m_cfg.bTypeZ,
                                              m_cfg.envelopeR,
                                              m_cfg.envelopeZ,
                                              m_cfg.buildDigitizationModules));
  return FW::ProcessCode::SUCCESS;
}

std::unique_ptr<const Acts::TrackingGeometry>
FW::DD4hep::GeometryService::trackingGeometry()
{
  if (!m_trackingGeometry) buildTrackingGeometry();
  return std::move(m_trackingGeometry);
}

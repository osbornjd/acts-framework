#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Plugins/DD4hepPlugins/DD4hepCylinderGeometryBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"

DD4hepPlugin::GeometryService::GeometryService(
    const DD4hepPlugin::GeometryService::Config& cfg)
  : m_cfg(cfg)
  , m_lcdd(nullptr)
  , m_dd4hepGeometry(nullptr)
  , m_trackingGeometry(nullptr)
{
}

DD4hepPlugin::GeometryService::~GeometryService()
{
  if (m_lcdd) m_lcdd->destroyInstance();
}

/** Framework intialize method */
FW::ProcessCode
DD4hepPlugin::GeometryService::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

/** Framework finalize mehtod */
FW::ProcessCode
DD4hepPlugin::GeometryService::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
DD4hepPlugin::GeometryService::buildDD4hepGeometry()
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
DD4hepPlugin::GeometryService::dd4hepGeometry()
{
  if (!m_dd4hepGeometry) buildDD4hepGeometry();
  return m_dd4hepGeometry;
}

DD4hep::Geometry::LCDD*
DD4hepPlugin::GeometryService::GeometryService::lcdd()
{
  if (!m_lcdd) buildDD4hepGeometry();
  return m_lcdd;
}

TGeoNode*
DD4hepPlugin::GeometryService::tgeoGeometry()
{
  if (!m_dd4hepGeometry) buildDD4hepGeometry();
  return m_dd4hepGeometry.placement().ptr();
}

FW::ProcessCode
DD4hepPlugin::GeometryService::buildTrackingGeometry()
{
  // hand over LayerArrayCreator
  auto layerArrayCreator = std::make_shared<Acts::LayerArrayCreator>(
      Acts::getDefaultLogger("LayArrayCreator", m_cfg.lvl));
  // tracking volume array creator
  auto trackingVolumeArrayCreator
      = std::make_shared<Acts::TrackingVolumeArrayCreator>(
          Acts::getDefaultLogger("TrkVolArrayCreator", m_cfg.lvl));
  // configure the cylinder volume helper
  Acts::CylinderVolumeHelper::Config cvhConfig;
  cvhConfig.layerArrayCreator          = layerArrayCreator;
  cvhConfig.trackingVolumeArrayCreator = trackingVolumeArrayCreator;
  auto cylinderVolumeHelper = std::make_shared<Acts::CylinderVolumeHelper>(
      cvhConfig, Acts::getDefaultLogger("CylVolHelper", m_cfg.lvl));
  // configure the volume builder
  Acts::CylinderVolumeBuilder::Config pvbConfig;
  pvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
  pvbConfig.volumeSignature      = 0;
  auto cylinderVolumeBuilder = std::make_shared<Acts::CylinderVolumeBuilder>(
      pvbConfig, Acts::getDefaultLogger("CylVolBuilder", m_cfg.lvl));
  // configure geometry builder with the surface array creator
  Acts::DD4hepCylinderGeometryBuilder::Config cgConfig;
  cgConfig.detWorld      = dd4hepGeometry();
  cgConfig.volumeHelper  = cylinderVolumeHelper;
  cgConfig.volumeBuilder = cylinderVolumeBuilder;
  auto geometryBuilder = std::make_shared<Acts::DD4hepCylinderGeometryBuilder>(
      cgConfig, Acts::getDefaultLogger("DD4hepCylGeoBuilder", m_cfg.lvl));
  // set the tracking geometry
  m_trackingGeometry = std::move(geometryBuilder->trackingGeometry());
  return FW::ProcessCode::SUCCESS;
}

std::unique_ptr<const Acts::TrackingGeometry>
DD4hepPlugin::GeometryService::trackingGeometry()
{
  if (!m_trackingGeometry) buildTrackingGeometry();
  return std::move(m_trackingGeometry);
}

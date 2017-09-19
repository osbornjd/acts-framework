#include "ACTFW/Plugins/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/Plugins/DD4hepG4/GeoConstruction.hpp"

FW::DD4hepG4::DD4hepToG4Svc::DD4hepToG4Svc(const FW::DD4hepG4::DD4hepToG4Svc::Config& cfg) :
m_cfg(cfg),
m_geant4Geometry(nullptr)
{}

FW::DD4hepG4::DD4hepToG4Svc::~DD4hepToG4Svc()
{
    //delete m_geant4Geometry;
}

std::string FW::DD4hepG4::DD4hepToG4Svc::name() const
{
  return m_cfg.name;
}

FW::ProcessCode FW::DD4hepG4::DD4hepToG4Svc::buildGeant4Geometry()
{
    if(m_cfg.dd4hepService->lcdd()) {
        G4VUserDetectorConstruction* detector(new FW::DD4hepG4::GeoConstruction(*(m_cfg.dd4hepService->lcdd())));
        m_geant4Geometry = detector;
    }
 //   if (!m_geant4Geometry) FW::ProcessCode::ERROR;
    return FW::ProcessCode::SUCCESS;
}

G4VUserDetectorConstruction*  FW::DD4hepG4::DD4hepToG4Svc::geant4Geometry()
{
    if (!m_geant4Geometry) buildGeant4Geometry();
    return m_geant4Geometry;
}

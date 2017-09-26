#include "ACTFW/Plugins/DD4hepG4/GeoConstruction.hpp"
#include <stdexcept>
#include "DDG4/Geant4Converter.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Detector.h"

namespace FW {
namespace DD4hepG4 {
    GeoConstruction::GeoConstruction(dd4hep::Detector& lcdd)
    : m_lcdd(lcdd) {}
    
    // method borrowed from DD4hep::Simulation::Geant4DetectorConstruction::Construct()
    G4VPhysicalVolume* GeoConstruction::Construct() {
        dd4hep::sim::Geant4Mapping& g4map = dd4hep::sim::Geant4Mapping::instance();
        dd4hep::DetElement world = m_lcdd.world();
        dd4hep::sim::Geant4Converter conv(m_lcdd, dd4hep::INFO);
        dd4hep::sim::Geant4GeometryInfo* geo_info = conv.create(world).detach();
        g4map.attach(geo_info);
        // All volumes are deleted in ~G4PhysicalVolumeStore()
        G4VPhysicalVolume* m_world = geo_info->world();
        m_lcdd.apply("DD4hepVolumeManager", 0, 0);
        // Create Geant4 volume manager
        g4map.volumeManager();
        return m_world;
    }
  }
}

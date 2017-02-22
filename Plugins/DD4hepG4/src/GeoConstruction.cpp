#include "ACTFW/DD4hepG4/GeoConstruction.hpp"
#include <stdexcept>
// DD4hep
#include "DDG4/Geant4Converter.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/LCDD.h"
// Geant4


namespace DD4hepG4 {
    GeoConstruction::GeoConstruction(DD4hep::Geometry::LCDD& lcdd)
    : m_lcdd(lcdd) {}
    
    GeoConstruction::~GeoConstruction() {}
    
    // method borrowed from DD4hep::Simulation::Geant4DetectorConstruction::Construct()
    G4VPhysicalVolume* GeoConstruction::Construct() {
        DD4hep::Simulation::Geant4Mapping& g4map = DD4hep::Simulation::Geant4Mapping::instance();
        DD4hep::Geometry::DetElement world = m_lcdd.world();
        DD4hep::Simulation::Geant4Converter conv(m_lcdd, DD4hep::INFO);
        DD4hep::Simulation::Geant4GeometryInfo* geo_info = conv.create(world).detach();
        g4map.attach(geo_info);
        // All volumes are deleted in ~G4PhysicalVolumeStore()
        G4VPhysicalVolume* m_world = geo_info->world();
        m_lcdd.apply("DD4hepVolumeManager", 0, 0);
        // Create Geant4 volume manager
        g4map.volumeManager();
        return m_world;
    }
}
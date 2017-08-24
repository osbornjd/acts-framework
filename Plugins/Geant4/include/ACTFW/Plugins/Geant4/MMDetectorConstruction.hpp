#ifndef ACTFW_PLUGINS_GEANT4_MMDETECTORCONSTRUCTION_H
#define ACTFW_PLUGINS_GEANT4_MMDETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

//see examples http://svn.code.sf.net/p/vgm/code/tags/v4-3/examples/E02/include/B4DetectorConstruction.hh

class G4VPhysicalVolume;
class TGeoNode;

namespace FW {
namespace G4 {
    
    /// @class MMDetectorConstruction
    ///
    /// @brief constructing the detector in Geant4
    /// this can be used with GDML and TGeo iput
    ///
    /// @TODO implement it with conversion from TGeo
    
    class MMDetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
      /// Constructor
      MMDetectorConstruction();
      
      /// Destructor 
      virtual ~MMDetectorConstruction();
      
      /// Construct method
      /// @return the world volume as G4VPhysicalVolume
      virtual G4VPhysicalVolume* Construct();
      
      /// Set the world TGeoNode to be transformed into
      /// a Geant4 geometry
      /// @param tgeoNode is the world not in ROOT::TGeo
      void setTGeoGeometry(TGeoNode* tgeoNode); 
      
      /// Set the path and name to the GDML file
      /// @param gdmlFile is the path+name of the GDML file
      void setGdmlInput(std::string gdmlFile);
        
    private:
        
      TGeoNode*       m_tgeoNode;
      std::string*    m_gdmlFile;
    };
} // namespace G4
} // namespace FW

#endif //ACTFW_PLUGINS_GEANT4_MMDETECTORCONSTRUCTION_H

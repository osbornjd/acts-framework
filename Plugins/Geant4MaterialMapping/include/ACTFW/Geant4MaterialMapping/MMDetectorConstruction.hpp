#ifndef GEANT4MATERIALMAPPING_MMDETECTORCONSTRUCTION_H
#define GEANT4MATERIALMAPPING_MMDETECTORCONSTRUCTION_H 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"


//see examples http://svn.code.sf.net/p/vgm/code/tags/v4-3/examples/E02/include/B4DetectorConstruction.hh

class G4VPhysicalVolume;
class TGeoNode;

namespace G4MM {
    
    /// @class MMDetectorConstruction
    ///
    /// @brief constructing the detector in Geant4
    ///
    /// @TODO implement it with conversion from TGeo
    
    class MMDetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        
        MMDetectorConstruction();
        virtual ~MMDetectorConstruction();
        
        virtual G4VPhysicalVolume* Construct();
        
        void setTGeoGeometry(TGeoNode* tgeoNode); //possibility to set also other formats
        void setGdmlInput(std::string gdmlFile);
        
    private:
        
        TGeoNode*       m_tgeoNode;
        std::string*    m_gdmlFile;
    };
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif //GEANT4MATERIALMAPPING_MMDETECTORCONSTRUCTION_H

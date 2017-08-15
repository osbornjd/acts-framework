///////////////////////////////////////////////////////////////////
// MMMaterialStepAction.h
///////////////////////////////////////////////////////////////////


#ifndef ACTFW_PLUGINS_GEANT4_MMSTEPPINGACTION_H
#define ACTFW_PLUGINS_GEANT4_MMSTEPPINGACTION_H

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "ACTS/Plugins/MaterialPlugins/MaterialStep.hpp"
#include <vector>

namespace FW {
namespace G4 {
    
    /// @class MMSteppingAction
    ///
    /// @brief Collects the MaterialStep entities
    ///
    /// The MMSteppingAction class is the implementation of the
    /// Geant4 class SteppingAction. It creates extracts the weighted material
    /// of every step and collects all material steps.
    
    class MMSteppingAction : public G4UserSteppingAction
    {
    public:
        /// Constructor
        MMSteppingAction();
        
        /// Destructor
        virtual ~MMSteppingAction();
        
        /// Static access method
        static MMSteppingAction* Instance();
        
        /// Interface Method doing the step
        /// @note it creates and collects the MaterialStep entities
        /// @param step is teh Geant4 step of the particle  
        virtual void UserSteppingAction(const G4Step* step);
        
        /// Interface reset method
        /// @note it clears the collected step vector
        void Reset();
        
        /// Access to the collected MaterialStep entities
        std::vector<Acts::MaterialStep> materialSteps() {return m_steps;}
        
    private:
        /// Instance of the SteppingAction
        static MMSteppingAction* fgInstance;
        
        /// The collected MaterialStep entities
        std::vector<Acts::MaterialStep> m_steps;
    };

} // namespace G4
} // namespace FW

#endif // ACTFW_PLUGINS_GEANT4_MMSTEPPINGACTION_H


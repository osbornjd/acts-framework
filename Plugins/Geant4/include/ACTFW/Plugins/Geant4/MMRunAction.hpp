///////////////////////////////////////////////////////////////////
// MMRunAction.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_PLUGINS_GEANT4_MMRUNACTION_H
#define ACTFW_PLUGINS_GEANT4_MMRUNACTION_H

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <memory>

class G4Run;

namespace FW {
namespace G4 {
    
    /// @class MMRunAction
    ///
    /// @brief The material mapping run action
    ///
    /// The MMRunAction class is the implementation of the
    /// Geant4 class G4UserRunAction. It initiates the run
    /// an resets the EventAction
    
    class MMRunAction : public G4UserRunAction
    {
    public:
        /// Constructor
        MMRunAction();
        
        /// Destructor
        virtual ~MMRunAction();
        
        /// Static access method
        static MMRunAction* Instance();
        
        /// Interface method at the begin of the run
        /// @note resets the event action
        virtual void BeginOfRunAction(const G4Run*);
        
        /// Interface method at the end of the run
        virtual void   EndOfRunAction(const G4Run*);
        
    private:
        /// Instance of the EventAction
        static MMRunAction* fgInstance;
    };
} // namespace G4
} // namespace FW

#endif // ACTFW_PLUGINS_GEANT4_MMRUNACTION_H

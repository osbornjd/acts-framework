///////////////////////////////////////////////////////////////////
// MMRunAction.h
///////////////////////////////////////////////////////////////////

#ifndef GEANT4MATERIALMAPPING_MMRUNACTION_H
#define GEANT4MATERIALMAPPING_MMRUNACTION_H 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <memory>

class G4Run;

namespace G4MM {
    
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
}

#endif //GEANT4MATERIALMAPPING_MMRUNACTION_H

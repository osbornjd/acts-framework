///////////////////////////////////////////////////////////////////
// MMEventAction.h
///////////////////////////////////////////////////////////////////

#ifndef GEANT4MATERIALMAPPING_MMEVENTACTION_H
#define GEANT4MATERIALMAPPING_MMEVENTACTION_H 1

#include <memory>
#include "ACTFW/Writers/IMaterialTrackRecWriter.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"
#include "G4UserEventAction.hh"
#include "globals.hh"

/// @namespace G4MM Namespace for geant4 material mapping

namespace G4MM {

class MMSteppingAction;

/// @class MMEventAction
///
/// @brief Writes out material track records
///
/// The MMEventAction class is the realization of the Geant4 class
/// G4UserEventAction and is writing out the collected MaterialTrackRecord
/// entities needed for material mapping once per event.
///

class MMEventAction : public G4UserEventAction
{
public:
  /// Constructor
  MMEventAction();
  /// Destructor
  virtual ~MMEventAction();
  /// Static access method
  static MMEventAction*
  Instance();
  /// Interface method for begin of the event
  /// @note resets the material step action
  virtual void
  BeginOfEventAction(const G4Event* event);
  /// Interface method for end of event
  /// @note this method is writing out the material track records
  virtual void
  EndOfEventAction(const G4Event* event);
  /// Interface method
  /// @note does nothing
  void
  Reset();
  // Access the material track records
  std::vector<Acts::MaterialTrackRecord> const
  materialTrackRecords();

private:
  /// Instance of the EventAction
  static MMEventAction* fgInstance;
  /// The MaterialTrackRecWriter
  std::vector<Acts::MaterialTrackRecord> m_records;
};

inline std::vector<Acts::MaterialTrackRecord> const
MMEventAction::materialTrackRecords()
{
  return m_records;
}
}

#endif  // GEANT4MATERIALMAPPING_MMEVENTACTION_H

//  EventContext.h
//  ACTFW
//
//  Created by Hadrien Grasland on 26/10/16.
//
//

#ifndef EVENTCONTEXT_H
#define EVENTCONTEXT_H

#include <memory>

namespace FW {

class WhiteBoard;

/// @class EventContext
///
/// Describes the properties of a test framework event
///
struct EventContext
{
  size_t eventNumber;                            ///< Unique event identifier
  std::shared_ptr<WhiteBoard> eventStore;        ///< Storage for event-specific data
  
  EventContext(size_t p_eventNumber,
               std::shared_ptr<WhiteBoard> p_eventStore)
    : eventNumber{p_eventNumber}
    , eventStore{p_eventStore}
  {
  }
};

}

#endif  // EVENTCONTEXT_H

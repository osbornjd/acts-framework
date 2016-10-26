//  EventContext.h
//  ACTFW
//
//  Created by Hadrien Grasland on 26/10/16.
//
//

#ifndef EVENTCONTEXT_H
#define EVENTCONTEXT_H

#include <memory>

#include "ACTFW/Framework/JobContext.hpp"

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
  std::shared_ptr<const JobContext> jobContext;  ///< Link to the job context
  
  EventContext(size_t p_eventNumber,
               std::shared_ptr<WhiteBoard> p_eventStore,
               std::shared_ptr<const JobContext> p_jobContext)
    : eventNumber{p_eventNumber}
    , eventStore{p_eventStore}
    , jobContext{p_jobContext}
  {
  }
};

}

#endif  // EVENTCONTEXT_H

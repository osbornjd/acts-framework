//  JobContext.h
//  ACTFW
//
//  Created by Hadrien Grasland on 26/10/16.
//
//

#ifndef JOBCONTEXT_H
#define JOBCONTEXT_H

#include <memory>

namespace FW {

class WhiteBoard;

/// @class JobContext
///
/// Describes the properties of a test framework job
///
struct JobContext
{
  size_t eventCount;                     ///< Amount of events being simulated
  std::shared_ptr<WhiteBoard> jobStore;  ///< Storage for job-wide data
  
  JobContext(size_t p_eventCount,
             std::shared_ptr<WhiteBoard> p_jobStore)
    : eventCount{p_eventCount}
    , jobStore{p_jobStore}
  {
  }
};

}

#endif  // JOBCONTEXT_H

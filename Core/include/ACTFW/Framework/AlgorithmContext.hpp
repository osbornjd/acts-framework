/// @file
/// @date 2016-10-26 Initial version
/// @author Hadrien Grasland
/// @author Moritz Kiehn <msmk@cern.ch>

#ifndef ACTFW_ALGORITHMCONTEXT_H
#define ACTFW_ALGORITHMCONTEXT_H

#include <memory>

namespace FW {

class WhiteBoard;

/// Aggregated information to run one algorithm over one event.
struct AlgorithmContext
{
  size_t                      algorithmNumber;  ///< Unique algorithm identifier
  size_t                      eventNumber;      ///< Unique event identifier
  std::shared_ptr<WhiteBoard> eventStore;       ///< Per-event data store
};

}  // namespace FW

#endif  // ACTFW_ALGORITHMCONTEXT_H

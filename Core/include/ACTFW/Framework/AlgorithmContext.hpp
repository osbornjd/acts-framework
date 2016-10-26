//  AlgorithmContext.h
//  ACTFW
//
//  Created by Hadrien Grasland on 26/10/16.
//
//

#ifndef ALGORITHMCONTEXT_H
#define ALGORITHMCONTEXT_H

#include <memory>

#include "ACTFW/Framework/EventContext.hpp"

namespace FW {

/// @class AlgorithmContext
///
/// Describes the properties of a test framework algorithm execution
///
struct AlgorithmContext
{
  size_t algorithmNumber;  ///< Unique identifier of an algorithm in the sequence
  std::shared_ptr<const FW::EventContext> eventContext;  ///< Link to the event context
};

}

#endif  // ALGORITHMCONTEXT_H

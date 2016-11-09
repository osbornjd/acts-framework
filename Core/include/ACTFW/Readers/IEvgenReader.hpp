//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef ACTFW_READERS_IEVGENREADER_H
#define ACTFW_READERS_IEVGENREADER_H 1

#include "ACTS/Extrapolation/ExtrapolationCell.hpp"


namespace FW {

/// @class IExtrapolatiionCellWriter
///
/// Interface class for extrapolation cell writers, 
/// returns a vector of Vertices of stable particles
///
class IEvgenReader
{
public:
  /// Virtual Destructor
  virtual
  ~IEvgenReader() {}

  /// returns the list of process vertices 
  virtual std::vector< Acts::ProcessVertex > 
  processVertices() const 
    = 0;
  
  /// skip some events
  virtual void
  skip(size_t nEvents = 1) const 
    = 0;

};
}

#endif  // ACTFW_READERS_IEVGENREADER_H
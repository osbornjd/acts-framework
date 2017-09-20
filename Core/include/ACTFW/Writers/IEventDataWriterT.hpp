//  IPlanarClusterWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_WRITERS_IEventDataWriterTT_H
#define ACTFW_WRITERS_IEventDataWriterTT_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTS/Utilities/GeometryID.hpp"

namespace FW {

/// @class IEventDataWriterT
///
/// Interface class for writing EventData are ordered
/// in DataContainers
///
template <class T> class IEventDataWriterT : public IService
{
public:
  /// The write interface
  /// @param dd is the detector data in the dedicated container
  /// @return is a ProcessCode indicating success/failure
  virtual ProcessCode
  write(const DetectorData<geo_id_value, T>& dd) = 0;

  /// write a bit of string
  /// @param sinfo is some string info to be written
  /// @return is a ProcessCode indicating return/failure
  virtual ProcessCode
  write(const std::string& sinfo) = 0;
  
};

}

#endif  // ACTFW_WRITERS_IEventDataWriterTT_H

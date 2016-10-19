//  DummyExCellWriter.h
//  ACTS-Development
//
//  Created by Hadrien Grasland on 18/10/16.
//
//

#ifndef ACTFW_WRITERS_DUMMYEXCELLWRITER_H
#define ACTFW_WRITERS_DUMMYEXCELLWRITER_H 1

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IExtrapolationCellWriter.hpp"

namespace FW {

/// @class DummyExCellWriter
///
/// An extrapolation cell writer which does not actually write anything
///
class DummyExCellWriter : public IExtrapolationCellWriter
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    std::string                   name;      ///< the name of the service

    Config(const std::string& p_name = "DummyExCellWriter")
      : name(p_name)
    {
    }
  };

  /// Constructor
  ///
  /// @param name is the name that the dummy writer will use
  DummyExCellWriter(const Config& cfg) : m_cfg(cfg) { }

  /// Destructor
  virtual ~DummyExCellWriter() {}

  /// Framework intialize method
  FW::ProcessCode
  initialize() final {}

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final {}

  /// The write interface
  ///
  /// @param eCell will be ignored
  FW::ProcessCode
  write(const Acts::ExCellCharged& eCell) final {}

  /// The write interface
  ///
  /// @param eCell will be ignored
  FW::ProcessCode
  write(const Acts::ExCellNeutral& eCell) final {}

  /// Framework name() method
  const std::string&
  name() const final { return m_cfg.name; }
  
private:
   Config             m_cfg;               ///< the config class
};

}

#endif  // ACTFW_WRITERS_DUMMYEXCELLWRITER_H

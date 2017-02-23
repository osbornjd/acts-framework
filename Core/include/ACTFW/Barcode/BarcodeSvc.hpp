//
//  RandomNumbersSvc.hpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#ifndef ACTFW_BARCODE_BARCODESVC_H
#define ACTFW_BARCODE_BARCODESVC_H 1

#include <array>
#include <string>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Utilities/Helpers.hpp"

// barcodes
typedef unsigned long barcode_type;

namespace FW {

/// @class RandomNumbersSvc
///
/// An implementation of a counting barcode service
///
class BarcodeSvc : public IService
{
public:
  /// @class Config
  ///
  /// Nested Configuration class
  struct Config
  {
    barcode_type vertex_mask     = 0xfff0000000000000;
    barcode_type primary_mask    = 0x000ffff000000000;
    barcode_type generation_mask = 0x0000000fff000000;
    barcode_type secondary_mask  = 0x0000000000fff000;
    barcode_type process_mask    = 0x0000000000000fff;
    std::string  name            = "BarcodeSvc";

    ///
    Config() {}
  };

  /// Constructor
  BarcodeSvc(const Config&                 bsConfig,
             std::unique_ptr<Acts::Logger> mlogger
             = Acts::getDefaultLogger("BarcodeSvc", Acts::Logging::INFO));

  /// Destructor
  ~BarcodeSvc() {}

  /// Framework initialize method
  FW::ProcessCode
  initialize() override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() override final;

  /// generate a new barcode / encoding
  barcode_type
  generate(barcode_type vertex,
           barcode_type primary,
           barcode_type generation,
           barcode_type secondary,
           barcode_type process) const;

  /// get the numbers back / decoding
  barcode_type
  vertex(barcode_type barcode) const;

  /// get the numbers back / decoding
  barcode_type
  primary(barcode_type barcode) const;

  /// get the numbers back / decoding
  barcode_type
  generation(barcode_type barcode) const;

  /// get the numbers back / decoding
  barcode_type
  secondary(barcode_type barcode) const;

  /// get the numbers back / decoding
  barcode_type
  process(barcode_type barcode) const;

  /// Framework name() method
  const std::string&
  name() const override final;

private:
  Config                        m_cfg;     ///< the configuration class
  std::shared_ptr<Acts::Logger> m_logger;  ///!< the logging instance

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

inline const std::string&
BarcodeSvc::name() const
{
  return m_cfg.name;
}

inline barcode_type
BarcodeSvc::generate(barcode_type vertex = 0,
                     barcode_type primary = 0,
                     barcode_type generation = 0,
                     barcode_type secondary = 0,
                     barcode_type process = 0) const
{

  // create the barcode
  barcode_type barcode = ACTS_BIT_ENCODE(vertex, m_cfg.vertex_mask);
  // now add all other parameters
  barcode += ACTS_BIT_ENCODE(primary, m_cfg.primary_mask);
  barcode += ACTS_BIT_ENCODE(generation, m_cfg.generation_mask);
  barcode += ACTS_BIT_ENCODE(secondary, m_cfg.secondary_mask);
  barcode += ACTS_BIT_ENCODE(process, m_cfg.process_mask);
  /// and return
  return barcode;
}

inline barcode_type
BarcodeSvc::vertex(barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.vertex_mask);
}

inline barcode_type
BarcodeSvc::primary(barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.primary_mask);
}

inline barcode_type
BarcodeSvc::generation(barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.generation_mask);
}

inline barcode_type
BarcodeSvc::secondary(barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.secondary_mask);
}

inline barcode_type
BarcodeSvc::process(barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.process_mask);
}
}

#endif  // ACTFW_RANDOM_RANDOMNUMBERSSVC_H

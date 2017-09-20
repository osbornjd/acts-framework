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
#include "ACTS/Utilities/Helpers.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

// barcodes
typedef unsigned long barcode_type;

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
  };

  /// Constructor
  BarcodeSvc(const Config&                       cfg,
             std::unique_ptr<const Acts::Logger> logger
             = Acts::getDefaultLogger("BarcodeSvc", Acts::Logging::INFO));

  /// Framework name() method
  std::string
  name() const final override;

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

private:
  Config                              m_cfg;     ///< the configuration class
  std::shared_ptr<const Acts::Logger> m_logger;  ///!< the logging instance

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

inline barcode_type
BarcodeSvc::generate(barcode_type vertex     = 0,
                     barcode_type primary    = 0,
                     barcode_type generation = 0,
                     barcode_type secondary  = 0,
                     barcode_type process    = 0) const
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

}  // namespace FW

inline FW::barcode_type
FW::BarcodeSvc::vertex(FW::barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.vertex_mask);
}

inline FW::barcode_type
FW::BarcodeSvc::primary(FW::barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.primary_mask);
}

inline FW::barcode_type
FW::BarcodeSvc::generation(FW::barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.generation_mask);
}

inline FW::barcode_type
FW::BarcodeSvc::secondary(FW::barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.secondary_mask);
}

inline FW::barcode_type
FW::BarcodeSvc::process(FW::barcode_type barcode) const
{
  return ACTS_BIT_DECODE(barcode, m_cfg.process_mask);
}

#endif  // ACTFW_RANDOM_RANDOMNUMBERSSVC_H

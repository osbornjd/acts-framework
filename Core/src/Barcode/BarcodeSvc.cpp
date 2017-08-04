//
//  BarcodeSvc.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//
#include "ACTFW/Barcode/BarcodeSvc.hpp"

FW::BarcodeSvc::BarcodeSvc(const FW::BarcodeSvc::Config&       cfg,
                           std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

std::string
FW::BarcodeSvc::name() const
{
  return "BarcodeSvc";
}

FW::ProcessCode
FW::BarcodeSvc::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::BarcodeSvc::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

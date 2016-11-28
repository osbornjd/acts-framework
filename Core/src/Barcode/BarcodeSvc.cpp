//
//  BarcodeSvc.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//
#include "ACTFW/Barcode/BarcodeSvc.hpp"

FW::BarcodeSvc::BarcodeSvc(const FW::BarcodeSvc::Config& cfg,
                           std::unique_ptr<Acts::Logger> mlogger)
  : m_cfg(cfg), m_logger(std::move(mlogger))
{
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


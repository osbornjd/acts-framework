// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "ACTFW/Plugins/Obj/ObjSurfaceWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjTrackingGeometryWriter.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <class AOPT>
  void
  addObjWriterOptions(AOPT& opt)
  {
    opt.add_options()("obj-tg-fileheader",
                      po::value<std::string>()->default_value(""),
                      "The (optional) file header for the tracking geometry.")(
                      "obj-tg-sensitiveheader",
                      po::value<std::string>()->default_value(""),
                      "The (optional) header in front of sensitive sensors.")(
                      "obj-tg-layerheader",
                      po::value<std::string>()->default_value(""),
                      "The (optional) header in front of layer surfaces.")(
                      "obj-sf-fileheader",
                      po::value<std::string>()->default_value(""),
                      "The (optional) file header for the surface writer.")(
                      "obj-sf-phisegments",
                      po::value<int>()->default_value(72),
                      "Number of phi segments to approximate curves.")(
                      "obj-sf-outputprecission",
                      po::value<int>()->default_value(6),
                      "Floating number output precission.")(
                      "obj-sf-outputScalor",
                      po::value<double>()->default_value(1.),
                      "Scale factor to be applied.")(
                      "obj-sf-outputThickness",
                      po::value<double>()->default_value(1.),
                      "The surface thickness.")(
                      "obj-sf-outputSensitive",
                      po::value<bool>()->default_value(true),
                      "Write sensitive surfaces.")(
                      "obj-sf-otuputLayers",
                      po::value<bool>()->default_value(true),
                      "Write layer surfaces.");

  }

  /// read the evgen options and return a Config file
  template <class AMAP>
  FW::Obj::ObjTrackingGeometryWriter::Config
  readObjTrackingGeometryWriterConfig(const AMAP& vm, 
                                      const std::string& name)
  {
    FW::Obj::ObjTrackingGeometryWriter::Config objTgConfig(name);
    objTgConfig.filePrefix           = vm["obj-tg-fileheader"].template as<std::string>();
    objTgConfig.sensitiveGroupPrefix = vm["obj-tg-sensitiveheader"].template as<std::string>();
    objTgConfig.layerPrefix          = vm["obj-tg-layerheader"].template as<std::string>();
    // return the config
    return objTgConfig;
  }

  template <class AMAP>
  FW::Obj::ObjSurfaceWriter::Config
  readObjSurfaceWriterConfig(const AMAP& vm, 
                             const std::string& name)
  {
    FW::Obj::ObjSurfaceWriter::Config objSfConfig(name, Acts::Logging::INFO);
    objSfConfig.filePrefix         = vm["obj-sf-fileheader"].template as<std::string>();
    objSfConfig.outputPhiSegemnts  = vm["obj-sf-phisegments"].template as<int>();
    objSfConfig.outputPrecision    = vm["obj-sf-outputprecission"].template as<int>();
    objSfConfig.outputScalor       = vm["obj-sf-outputScalor"].template as<double>();
    objSfConfig.outputThickness    = vm["obj-sf-outputThickness"].template as<double>();
    objSfConfig.outputSensitive    = vm["obj-sf-outputSensitive"].template as<bool>();
    objSfConfig.outputLayerSurface = vm["obj-sf-outputLayers"].template as<bool>();    
    return objSfConfig;
  
  }

} // namespace Options
} // namespace FW

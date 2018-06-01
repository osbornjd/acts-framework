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
#include "ACTFW/Plugins/Csv/CsvSurfaceWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvTrackingGeometryWriter.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <class AOPT>
  void
  addCsvWriterOptions(AOPT& opt)
  {
    opt.add_options()("csv-tg-fileheader",
                      po::value<std::string>()->default_value(""),
                      "The (optional) file header for the tracking geometry.")(
                      "csv-tg-layerheader",
                      po::value<std::string>()->default_value(""),
                      "The (optional) header in front of layers.")(
                      "csv-sf-fileheader",
                      po::value<std::string>()->default_value(""),
                      "The (optional) file header for the surface writer.")(
                      "csv-sf-outputprecission",
                      po::value<int>()->default_value(6),
                      "Floating number output precission.")(
                      "csv-sf-outputScalor",
                      po::value<double>()->default_value(1.),
                      "Scale factor to be applied.")(
                      "csv-sf-outputBounds",
                      po::value<bool>()->default_value(true),
                      "Write the surface bounds to the file.")(
                      "csv-sf-outputSensitive",
                      po::value<bool>()->default_value(true),
                      "Write sensitive surfaces.")(
                      "csv-sf-otuputLayers",
                      po::value<bool>()->default_value(true),
                      "Write layer surfaces.");

  }

  /// read the evgen options and return a Config file
  template <class AMAP>
  FW::Csv::CsvTrackingGeometryWriter::Config
  readCsvTrackingGeometryWriterConfig(const AMAP& vm, const std::string& name)
  {
    FW::Csv::CsvTrackingGeometryWriter::Config 
      objTgConfig(name, Acts::Logging::INFO);
    objTgConfig.filePrefix           = vm["csv-tg-fileheader"].template as<std::string>();
    objTgConfig.layerPrefix          = vm["csv-tg-layerheader"].template as<std::string>();
    // return the config
    return objTgConfig;
  }

  template <class AMAP>
  FW::Csv::CsvSurfaceWriter::Config
  readCsvSurfaceWriterConfig(const AMAP& vm, const std::string& name)
  {
    FW::Csv::CsvSurfaceWriter::Config 
      objSfConfig("CsvSurfaceWriter", Acts::Logging::INFO);
    objSfConfig.filePrefix         = vm["csv-sf-fileheader"].template as<std::string>();
    objSfConfig.outputPrecision    = vm["csv-sf-outputprecission"].template as<int>();
    objSfConfig.outputScalor       = vm["csv-sf-outputScalor"].template as<double>();
    objSfConfig.outputBounds       = vm["csv-sf-outputBounds"].template as<bool>();
    objSfConfig.outputSensitive    = vm["csv-sf-outputSensitive"].template as<bool>();
    objSfConfig.outputLayerSurface = vm["csv-sf-outputLayers"].template as<bool>();    
    return objSfConfig;
  }

} // namespace Options
} // namespace FW

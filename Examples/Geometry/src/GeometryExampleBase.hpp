// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include "ACTFW/Utilities/Options.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Plugins/Obj/ObjWriterOptions.hpp"
#include "ACTFW/Plugins/Obj/ObjSurfaceWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjTrackingGeometryWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvWriterOptions.hpp"
#include "ACTFW/Plugins/Csv/CsvSurfaceWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvTrackingGeometryWriter.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"

namespace po = boost::program_options;

template <typename geometry_options_t, typename geometry_getter_t>
int
processGeometry(int argc, char* argv[], 
                geometry_options_t geometryOptions, 
                geometry_getter_t trackingGeometry)
{

  // Declare the supported program options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "Produce help message")(
      "sloglevel",
      po::value<size_t>()->default_value(2),
      "The output log level for surfaces")(
      "lloglevel",
      po::value<size_t>()->default_value(2),
      "The output log level for surfaces")(
      "vloglevel",
      po::value<size_t>()->default_value(2),
      "The output log level for surfaces")
      ("geo-subdetectors",
      po::value<read_strings>()->multitoken()->default_value({{}}),
      "Sub detectors for the output writing");

  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add options for the Obj writing 
  FW::Options::addObjWriterOptions<po::options_description>(desc);
  
  // add specific options for this geometry
  geometryOptions(desc);
  
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }

  // now read the standard options
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto logLevel = standardOptions.second;
  
  Acts::Logging::Level surfaceLogLevel
      = Acts::Logging::Level(vm["sloglevel"].template as<size_t>());
  Acts::Logging::Level layerLogLevel
      = Acts::Logging::Level(vm["lloglevel"].template as<size_t>());
  Acts::Logging::Level volumeLogLevel
      = Acts::Logging::Level(vm["vloglevel"].template as<size_t>());
  
  auto tGeometry = trackingGeometry(vm);
      
  // the detectors
  read_strings subDetectors = vm["geo-subdetectors"].as< read_strings >();
    
  // ---------------------------------------------------------------------------------
  // OBJ output 
  // the writers
  std::vector<std::shared_ptr<FW::Obj::ObjSurfaceWriter>> subWriters;
  std::vector<std::shared_ptr<std::ofstream>>           subStreams;
  // loop and create the obj output writers per defined sub detector 
  for (auto sdet : subDetectors) {
    // sub detector stream
    auto        sdStream = std::shared_ptr<std::ofstream>(new std::ofstream);
    std::string sdOutputName = sdet + std::string(".obj");
    sdStream->open(sdOutputName);
    // object surface writers
    FW::Obj::ObjSurfaceWriter::Config sdObjWriterConfig
       = FW::Options::readObjSurfaceWriterConfig(vm, 
                                                 sdet,
                                                 surfaceLogLevel);
    sdObjWriterConfig.outputStream       = sdStream;
    auto sdObjWriter
        = std::make_shared<FW::Obj::ObjSurfaceWriter>(sdObjWriterConfig);
    // push back
    subWriters.push_back(sdObjWriter);
    subStreams.push_back(sdStream);
  }
          
  // configure the tracking geometry writer
  auto tgObjWriterConfig 
    = FW::Options::readObjTrackingGeometryWriterConfig(vm,
                                                       "ObjTrackingGeometryWriter",
                                                       volumeLogLevel);
                                                       
  tgObjWriterConfig.surfaceWriters = subWriters;
  auto tgObjWriter
      = std::make_shared<FW::Obj::ObjTrackingGeometryWriter>(tgObjWriterConfig);

  // write the tracking geometry object
  tgObjWriter->write(*tGeometry);

  // --------------------------------------------------------------------------------
  // close the output streams
  for (auto sStreams : subStreams) {
    sStreams->close();
  }
    
  // CSV output
  // sub detector stream
  // auto csvStream = std::shared_ptr<std::ofstream>(new std::ofstream);
  // std::string csvOutputName = "Detector.csv";
  // csvStream->open(csvOutputName);
  // 
  // FW::Csv::CsvSurfaceWriter::Config sfCsvWriterConfig
  //   = FW::Options::readCsvSurfaceWriterConfig(vm,"CsvSurfaceWriter");
  // sfCsvWriterConfig.outputStream       = csvStream;
  // auto sfCsvWriter
  //         = std::make_shared<FW::Csv::CsvSurfaceWriter>(sfCsvWriterConfig);
  // 
  // // configure the tracking geometry writer
  // FW::Csv::CsvTrackingGeometryWriter::Config tgCsvWriterConfig
  //   = FW::Options::readCsvTrackingGeometryWriterConfig(vm,
  //                                                      "CsvTrackingGeometryWriter");
  // tgCsvWriterConfig.surfaceWriter       = sfCsvWriter;
  // tgCsvWriterConfig.layerPrefix         = "\n";
  // // the tracking geometry writers
  // auto tgCsvWriter
  //     = std::make_shared<FW::Csv::CsvTrackingGeometryWriter>(tgCsvWriterConfig);
  // // ---------------------------------------------------------------------------------
  // // write the tracking geometry object
  // tgCsvWriter->write(*(tGeometry.get()));
  // // close the file
  // csvStream->close();
  
  return 0;
}

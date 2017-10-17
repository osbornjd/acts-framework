// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// GenatinoRecording.cpp
///////////////////////////////////////////////////////////////////

#include <boost/program_options.hpp>
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Plugins/Obj/ObjSurfaceWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjTrackingGeometryWriter.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
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
      "The output log level for surfaces");

  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // --------------------------------------------------------------------------------
  // set geometry building logging level
  Acts::Logging::Level surfaceLogLevel
      = Acts::Logging::Level(vm["sloglevel"].as<size_t>());
  Acts::Logging::Level layerLogLevel
      = Acts::Logging::Level(vm["lloglevel"].as<size_t>());
  Acts::Logging::Level volumeLogLevel
      = Acts::Logging::Level(vm["vloglevel"].as<size_t>());

  // create the tracking geometry as a shared pointer
  std::shared_ptr<const Acts::TrackingGeometry> tGeometry
      = FWGen::buildGenericDetector(
          surfaceLogLevel, layerLogLevel, volumeLogLevel, 3);

  // the detectors
  std::vector<std::string> subDetectors
      = {"BeamPipe", "Pix", "PST", "SStrip", "LStrip"};
  // the writers
  std::vector<std::shared_ptr<FWObj::ObjSurfaceWriter>> subWriters;
  std::vector<std::shared_ptr<std::ofstream>>           subStreams;
  // loop and create
  for (auto sdet : subDetectors) {
    // sub detector stream
    auto        sdStream = std::shared_ptr<std::ofstream>(new std::ofstream);
    std::string sdOutputName = sdet + std::string(".obj");
    sdStream->open(sdOutputName);
    // object surface writers
    FWObj::ObjSurfaceWriter::Config sdObjWriterConfig(sdet,
                                                      Acts::Logging::INFO);
    sdObjWriterConfig.filePrefix         = "mtllib materials.mtl";
    sdObjWriterConfig.outputPhiSegemnts  = 72;
    sdObjWriterConfig.outputPrecision    = 6;
    sdObjWriterConfig.outputScalor       = 1.;
    sdObjWriterConfig.outputThickness    = 1.;
    sdObjWriterConfig.outputSensitive    = true;
    sdObjWriterConfig.outputLayerSurface = true;
    sdObjWriterConfig.outputStream       = sdStream;
    auto sdObjWriter
        = std::make_shared<FWObj::ObjSurfaceWriter>(sdObjWriterConfig);
    // push back
    subWriters.push_back(sdObjWriter);
    subStreams.push_back(sdStream);
  }
  // configure the tracking geometry writer
  FWObj::ObjTrackingGeometryWriter::Config tgObjWriterConfig(
      "ObjTrackingGeometryWriter", Acts::Logging::INFO);
  tgObjWriterConfig.surfaceWriters       = subWriters;
  tgObjWriterConfig.filePrefix           = "mtllib materials.mtl\n";
  tgObjWriterConfig.sensitiveGroupPrefix = "usemtl silicon\n";
  tgObjWriterConfig.layerPrefix          = "usemtl support\n";
  // the tracking geometry writers
  auto tgObjWriter
      = std::make_shared<FWObj::ObjTrackingGeometryWriter>(tgObjWriterConfig);

  // write the tracking geometry object
  tgObjWriter->write(*(tGeometry.get()));

  // --------------------------------------------------------------------------------
  // close the output streams
  for (auto sStreams : subStreams) {
    sStreams->close();
  }
}

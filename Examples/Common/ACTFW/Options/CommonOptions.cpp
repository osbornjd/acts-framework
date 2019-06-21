// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Options/CommonOptions.hpp"

#include "ACTFW/Utilities/Options.hpp"

using namespace boost::program_options;

boost::program_options::options_description
FW::Options::makeDefaultOptions(std::string caption)
{
  options_description opt(caption);

  opt.add_options()("help,h", "Produce help message");
  opt.add_options()(
      "loglevel,l",
      value<size_t>()->default_value(2),
      "The output log level. Please set the wished number (0 = VERBOSE, 1 = "
      "DEBUG, 2 = INFO, 3 = WARNING, 4 = ERROR, 5 = FATAL).");

  return opt;
}

void
FW::Options::addSequencerOptions(
    boost::program_options::options_description& opt)
{
  // sequencer options
  opt.add_options()("events,n",
                    value<size_t>(),
                    "The number of events to process. If not given, all "
                    "available events will be processed.")(
      "skip",
      value<size_t>()->default_value(0),
      "The number of events to skip")(
      "jobs,j",
      value<int>()->default_value(-1),
      "Number of parallel jobs, negative for automatic.");
}

void
FW::Options::addGeometryOptions(
    boost::program_options::options_description& opt)
{
  opt.add_options()("geo-surface-loglevel",
                    value<size_t>()->default_value(3),
                    "The outoput log level for the surface building.")(
      "geo-layer-loglevel",
      value<size_t>()->default_value(3),
      "The output log level for the layer building.")(
      "geo-volume-loglevel",
      value<size_t>()->default_value(3),
      "The output log level for the volume building.")(
      "geo-subdetectors",
      value<read_strings>()->multitoken()->default_value({{}}),
      "Sub detectors for the output writing");
}

void
FW::Options::addOutputOptions(boost::program_options::options_description& opt)
{
  // Add specific options for this example
  opt.add_options()("output-dir",
                    value<std::string>()->default_value(""),
                    "Output directory location.")(
      "output-root",
      value<bool>()->default_value(false),
      "Switch on to write '.root' output file(s).")(
      "output-csv",
      value<bool>()->default_value(false),
      "Switch on to write '.csv' output file(s).")(
      "output-obj",
      value<bool>()->default_value(false),
      "Switch on to write '.obj' ouput file(s).")(
      "output-json",
      value<bool>()->default_value(false),
      "Switch on to write '.json' ouput file(s).");
}

boost::program_options::variables_map
FW::Options::parse(const boost::program_options::options_description& opt,
                   int                                                argc,
                   char*                                              argv[])
{
  variables_map vm;
  store(parse_command_line(argc, argv, opt), vm);
  notify(vm);
  // automatically handle help
  if (vm.count("help")) {
    std::cout << opt << std::endl;
    vm.clear();
  }
  return vm;
}

Acts::Logging::Level
FW::Options::readLogLevel(const boost::program_options::variables_map& vm)
{
  return Acts::Logging::Level(vm["loglevel"].as<size_t>());
}

FW::Sequencer::Config
FW::Options::readSequencerConfig(
    const boost::program_options::variables_map& vm)
{
  Sequencer::Config cfg;
  cfg.skip = vm["skip"].as<size_t>();
  if (not vm["events"].empty()) {
    cfg.events = vm["events"].as<size_t>();
  }
  cfg.logLevel   = readLogLevel(vm);
  cfg.numThreads = vm["jobs"].as<int>();
  if (not vm["output-dir"].empty()) {
    cfg.outputDir = vm["output-dir"].as<std::string>();
  }
  return cfg;
}

#ifndef ACTFW_BFIELD_BFIELDEXAMPLE_H
#define ACTFW_BFIELD_BFIELDEXAMPLE_H

#include <boost/program_options.hpp>
#include <string>
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Plugins/BField/BFieldUtils.hpp"
#include "ACTFW/Plugins/BField/RootInterpolatedBFieldWriter.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"
#include "ACTS/MagneticField/concept/AnyFieldLookup.hpp"

/// The main executable
///
/// Creates an InterpolatedBFieldMap from a txt or csv file and writes out the
/// grid points and values of the map into root format. The Field can then be
/// displayed using the root script printBField.cpp

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "Produce help message")(
      "map,m",
      po::value<std::string>(),
      "Set the string to the magnetic field map to be read in")(
      "root",
      po::value<bool>()->default_value(false),
      "Please set this flag to true, if your file format is root. "
      "The default is is txt/csv.")(
      "treeName",
      po::value<std::string>()->default_value("bField"),
      "In case your field map file is given in root format, please specify the "
      "name of the TTree.")(
      "out,o",
      po::value<std::string>()->default_value("bField.root"),
      "Set the name of the root output file. Default is bField.root")(
      "nPoints,n",
      po::value<size_t>()->default_value(100000),
      "Estimate of number of grid points, needed for allocation.")(
      "lScalor",
      po::value<double>()->default_value(1.),
      "The default unit for the grid "
      "points is mm. In case the grid points of your field map has another "
      "unit, please set  the scalor to mm.")(
      "bScalor",
      po::value<double>()->default_value(1.),
      "The default unit for the magnetic field values is Tesla. In case the "
      "grid points of your field map has another unit, please set  the scalor "
      "to [T].")(
      "rz",
      po::value<bool>()->default_value(false),
      "Please set this flag to true, if your grid points and your "
      "magnetic field values are given in 'rz'. The default is 'xyz'.")(
      "firstOctant,f",
      po::value<bool>()->default_value(false),
      "Please set this flag to true, if your field map is only given for the "
      "first octant/quadrant and should be symmetrically created for all other "
      "octants/quadrants.");

  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  // output messages
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }

  if (vm.count("map")) {
    std::cout << "Read in magnetic field map: " << vm["map"].as<std::string>()
              << std::endl;
  } else {
    std::cout << "Magnetic field map was not set. Stop program." << std::endl;
    return -1;
  }

  if (vm["root"].as<bool>()) {
    std::cout << "BField map is given in root file format." << std::endl;
    if (vm.count("treeName"))
      std::cout << "The TTree name is: " << vm["treeName"].as<std::string>()
                << std::endl;
  } else
    std::cout << "BField map is given in txt/csv file format." << std::endl;

  if (vm.count("out")) {
    std::cout << "Output file is set to: " << vm["out"].as<std::string>()
              << std::endl;
  }

  if (vm.count("nPoints")) {
    std::cout << "Number of points set to: " << vm["nPoints"].as<size_t>()
              << std::endl;
  }

  if (vm.count("lScalor")) {
    std::cout << "Length scalor to mm set to: " << vm["lScalor"].as<double>()
              << std::endl;
  }

  if (vm.count("bScalor")) {
    std::cout << "BField scalor to Tesla set to: " << vm["bScalor"].as<double>()
              << std::endl;
  }

  if (vm["rz"].as<bool>())
    std::cout << "BField map is given in 'rz' coordiantes." << std::endl;
  else
    std::cout << "BField map is given in 'xyz' coordiantes." << std::endl;

  if (vm["firstOctant"].as<bool>()) {
    std::cout << "Only the first octant/quadrant is given, bField map will be "
                 "symmetrically created for all other octants/quadrants"
              << std::endl;
  }

  // Declare the mapper
  Acts::concept::AnyFieldLookup<> mapper;

  double lengthUnit = vm["lScalor"].as<double>() * Acts::units::_mm;
  double BFieldUnit = vm["bScalor"].as<double>() * Acts::units::_T;

  // set the mapper
  if (vm["root"].as<bool>()) {
    if (vm["rz"].as<bool>()) {
      mapper = FW::BField::root::fieldMapperRZ(
          [](std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ) {
            return (binsRZ.at(1) * nBinsRZ.at(0) + binsRZ.at(0));
          },
          vm["map"].as<std::string>(),
          vm["treeName"].as<std::string>(),
          lengthUnit,
          BFieldUnit,
          vm["firstOctant"].as<bool>());
    } else {
      mapper = FW::BField::root::fieldMapperXYZ(
          [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
            return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2))
                    + binsXYZ.at(1) * nBinsXYZ.at(2)
                    + binsXYZ.at(2));
          },
          vm["map"].as<std::string>(),
          vm["treeName"].as<std::string>(),
          lengthUnit,
          BFieldUnit,
          vm["firstOctant"].as<bool>());
    }
  } else {
    if (vm["rz"].as<bool>()) {
      mapper = FW::BField::txt::fieldMapperRZ(
          [](std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ) {
            return (binsRZ.at(1) * nBinsRZ.at(0) + binsRZ.at(0));
          },
          vm["map"].as<std::string>(),
          lengthUnit,
          BFieldUnit,
          vm["nPoints"].as<size_t>(),
          vm["firstOctant"].as<bool>());
    } else {
      mapper = FW::BField::txt::fieldMapperXYZ(
          [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
            return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2))
                    + binsXYZ.at(1) * nBinsXYZ.at(2)
                    + binsXYZ.at(2));
          },
          vm["map"].as<std::string>(),
          lengthUnit,
          BFieldUnit,
          vm["nPoints"].as<size_t>(),
          vm["firstOctant"].as<bool>());
    }
  }
  Acts::InterpolatedBFieldMap::Config config;
  config.scale  = 1.;
  config.mapper = std::move(mapper);
  // create BField service
  auto bField
      = std::make_shared<const Acts::InterpolatedBFieldMap>(std::move(config));

  // Create the InterpolatedBFieldWriter
  FW::BField::RootInterpolatedBFieldWriter::Config writerConfig;
  if (vm["rz"].as<bool>())
    writerConfig.gridType = FW::BField::GridType::rz;
  else
    writerConfig.gridType = FW::BField::GridType::xyz;
  writerConfig.treeName   = "bField";
  writerConfig.fileName   = vm["out"].as<std::string>();

  writerConfig.bField = bField;
  auto bFieldWriter
      = std::make_shared<FW::BField::RootInterpolatedBFieldWriter>(
          writerConfig);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({bFieldWriter});
  sequencer.run(1);
}

#endif  // ACTFW_BFIELD_BFIELDEXAMPLE_H

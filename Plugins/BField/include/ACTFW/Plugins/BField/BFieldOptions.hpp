///////////////////////////////////////////////////////////////////
// TxtBFieldUtils.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_OPTIONS_BFIELDOPTIONS_HPP
#define ACTFW_OPTIONS_BFIELDOPTIONS_HPP

#include <utility>
#include <iostream>
#include "ACTFW/Plugins/BField/BFieldUtils.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/MagneticField/concept/AnyFieldLookup.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Utilities/Units.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common bfield options
  template <class AOPT>
  void
  addBFieldOptions(AOPT& opt){
    opt.add_options()(
        "bfieldmap",
        po::value<std::string>()->default_value(""),
        "Set this string to point to the bfield source file."
        "That can either be a '.txt', a '.cvs' or a '.root' file. "
        "Omit for a constant magnetic field.")(
        "bfieldname",
        po::value<std::string>()->default_value("bField"),
        "In case your field map file is given in root format, please specify the "
        "name of the TTree.")(
        "gridpoints",
        po::value<size_t>()->default_value(100000),
        "Estimate of number of grid points, needed for allocation.")(
        "lscalor",
        po::value<double>()->default_value(1.),
        "The default unit for the grid "
        "points is mm. In case the grid points of your field map has another "
        "unit, please set  the scalor to mm.")(
        "bscalor",
        po::value<double>()->default_value(1.),
        "The default unit for the magnetic field values is Tesla. In case the "
        "grid points of your field map has another unit, please set  the scalor "
        "to [T]. In case of a constant map, this is the value of the field in [T] "
        "along the z axis.")(
        "rz",
        po::value<bool>()->default_value(false),
        "Please set this flag to true, if your grid points and your "
        "magnetic field values are given in 'rz'. The default is 'xyz'.")(
        "foctant",
        po::value<bool>()->default_value(false),
        "Please set this flag to true, if your field map is only given for the "
        "first octant/quadrant and should be symmetrically created for all other "
        "octants/quadrants.");
  }

  // create the map  
  template <class AMAP> 
  std::pair<
  std::shared_ptr<Acts::InterpolatedBFieldMap>,
  std::shared_ptr<Acts::ConstantBField> >  
  readBField(const AMAP& vm) {
    
    std::string bfieldmap = "constfield";
    int bfieldmaptype = 0;
    if (vm.count("bfieldmap")) {
      bfieldmap = vm["bfieldmap"].template as<std::string>();
      std::cout << "- read in magnetic field map: " 
                << vm["bfieldmap"].template as<std::string>() << std::endl;
      if (bfieldmap.find(".root") != std::string::npos){
          std::cout << "- root format for magnetic field detected" << std::endl;
          bfieldmaptype = 1; 
      } else if (bfieldmap.find(".txt") != std::string::npos ||
                 bfieldmap.find(".csv") != std::string::npos){
        std::cout << "- txt format for magnetic field detected" << std::endl;
        bfieldmaptype = 2; 
      } else {
        std::cout << "- magnetic field format could not be detected";
        std::cout << " use '.root', '.txt', or '.csv'." << std::endl;
        return std::pair<
            std::shared_ptr<Acts::InterpolatedBFieldMap>,
            std::shared_ptr<Acts::ConstantBField> >(nullptr,nullptr);
      }
    } 
    if (vm.count("gridpoints")) {
      std::cout << "- number of points set to: " 
                << vm["gridpoints"].template as<size_t>()
                << std::endl;
    }
    double lscalor = 1.;
    if (vm.count("lscalor")) {
      lscalor = vm["lscalor"].template as<double>();
      std::cout << "- length scalor to mm set to: " << lscalor << std::endl;
    }
    double bscalor = 1.;
    if (vm.count("bscalor")) {
      bscalor = vm["bscalor"].template as<double>();
      std::cout << "- BField scalor to Tesla set to: " << bscalor << std::endl;
    }
    if (vm["rz"].template as<bool>())
      std::cout << "- BField map is given in 'rz' coordiantes." << std::endl;
    else
      std::cout << "- BField map is given in 'xyz' coordiantes." << std::endl;
    
    if (vm["foctant"].template as<bool>()) {
      std::cout << "- Only the first octant/quadrant is given, bField map will be "
                   "symmetrically created for all other octants/quadrants"
                << std::endl;
    }
      
    // Declare the mapper
    Acts::concept::AnyFieldLookup<> mapper;
    double lengthUnit = lscalor * Acts::units::_mm;
    double BFieldUnit = bscalor * Acts::units::_T;
    
    // set the mapper
    if (bfieldmaptype == 1) {
      if (vm["rz"].template as<bool>()) {
        mapper = FW::BField::root::fieldMapperRZ(
            [](std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ) {
              return (binsRZ.at(1) * nBinsRZ.at(0) + binsRZ.at(0));
            },
            vm["bfieldmap"].template as<std::string>(),
            vm["bfieldname"].template as<std::string>(),
            lengthUnit,
            BFieldUnit,
            vm["foctant"].template as<bool>());
      } else {
        mapper = FW::BField::root::fieldMapperXYZ(
            [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
              return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2))
                      + binsXYZ.at(1) * nBinsXYZ.at(2)
                      + binsXYZ.at(2));
            },
            vm["bfieldmap"].template as<std::string>(),
            vm["bfieldname"].template as<std::string>(),
            lengthUnit,
            BFieldUnit,
            vm["foctant"].template as<bool>());
      }
    } else {
      if (vm["rz"].template as<bool>()) {
        mapper = FW::BField::txt::fieldMapperRZ(
            [](std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ) {
              return (binsRZ.at(1) * nBinsRZ.at(0) + binsRZ.at(0));
            },
            vm["bfieldmap"].template as<std::string>(),
            lengthUnit,
            BFieldUnit,
            vm["gridpoints"].template as<size_t>(),
            vm["foctant"].template as<bool>());
      } else {
        mapper = FW::BField::txt::fieldMapperXYZ(
            [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
              return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2))
                      + binsXYZ.at(1) * nBinsXYZ.at(2)
                      + binsXYZ.at(2));
            },
            vm["bfieldmap"].template as<std::string>(),
            lengthUnit,
            BFieldUnit,
            vm["gridpoints"].template as<size_t>(),
            vm["foctant"].template as<bool>());
      }
    }
    Acts::InterpolatedBFieldMap::Config config;
    config.scale  = 1.;
    config.mapper = std::move(mapper);
    
    std::shared_ptr<Acts::InterpolatedBFieldMap> bField
     = std::make_shared<Acts::InterpolatedBFieldMap>(std::move(config));
    
    std::shared_ptr<Acts::ConstantBField> cField 
      = std::make_shared<Acts::ConstantBField>(0.,0.,BFieldUnit);
   
    return std::pair<
      std::shared_ptr<Acts::InterpolatedBFieldMap>,
      std::shared_ptr<Acts::ConstantBField> >(bField,cField);
    
  }
}
}

#endif // ACTFW_OPTIONS_BFIELDOPTIONS_HPP
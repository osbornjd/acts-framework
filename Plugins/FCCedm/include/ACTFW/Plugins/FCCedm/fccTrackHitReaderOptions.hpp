#ifndef ACTFW_OPTIONS_FCCTRACKHITREADEROPTIONS_HPP
#define ACTFW_OPTIONS_FCCTRACKHITREADEROPTIONS_HPP

#include <cstdlib>
#include <iostream>
#include <utility>
#include "ACTFW/Utilities/Options.hpp"
#include "ACTS/Utilities/Units.hpp"

namespace po = boost::program_options;
namespace au = Acts::units;

namespace FW {

namespace Options {

  /// the options for the positioned track hit reader
  template <class AOPT>
  void
  addFccTrackHitReaderOptions(AOPT& opt)
  {
    opt.add_options()(
        "thReader-input",
        po::value<std::vector<std::string>>()->multitoken(),
        "The location of the input file(s) of the positioned track "
        "hits, use {'PATH_TO_FILE1/file1.root','PATH_TO_FILE2/file2.root'}")(
        "thReader-tree",
        po::value<std::string>()->default_value("events"),
        "The name of the tree to be read in.")(
        "thReader-branch",
        po::value<std::string>()->default_value("overlaidPositionedTrackHits"),
        "The name of the branch to be read in.");
  }

  /// read the particle gun options and return a Config file
  template <class AMAP>
  FW::FCCedm::fccTrackHitReader::Config
  readFccTrackHitReaderConfig(const AMAP& vm)
  {
    FW::FCCedm::fccTrackHitReader::Config thReaderConfig;
    thReaderConfig.fileList
        = vm["thReader-input"].template as<std::vector<std::string>>();
    thReaderConfig.treeName = vm["thReader-tree"].template as<std::string>();
    thReaderConfig.branchName
        = vm["thReader-branch"].template as<std::string>();

    return thReaderConfig;
  }
}
}

#endif  // ACTFW_OPTIONS_FCCTRACKHITREADEROPTIONS_HPP

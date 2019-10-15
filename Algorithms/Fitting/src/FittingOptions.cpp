#include "ACTFW/Fitting/FittingOptions.hpp"

#include "ACTFW/Utilities/Options.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace FW {

namespace Options {
  void
  addFittingOptions(boost::program_options::options_description& opt)
  {
    opt.add_options()(
        "fatras-sim-particles",
        po::value<std::string>()->default_value("fatras-particles"),
        "The collection of simulated particles.")(
        "fatras-sim-hits",
        po::value<std::string>()->default_value("fatras-hits"),
        "The collection of simulated hits")(
        "fitted-tracks",
        po::value<std::string>()->default_value("fitted-tracks"),
        "The collection of output tracks")(
        "initial-parameter-sigma",
        po::value<read_range>()->multitoken()->default_value(
            {10., 10., 0.02, 0.02, 1}),
        "Gaussian sigma used to smear the truth track parameter Loc0 [um], "
        "Loc1 [um], phi, theta, q/p [-q/(p*p)*GeV]")(
        "measurement-sigma",
        po::value<read_range>()->multitoken()->default_value({30., 30.}),
        "Gaussian sigma used to smear the truth hit Loc0 [um], Loc1 [um]");
  }
}  // namespace Options
}  // namespace FW

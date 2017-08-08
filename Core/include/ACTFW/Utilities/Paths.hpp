/// @file
/// @date 2017-08-03
/// @author Moritz Kiehnn <msmk@cern.ch>

#ifndef ACTFW_PATHS_H
#define ACTFW_PATHS_H

#include <string>

namespace FW {

/// Join dir and name into one paths with correct handling of empty dirs.
std::string
joinPaths(const std::string& dir, const std::string& name);

/// Construct a file path of the form `[<dir>/]event<XXXXX>-<name>`.
///
/// @params dir output directory, unused if empty
/// @params name basic filename
/// @params event event number
std::string
perEventFilepath(const std::string& dir, const std::string& name, size_t event);

}  // namespace FW

#endif  // ACTFW_PATHS_H

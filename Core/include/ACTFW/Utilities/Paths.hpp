/// @file
/// @date 2017-08-03
/// @author Moritz Kiehnn <msmk@cern.ch>

#ifndef ACTFW_FILENAMES_H
#define ACTFW_FILENAMES_H

#include <string>

namespace FW {
/// Construct a file path of the form `[<dir>/]event<XXXXX>-<name>`.
///
/// @params dir output directory, unused if empty
/// @params name basic filename
/// @params event event number
std::string
perEventFilepath(const std::string& dir, const std::string& name, size_t event);
}  // namespace FW

#endif  // ACTFW_FILENAMES_H

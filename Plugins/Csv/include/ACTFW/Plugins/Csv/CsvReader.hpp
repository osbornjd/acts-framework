// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_CSVREADER_H
#define ACTFW_CSVREADER_H

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "Acts/Utilities/Logger.hpp"

namespace FW {

using StringVec = std::vector<std::string>;

/// Read a csv file which is in comma-separated-value format.

class CsvReader
{
public:
  /// constructor
  /// @param filename is the file name
  /// @param level is the output logging level
  CsvReader(const std::string&   filename,
            Acts::Logging::Level level = Acts::Logging::INFO);

  /// Read one line and return corresponding values for given sets of csv
  /// parameters
  bool
  readParLine(const StringVec& csvPars, StringVec& csvVals);

  /// Read one line and return values for assumed sets of csv parameters
  bool
  readLine(StringVec& csvVals);

  /// Peek the next line
  StringVec
  peekLine();

  /// Return number of csv parameters
  size_t
  numPars() const;

  /// Return number of lines read
  size_t
  numLines() const;

  /// Get the name of parameter at a given location
  std::string
  parName(const size_t id) const;

private:
  std::string                         m_fileName;  ///< csv filename
  std::unique_ptr<const Acts::Logger> m_logger;    ///< The logging instance
  std::ifstream m_inputStream;             ///< input stream for the input file
  std::map<std::string, int> m_csvParIDs;  ///< the map of parameter location
  size_t m_nPars;   ///< number of csv parameters from header
  size_t m_nLines;  ///< number of lines read

  /// Read one line, called by readParLine, readLine and peekLine
  bool
  read(std::string& line);

  /// The logger
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

inline size_t
CsvReader::numPars() const
{
  return m_nPars;
}

inline size_t
CsvReader::numLines() const
{
  return m_nLines;
}

}  // namespace FW

#endif  // ACTFW_CSVREADER_H

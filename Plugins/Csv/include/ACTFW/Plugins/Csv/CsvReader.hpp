// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_CSVREADER_H
#define ACTFW_CSVREADER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp> 
#include "Acts/Utilities/Logger.hpp"

namespace FW{

  using StringVec = std::vector<std::string>;

  /// Read a csv file which is in comma-separated-value format.

  class CsvReader
  {
  public:
    /// constructor
    /// @param filename is the file name 
    /// @parm delm is the delimeter to parse the csv 
    CsvReader(const std::string&   filename, 
              const std::string&   delm,
              Acts::Logging::Level level = Acts::Logging::INFO);

    /// Read one line
    bool read(std::string& line);

    /// Read one line and return corresponding values for given sets of csv parameters 
    bool readParLine(const StringVec& csvPars, StringVec& csvVals);

    /// Read one line and return values for assumed sets of csv parameters
    bool readLine(StringVec& csvVals);

    /// Peek the next line
    StringVec peekLine();
        
    /// Return number of csv parameters 
    size_t numPars() const;
        
    /// Return number of lines read
    size_t numLines() const;

    /// Get the name of parameter at a given location
    std::string getParName(const size_t id) const;          
        
  private:
    std::string m_fileName;                               ///< csv filename
    std::string m_delimeter;                              ///< csv parser, e.g. ','
    std::unique_ptr<const Acts::Logger> m_logger;         ///< The logging instance 
    std::shared_ptr<std::ifstream> m_inputStream= nullptr;///< input stream for the input file      
    std::map<std::string, int> m_csvParIDs;               ///< the map of parameter location
    size_t m_nPars;                                       ///< number of csv parameters from header       
    size_t m_nLines;                                      ///< number of lines read
        
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


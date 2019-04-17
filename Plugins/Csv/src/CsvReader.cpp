// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Csv/CsvReader.hpp"

FW::CsvReader::CsvReader(const std::string&   filename,
                         Acts::Logging::Level level)
  : m_fileName(filename), m_logger(Acts::getDefaultLogger("CsvReader", level))
{
  m_inputStream.open(m_fileName, std::ifstream::in);
  if (m_inputStream.fail()) {
    throw std::ios_base::failure("Could not open '" + m_fileName
                                 + "' to read. Aborting.");
  }

  std::string line;
  StringVec   csvParNames;
  if (read(line)) {
    boost::algorithm::split(csvParNames, line, boost::is_any_of(","));
    ACTS_DEBUG(csvParNames.size() << " csvs in .csv file '" << m_fileName
                                  << "'.");
  }

  m_nPars = csvParNames.size();
  if (m_nPars == 0)
    throw std::ios_base::failure(
        "Failed to fetch header from the first line of file '" + m_fileName
        + "'.");

  for (int i = 0; i < m_nPars; i++) {
    m_csvParIDs.insert(std::pair<std::string, int>(csvParNames[i], i));
  }

  m_nLines = 0;
}

bool
FW::CsvReader::read(std::string& line)
{
  if (std::getline(m_inputStream, line)) {
    m_nLines += 1;
    if (line.empty()) {
      ACTS_WARNING("Line " << numLines() << " of file '" << m_fileName
                           << "' is empty. "
                           << "Will be skipped to continue.");
      read(line);
    }
    return true;
  }
  return false;
}

bool
FW::CsvReader::readParLine(const StringVec& csvPars, StringVec& csvVals)
{
  std::string line;
  if (read(line)) {
    StringVec csvLine;
    boost::algorithm::split(csvLine, line, boost::is_any_of(","));
    if (m_csvParIDs.size() != csvLine.size()) {
      ACTS_ERROR("Number of csvs at line " << numLines() << " is "
                                           << csvLine.size()
                                           << ". But there are "
                                           << m_csvParIDs.size()
                                           << " csvs from the header in file '"
                                           << m_fileName
                                           << "'.");
    }
    csvVals.clear();
    for (const std::string& csv : csvPars) {
      std::map<std::string, int>::const_iterator it = m_csvParIDs.find(csv);
      if (it == m_csvParIDs.end()) {
        ACTS_ERROR("The csv named '"
                   << csv
                   << "' does not exist according to the .csv header in file '"
                   << m_fileName
                   << "'.");
        return false;
      }
      unsigned int location = it->second;
      csvVals.push_back(csvLine[location]);
    }
    return true;
  }
  return false;
}

bool
FW::CsvReader::readLine(StringVec& csvVals)
{
  std::string line;
  if (read(line)) {
    boost::algorithm::split(csvVals, line, boost::is_any_of(","));
    if (m_csvParIDs.size() != csvVals.size()) {
      ACTS_ERROR("Number of csvs at line " << numLines() << " is "
                                           << csvVals.size()
                                           << ". But there are "
                                           << m_csvParIDs.size()
                                           << " csvs from the header in file '"
                                           << m_fileName
                                           << "'.");
      return false;
    }
    return true;
  }
  return false;
}

FW::StringVec
FW::CsvReader::peekLine()
{
  long unsigned pos = m_inputStream.tellg();
  StringVec     parVal;
  std::string   line;
  if (read(line)) {
    boost::algorithm::split(parVal, line, boost::is_any_of(","));
    m_inputStream.seekg(pos);
  }
  return parVal;
}

std::string
FW::CsvReader::parName(const size_t id) const
{
  const std::string name = "";
  if (id > m_nPars - 1) {
    ACTS_ERROR("Maximum input parameter id is " << m_nPars
                   - 1 << ". An empty string returned.");
  }
  for (std::map<std::string, int>::const_iterator it = m_csvParIDs.begin();
       it != m_csvParIDs.end();
       ++it) {
    if (it->second == id) return it->first;
  }
  return name;
}

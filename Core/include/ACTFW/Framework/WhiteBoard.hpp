//
//  WhiteBoard.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_FRAMEWORK_WHITEBOARD_h
#define ACTFW_FRAMEWORK_WHITEBOARD_h

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

/// @class WhiteBoard
/// Simple whit board class to read from and write to
class WhiteBoard
{
public:
  /// Constructor
  /// @param cfg is the config struct for this WhiteBoard
  WhiteBoard(std::unique_ptr<const Acts::Logger> logger
             = Acts::getDefaultLogger("WhiteBoard", Acts::Logging::INFO));
  virtual ~WhiteBoard();

  /// write to the white board
  ///
  /// @paramt coll is the collection to be written
  /// @param cname is the collection to name
  template <class T>
  ProcessCode
  writeT(T* coll, const std::string& cname);

  /// read from the white board
  ///
  /// @paramt coll is the collection to be written
  /// @param cname is the collection to name
  template <class T>
  ProcessCode
  readT(T*& coll, const std::string& cname);

private:
  std::unique_ptr<const Acts::Logger> m_logger;
  std::map<std::string, void*>        m_store;

  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

}  // namespace FW

template <class T>
FW::ProcessCode
FW::WhiteBoard::writeT(T* coll, const std::string& cname)
{
  // record the new one
  if (coll == nullptr) {
    ACTS_FATAL("Could not write collection " << cname);
    return ProcessCode::ABORT;
  }
  ACTS_VERBOSE("Writing collection " << cname << " to board");
  m_store[cname] = (void*)coll;
  return ProcessCode::SUCCESS;
}

template <class T>
FW::ProcessCode
FW::WhiteBoard::readT(T*& coll, const std::string& cname)
{
  auto sCol = m_store.find(cname);
  if (sCol == m_store.end()) {
    ACTS_FATAL("Could not read collection " << cname);
    return ProcessCode::ABORT;
  }
  // now do the static_cast
  coll = reinterpret_cast<T*>(sCol->second);
  ACTS_VERBOSE("Reading collection " << cname << " from board");
  return ProcessCode::SUCCESS;
}

#endif  // ACTFW_FRAMEWORK_WHITEBOARD_h

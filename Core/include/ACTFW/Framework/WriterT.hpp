// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @date 2017-08-07
/// @author Moritz Kiehnn <msmk@cern.ch>

#ifndef ACTFW_OBJECTWRITERT_H
#define ACTFW_OBJECTWRITERT_H

#include <memory>
#include <string>

#include <Acts/Utilities/Logger.hpp>

#include "ACTFW/Framework/IWriter.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

namespace FW {

/// A helper class for users to implement framework writers.
///
/// @note This is not an additional interface class and should not be used as
///       such, e.g. as a constrained `IWriter` substitute. This class should
///       only be used as the base class for a concrete writer implementation.
///
/// @tparam T The object type read from the event store
///
/// This class can be used when a writer reads a single object from the event
/// store and writes it to file. Reading from the event store and casting
/// to the specified type is done automatically and the user only needs to
/// implement the type-specific write method.
///
/// Default no-op implementations for `initialize` and `finalize` are provided
/// but can be overriden by the user.
template <typename T>
class WriterT : public IWriter
{
public:
  /// @param objectName The object that should be read from the event store
  /// @param writerName The name of the writer, e.g. for logging output
  /// @param level The internal log level
  WriterT(std::string          objectName,
          std::string          writerName,
          Acts::Logging::Level level);

  /// Provide the name of the writer
  std::string
  name() const final override;

  /// No-op default implementation.
  ProcessCode
  endRun() override;

  /// Read the object and call the type-specific member function.
  ProcessCode
  write(const AlgorithmContext& ctx) final override;

protected:
  /// Type-specific write function implementation
  /// this method is implemented in the user implementation
  /// @param [in] ctx is the algorithm context that guarantees event
  ///        consistency
  /// @tparam [in] is the templeted collection to be written
  virtual ProcessCode
  writeT(const AlgorithmContext& ctx, const T& t)
      = 0;

  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }

private:
  std::string                         m_objectName;
  std::string                         m_writerName;
  std::unique_ptr<const Acts::Logger> m_logger;
};

}  // namespace FW

template <typename T>
FW::WriterT<T>::WriterT(std::string          objectName,
                        std::string          writerName,
                        Acts::Logging::Level level)
  : m_objectName(std::move(objectName))
  , m_writerName(std::move(writerName))
  , m_logger(Acts::getDefaultLogger(m_writerName, level))
{
  if (m_objectName.empty()) {
    throw std::invalid_argument("Missing input collection");
  } else if (m_writerName.empty()) {
    throw std::invalid_argument("Missing writer name");
  }
}

template <typename T>
inline std::string
FW::WriterT<T>::name() const
{
  return m_writerName;
}

template <typename T>
inline FW::ProcessCode
FW::WriterT<T>::endRun()
{
  return ProcessCode::SUCCESS;
}

template <typename T>
inline FW::ProcessCode
FW::WriterT<T>::write(const AlgorithmContext& ctx)
{
  const T* object = nullptr;
  if (ctx.eventStore.get(m_objectName, object) != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;
  return writeT(ctx, *object);
}

#endif  // ACTFW_OBJECTWRITERT_H

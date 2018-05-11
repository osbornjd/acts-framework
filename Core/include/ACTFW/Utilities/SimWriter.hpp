// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

#ifdef USE_HEPMC3
#include "ACTFW/Plugins/HepMC/HepMC3Writer.hpp"
#endif

namespace FW {

/// @struct SimulatedWriter
///
/// This structure is the default structure to write runs in external
/// datatypes. It allows calling the functions that always handle primitive or
/// Acts data types.
///
template <class W, class E>
struct SimulatedWriter
{
};

namespace SimWriter {

  /// @brief Writes an event to file
  /// @param writer writer of run files
  /// @param event storage of the event
  /// @return boolean indicator if the writing was successful
  template <class W, class E>
  static bool
  writeEvent(W& writer, std::shared_ptr<E> event)
  {
    return SimulatedWriter<W, E>::readEvent(writer, event);
  }

  /// @brief Reports the status of the writer
  /// @param writer writer of run files
  /// @return boolean status indicator
  template <class W, class E>
  static bool
  status(W& writer)
  {
    return SimulatedWriter<W, E>::status(writer);
  }
}  // SimReader
}  // FW

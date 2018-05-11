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
#include "ACTFW/Plugins/HepMC/HepMC3Reader.hpp"
#endif

namespace FW {

/// @struct SimulatedReader
///
/// This structure is the default structure to read runs from external
/// datatypes. It allows calling the functions that always handle primitive or
/// Acts data types.
///
template <class R, class E>
struct SimulatedReader
{
};

namespace SimReader {

  /// @brief Reads an event from file
  /// @param reader reader of run files
  /// @param event storage of the read event
  /// @return boolean indicator if the reading was successful
  template <class R, class E>
  static bool
  readEvent(R& reader, std::shared_ptr<E> event)
  {
    return SimulatedReader<R, E>::readEvent(reader, event);
  }

  /// @brief Reports the status of the reader
  /// @param reader reader of run files
  /// @return boolean status indicator
  template <class R, class E>
  static bool
  status(R& reader)
  {
    return SimulatedReader<R, E>::status(reader);
  }
}  // SimReader
}  // FW

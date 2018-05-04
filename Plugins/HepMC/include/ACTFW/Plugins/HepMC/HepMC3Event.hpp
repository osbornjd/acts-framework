// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <fstream>
#include <iostream>
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepPID/ParticleIDMethods.hh"
#include <stdexcept>
#include "HepMC/GenParticle.h"

namespace FW {

/// @class HepMC3Event
///
/// This class inherits from HepMC::GenEvent and expands the base class by getter functions that return parameters such as vertices or particles as Acts objects. The internal behaviour of HepMC::GenEvent is not changed by this class.
///
class HepMC3Event : public HepMC::GenEvent
{
public:
  // TODO: writer for the complete file

  /// @brief Getter of the unit of momentum used
  /// @return Unit in type of Acts::units
  double 
  momentum_unit();
  
  /// @brief Getter of the unit of length used
  /// @return Unit in type of Acts::units
  double 
  length_unit();
  
  /// @brief Getter of the position of the vertex
  /// @return Vector to the location of the vertex
  Acts::Vector3D 
  event_pos();
  
  /// @brief Getter of the time of the vertex
  /// @return Time of the vertex
  double 
  event_time();
  
    /// @brief Get list of const particles
    /// @return List of const particles
    const std::vector<std::shared_ptr<Acts::ParticleProperties>> 
    particles() const;
    
    /// @brief Get list of const vertices
    /// @return List of const vertices
    const std::vector<std::shared_ptr<Acts::ProcessVertex>> 
    vertices() const;

    /// @brief Get list of particles
    /// @return List of particles
    std::vector<std::shared_ptr<Acts::ParticleProperties>> 
    particles();
    
    /// @brief Get list of vertices
    /// @return List of vertices
    std::vector<std::shared_ptr<Acts::ProcessVertex>> 
    vertices();
    
    /// @brief Get beam particles
    /// @return List of beam particles
    const std::vector<std::shared_ptr<Acts::ParticleProperties>>
    beams() const;
    
private:

	/// @brief Transform HepMC::GenParticlePtr to Acts::ParticleProperties
	/// @param genParticle particle that will be transformed
	/// @return Corresponding particle in Acts type
  Acts::ParticleProperties
  genParticleToActs(HepMC::GenParticlePtr genParticle) const;
};
}  // FW

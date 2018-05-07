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
#include "HepMC/FourVector.h"

namespace FW {

/// @class HepMC3Event
///
/// This class inherits from HepMC::GenEvent and expands the base class by getter functions that return parameters such as vertices or particles as Acts objects. The internal behaviour of HepMC::GenEvent is not changed by this class.
///
/// @note HepMC treats status codes of particles and vertices. These parameters are not a part of Acts and therefore not treated. If this class should write to a HepMC file and new added particles/vertices are involved, the status codes need to be set manually. For further information see https://hepmc.web.cern.ch/hepmc/releases/HepMC2_user_manual.pdf .
///
class HepMC3Event : public HepMC::GenEvent
{
public:
  // TODO: writer for the complete file
  // TODO: constructor is a setter!
    
    /// @brief Sets new units for momentums and lengths.
    /// @note The allowed units are MeV and Gev or mm and cm
    /// @param newMomentumUnit new unit of momentum
    /// @param newLengthUnit new unit of length
    void 
    setUnits(const double newMomentumUnit, const double newLengthUnit);
    
    /// @brief Shifts the positioning of an event in space and time
    /// @param deltaPos relative spatial shift that will be applied
    /// @param deltaTime relative time shift that will be applied
    void 
    shiftPositionBy(const Acts::Vector3D& deltaPos, const double deltaTime);
    
    /// @brief Shifts the positioning of an event in space and time
    /// @param deltaPos relative spatial shift that will be applied
    /// @param deltaTime relative time shift that will be applied
    void 
    shiftPositionTo(const Acts::Vector3D& deltaPos, const double deltaTime);
    
    /// @brief Adds a new particle
    /// @param particle new particle that will be added
    /// @param mass mass of the new particle
    /// @param status HepMC internal indicator of the particle's behaviour
    void 
    addParticle(std::shared_ptr<Acts::ParticleProperties>& particle, double mass = 0, int status = 0);

	/// @brief Adds a new vertex
	/// @param vertex new vertex that will be added
	/// @param statusVtx HepMC internal indicator of the vertex' behaviour
    /// @param statusIn HepMC internal indicator of the particle's behaviour
    /// @param statusOut HepMC internal indicator of the particle's behaviour
    void 
    addVertex(const std::shared_ptr<Acts::ProcessVertex>& vertex, int statusVtx = 0, int statusIn = 0, int statusOut = 0);
    
    void 
    removeParticle(const std::shared_ptr<Acts::ParticleProperties>& actsParticle);
    
	void 
	removeParticles(const std::vector<std::shared_ptr<Acts::ParticleProperties>>& actsParticles);

    void 
    removeVertex(const std::shared_ptr<Acts::ProcessVertex>& actsVertex);

	void 
	addTree(const std::vector<std::shared_ptr<Acts::ProcessVertex>>& actsVertices, int statusVtx, int statusIn, int statusOut);
    
        //~ /// @brief Fill GenEventData object
    //~ void write_data(GenEventData &data) const;

    //~ /// @brief Fill GenEvent based on GenEventData
    //~ void read_data(const GenEventData &data);
    //////////////////////////////////////////////////////////////////

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
  genParticleToActs(const HepMC::GenParticlePtr& genParticle) const;
  
  /// @brief Transform Acts::ParticleProperties to HepMC::GenParticlePtr
  /// @param actsParticle particle that will be transformed
  /// @param status HepMC internal indicator of the particle's behaviour
  /// @return Corresponding particle in HepMC type
  HepMC::GenParticlePtr
  ActsParticleToGen(const Acts::ParticleProperties& actsParticle, int status) const;
  
  
	HepMC::GenVertexPtr
	createGenVertex(const std::shared_ptr<Acts::ProcessVertex>& vertex, int statusVtx, int statusIn, int statusOut);

  bool
  compareVertices(const std::shared_ptr<Acts::ProcessVertex>& actsVertex, const HepMC::GenVertexPtr& genVertex);
};
}  // FW

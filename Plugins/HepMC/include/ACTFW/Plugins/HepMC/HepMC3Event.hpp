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
    
    void 
    shiftPositionBy(const Acts::Vector3D& deltaPos, const double deltaTime);
    
    void 
    shiftPositionTo(const Acts::Vector3D& deltaPos, const double deltaTime);
    
    void 
    addParticle(Acts::ParticleProperties particle, double mass = 0, int status = 0);

    void 
    addVertex(Acts::ProcessVertex vertex, int statusVtx = 0, int statusIn = 0, int statusOut = 0); //TODO: angucken, was der status beim vertex macht
    
        //~ /// @brief Remove particle from the event
    //~ ///
    //~ /// This function  will remove whole sub-tree starting from this particle
    //~ /// if it is the only incoming particle of this vertex.
    //~ /// It will also production vertex of this particle if this vertex
    //~ /// has no more outgoing particles
    //~ void remove_particle( GenParticlePtr v );
    
        //~ /// @brief Remove a set of particles
    //~ ///
    //~ /// This function follows rules of GenEvent::remove_particle to remove
    //~ /// a list of particles from the event.
    //~ void remove_particles( std::vector<GenParticlePtr> v );

    //~ /// @brief Remove vertex from the event
    //~ ///
    //~ /// This will remove all sub-trees of all outgoing particles of this vertex
    //~ /// @todo Optimize. Currently each particle/vertex is erased separately
    //~ void remove_vertex( GenVertexPtr v );

    //~ /// @brief Add whole tree in topological order
    //~ ///
    //~ /// This function will find the beam particles (particles
    //~ /// that have no production vertices or their production vertices
    //~ /// have no particles) and will add the whole decay tree starting from
    //~ /// these particles.
    //~ ///
    //~ /// @note Any particles on this list that do not belong to the tree
    //~ ///       will be ignored.
    //~ void add_tree( const std::vector<GenParticlePtr> &particles );
    
    
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
  /// @param status HepMC internal indicator if a particle decayed or not
  /// @return Corresponding particle in HepMC type
  HepMC::GenParticlePtr
  ActsParticleToGen(const Acts::ParticleProperties& actsParticle, int status) const;
};
}  // FW

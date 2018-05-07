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
#include <stdexcept>
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "HepMC/FourVector.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepPID/ParticleIDMethods.hh"

namespace FW {

/// @class HepMC3Event
///
/// This class inherits from HepMC::GenEvent and expands the base class by
/// getter functions that return parameters such as vertices or particles as
/// Acts objects. The internal behaviour of HepMC::GenEvent is not changed by
/// this class.
///
/// @note HepMC treats status codes of particles and vertices. These parameters
/// are not a part of Acts and therefore not treated. If this class should write
/// to a HepMC file and new added particles/vertices are involved, the status
/// codes need to be set manually. For further information see
/// https://hepmc.web.cern.ch/hepmc/releases/HepMC2_user_manual.pdf .
///
/// @note The data stored in Acts::ParticleProperties and Acts::ProcessVertex
/// are not identical with the corresponding HepMC classes. Therefore
/// translations between both are performed that slice or add data to an object.
/// If one or both of the Acts classes change, the translation in this class
/// needs to be properly adapted.
///
class HepMC3Event : public HepMC::GenEvent
{
public:
  /// @brief Constructor
  /// @param momentumUnit unit of momentum that will be used
  /// @param lengthUnit unit of length that will be used
  HepMC3Event(double momentumUnit = Acts::units::_GeV,
              double lengthUnit   = Acts::units::_mm);

  ///
  /// Setter
  ///

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
  
  ///
  /// Getter
  ///
  
  /// @brief Getter of the unit of momentum used
  /// @return Unit in type of Acts::units
  double
  momentumUnit() const;

  /// @brief Getter of the unit of length used
  /// @return Unit in type of Acts::units
  double
  lengthUnit() const;

  /// @brief Getter of the position of the vertex
  /// @return Vector to the location of the vertex
  Acts::Vector3D
  eventPos() const;

  /// @brief Getter of the time of the vertex
  /// @return Time of the vertex
  double
  eventTime() const;

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
  
  ///
  /// Adder
  ///

  /// @brief Adds a new particle
  /// @param particle new particle that will be added
  /// @param mass mass of the new particle
  /// @param status HepMC internal steering of the particle's behaviour
  void
  addParticle(std::shared_ptr<Acts::ParticleProperties>& particle,
              double                                     mass   = 0,
              int                                        status = 0);

  /// @brief Adds a new vertex
  /// @param vertex new vertex that will be added
  /// @param statusVtx HepMC internal steering of the vertex' behaviour
  /// @param statusIn HepMC internal steering of the behaviour of incoming
  /// particles
  /// @param statusOut HepMC internal steering of the behaviour of outgoing
  /// particles
  /// @note The statuses are not represented in Acts and therefore need to be
  /// added manually.
  void
  addVertex(const std::shared_ptr<Acts::ProcessVertex>& vertex,
            int                                         statusVtx,
            const std::map<barcode_type, int>     statusIn,
            const std::map<barcode_type, int> statusOut);

  /// @brief Adds a tree of particles and corresponding vertices to the record.
  /// @note This function needs vertices since in Acts only the vertices know
  /// the particles that enter/exit a vertex. HepMC propagates this information
  /// to the particles, too. Therefore the Acts vertices need to be translated
  /// into corresponding HepMC::GenParticles.
  /// @param actsVertices list of vertices that will be added. These vertices
  /// contain the participating particles.
  /// @param statusVtx HepMC internal steering of the vertex' behaviour
  /// @param statusIn HepMC internal steering of the behaviour of incoming
  /// particles
  /// @param statusOut HepMC internal steering of the behaviour of outgoing
  /// particles
  /// @note The statuses are not steering in Acts and therefore need to be
  /// added manually.
  void
  addTree(const std::vector<std::shared_ptr<Acts::ProcessVertex>>& actsVertices,
          const std::vector<int>                                                      statusVtx,
          const std::map<barcode_type, int>                                                      statusIn,
          const std::map<barcode_type, int> statusOut);
          
  ///          
  /// Remover
  ///

  /// @brief Removes a particle from the record
  /// @param actsParticle particle that will be removed
  void
  removeParticle(const std::shared_ptr<Acts::ParticleProperties>& actsParticle);

  /// @brief Removes multiple particles from the record
  /// @param actsParticles particles that will be removed
  void
  removeParticles(const std::vector<std::shared_ptr<Acts::ParticleProperties>>&
                      actsParticles);

  /// @brief Removes a vertex from the record
  /// @note The identification of the vertex is potentially unstable (c.f.
  /// HepMC3Event::compareVertices())
  /// @param actsVertex vertex that will be removed
  void
  removeVertex(const std::shared_ptr<Acts::ProcessVertex>& actsVertex);

private:

  /// @brief Transform HepMC::GenParticlePtr to Acts::ParticleProperties
  /// @param genParticle particle that will be transformed
  /// @return Corresponding particle in Acts type
  Acts::ParticleProperties
  genParticleToActs(const HepMC::GenParticlePtr& genParticle) const;

  /// @brief Transform Acts::ParticleProperties to HepMC::GenParticlePtr
  /// @param actsParticle particle that will be transformed
  /// @param status HepMC internal steering of the particle's behaviour
  /// @return Corresponding particle in HepMC type
  HepMC::GenParticlePtr
  actsParticleToGen(const Acts::ParticleProperties& actsParticle,
                    int                             status) const;

  /// @brief Converts an Acts vertex to a HepMC::GenVertexPtr
  /// @param actsVertex Acts vertex that will be converted
  /// @param statusVtx HepMC internal steering of the vertex' behaviour
  /// @param statusIn HepMC internal steering of the behaviour of incoming
  /// particles
  /// @param statusOut HepMC internal steering of the behaviour of outgoing
  /// particles
  /// @return Converted Acts vertex to HepMC::GenVertexPtr
  HepMC::GenVertexPtr
  createGenVertex(const std::shared_ptr<Acts::ProcessVertex>& actsVertex,
                  int                                         statusVtx,
                  const std::map<barcode_type, int>                                         statusIn,
                  const std::map<barcode_type, int> statusOut) const;

  /// @brief Compares an Acts vertex with a HepMC::GenVertex
  /// @note An Acts vertex does not store a barcode. Therefore the content of
  /// both vertices is compared. The position, time and number of incoming and
  /// outgoing particles will be compared. Since a second vertex could exist in
  /// the record with identical informations (although unlikely), this
  /// comparison could lead to false positive results. On the other hand, a
  /// numerical deviation of the parameters could lead to a false negative.
  /// @param actsVertex Acts vertex
  /// @param genVertex HepMC::GenVertex
  /// @return boolean result if both vertices are identical
  bool
  compareVertices(const std::shared_ptr<Acts::ProcessVertex>& actsVertex,
                  const HepMC::GenVertexPtr&                  genVertex) const;
};
}  // FW

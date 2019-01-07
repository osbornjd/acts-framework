// This file is part of the Acts project.
//
// Copyright (C) 2016-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Vertex.hpp"

Vertex::Vertex(){};

Vertex::Vertex(const Acts::Vector3D& position,
		   const Acts::ActsSymMatrixD<3>& covariance,
		   std::vector<std::unique_ptr<TrackAtVertex>>& tracks) :
	m_position(position),
	m_covariance(covariance),
	m_tracksAtVertex(std::move(tracks))
	{}

const Acts::Vector3D& Vertex::position() const{
	return m_position;
}

const Acts::ActsSymMatrixD<3>& Vertex::covariance() const{
	return m_covariance;
}

const std::vector<std::unique_ptr<TrackAtVertex>>& Vertex::tracks() const{
	return m_tracksAtVertex;
}

void Vertex::setPosition(const Acts::Vector3D& position){
	m_position = position;
}

void Vertex::setCovariance(const Acts::ActsSymMatrixD<3>& covariance){
	m_covariance = covariance;
}

void Vertex::setTracksAtVertex(std::vector<std::unique_ptr<TrackAtVertex>>& tracks){
	m_tracksAtVertex = std::move(tracks);
}

// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_EXAMPLES_VERTEXING_H
#define ACTFW_EXAMPLES_VERTEXING_H

#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/EventData/SimVertex.hpp"


namespace FWE {

/// @class Algorithm
/// @tparam simulator_t The Fatras simulation kernel type
/// @tparam event_collection_t  The event collection type
/// @tparam hit_t The hit Type

class VertexingAlgorithm : public FW::BareAlgorithm
{
public:
	struct Config
	{
		std::string collection; 		///< which collection to write
	};

	/// Constructor
	VertexingAlgorithm(const Config& cfg, Acts::Logging::Level level = Acts::Logging::INFO);

	/// Framework execute method
	/// @param [in] context is the Algorithm context for event consistency
	FW::ProcessCode
	execute(FW::AlgorithmContext context) const final override;

private:
	/// The config class
	Config m_cfg;

};

} // namespace FWE


#endif // ACTFW_EXAMPLES_VERTEXING_H
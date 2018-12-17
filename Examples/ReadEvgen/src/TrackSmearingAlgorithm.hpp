// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once 

#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"


namespace FWE {

/// @class Algorithm
/// @tparam simulator_t The Fatras simulation kernel type
/// @tparam event_collection_t  The event collection type
/// @tparam hit_t The hit Type

class TrackSmearingAlgorithm : public FW::BareAlgorithm
{
public:
	struct Config
	{
		std::string collection; 						///< Input particle collection
		std::string collectionOut = "paramCollection"; 	///< Collection of smeared output track parameter
		std::shared_ptr<FW::RandomNumbersSvc> randomNumberSvc = nullptr;
	};

	/// Constructor
	TrackSmearingAlgorithm(const Config& cfg, Acts::Logging::Level level = Acts::Logging::INFO);

	/// Framework execute method
	/// @param [in] context is the Algorithm context for event consistency
	FW::ProcessCode
	execute(FW::AlgorithmContext context) const final override;

private:
	/// The config class
	Config m_cfg;

};

} // namespace FWE
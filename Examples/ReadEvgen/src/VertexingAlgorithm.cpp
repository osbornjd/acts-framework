// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "VertexingAlgorithm.hpp"
#include <iostream>


FWE::VertexingAlgorithm::VertexingAlgorithm(const Config& cfg, Acts::Logging::Level level)
  : FW::BareAlgorithm("Vertexing", level), m_cfg(cfg)
{
}

FW::ProcessCode
FWE::VertexingAlgorithm::execute(FW::AlgorithmContext context) const
{
	const std::vector<FW::Data::SimVertex<>>* inputEvent = nullptr;
	if (context.eventStore.get(m_cfg.collection, inputEvent) == FW::ProcessCode::ABORT)
	{
    	return FW::ProcessCode::ABORT;
	}

	std::cout <<  "Read collection " << m_cfg.collection << " with "
                                 << inputEvent->size()
                                 << " vertices" << std::endl;
	return FW::ProcessCode::SUCCESS;
}
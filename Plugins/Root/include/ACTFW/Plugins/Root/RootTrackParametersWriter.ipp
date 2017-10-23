// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

template <class T>
FW::ProcessCode
FW::Root::RootTrackParametersWriter<T>::writeT(
    const FW::AlgorithmContext& ctx,
    const std::vector<T>&       tParameterCollections)
{

  // exclusive access to the tree
  std::lock_guard<std::mutex> lock(m_writeMutex);

  m_tag.clear();
  m_par0.clear();
  m_par1.clear();
  m_par2.clear();
  m_par3.clear();
  m_par4.clear();
  m_cov00.clear();
  m_cov01.clear();
  m_cov02.clear();
  m_cov03.clear();
  m_cov04.clear();
  m_cov11.clear();
  m_cov12.clear();
  m_cov13.clear();
  m_cov14.clear();
  m_cov22.clear();
  m_cov23.clear();
  m_cov24.clear();
  m_cov33.clear();
  m_cov34.clear();
  m_cov44.clear();
  m_positionX.clear();
  m_positionY.clear();
  m_positionZ.clear();
  m_positionR.clear();
  m_momentumPhi.clear();
  m_momentumEta.clear();
  m_momentumPt.clear();

  // loop over all the extrapolation cells
  for (auto& tpCollection : tParameterCollections) {
    int tag = 0;
    for (auto& tp : tpCollection) {
      if (tp) {
        // fill the tag
        m_tag.push_back(tag++);
        // get the parameters
        m_par0.push_back(tp->parameters()[Acts::eLOC_0]);
        m_par1.push_back(tp->parameters()[Acts::eLOC_1]);
        m_par2.push_back(tp->parameters()[Acts::ePHI]);
        m_par3.push_back(tp->parameters()[Acts::eTHETA]);
        m_par4.push_back(tp->parameters()[Acts::eQOP]);
        // get the poistion
        m_positionX.push_back(tp->position().x());
        m_positionY.push_back(tp->position().y());
        m_positionZ.push_back(tp->position().z());
        m_positionR.push_back(tp->position().perp());
        // the momentum
        m_momentumPhi.push_back(tp->momentum().phi());
        m_momentumEta.push_back(tp->momentum().eta());
        m_momentumPt.push_back(tp->momentum().perp());
        // let's fill the covariances
        if (tp->covariance()) {
          // covariance
          const auto& cov = (*(tp->covariance()));
          m_cov00.push_back(cov(0, 0));
          m_cov01.push_back(cov(0, 1));
          m_cov02.push_back(cov(0, 2));
          m_cov03.push_back(cov(0, 3));
          m_cov04.push_back(cov(0, 4));
          m_cov11.push_back(cov(1, 1));
          m_cov12.push_back(cov(1, 2));
          m_cov13.push_back(cov(1, 3));
          m_cov14.push_back(cov(1, 4));
          m_cov22.push_back(cov(2, 2));
          m_cov23.push_back(cov(2, 3));
          m_cov24.push_back(cov(2, 4));
          m_cov33.push_back(cov(3, 3));
          m_cov34.push_back(cov(3, 4));
          m_cov44.push_back(cov(4, 4));
        }
      }
    }
    m_outputTree->Fill();
  }

  // return scuess
  return FW::ProcessCode::SUCCESS;
}

template <class T>
FW::Root::RootTrackParametersWriter<T>::RootTrackParametersWriter(
    const FW::Root::RootTrackParametersWriter<T>::Config& cfg,
    Acts::Logging::Level                                  level)
  : FW::WriterT<std::vector<T>>(cfg.collection,
                                "RootTrackParametersWriter",
                                level)
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
  m_outputFile = TFile::Open(m_cfg.filePath.c_str(), m_cfg.fileMode.c_str());
  m_outputFile->cd();
  m_outputTree
      = new TTree(m_cfg.treeName.c_str(), "TTree from TrackParametersWriter");
  // the brances
  // - the tag
  m_outputTree->Branch("tag", &m_tag);
  // - the parameters
  m_outputTree->Branch("parameter0", &m_par0);
  m_outputTree->Branch("parameter1", &m_par1);
  m_outputTree->Branch("parameter2", &m_par2);
  m_outputTree->Branch("parameter3", &m_par3);
  m_outputTree->Branch("parameter4", &m_par4);
  // - the covariance entries
  m_outputTree->Branch("covariance00", &m_cov00);
  m_outputTree->Branch("covariance01", &m_cov01);
  m_outputTree->Branch("covariance02", &m_cov02);
  m_outputTree->Branch("covariance03", &m_cov03);
  m_outputTree->Branch("covariance04", &m_cov04);
  m_outputTree->Branch("covariance11", &m_cov11);
  m_outputTree->Branch("covariance12", &m_cov12);
  m_outputTree->Branch("covariance13", &m_cov13);
  m_outputTree->Branch("covariance14", &m_cov14);
  m_outputTree->Branch("covariance22", &m_cov22);
  m_outputTree->Branch("covariance23", &m_cov23);
  m_outputTree->Branch("covariance24", &m_cov24);
  m_outputTree->Branch("covariance33", &m_cov33);
  m_outputTree->Branch("covariance34", &m_cov34);
  m_outputTree->Branch("covariance44", &m_cov44);
  // - the position information
  m_outputTree->Branch("positionX", &m_positionX);
  m_outputTree->Branch("positionY", &m_positionY);
  m_outputTree->Branch("positionZ", &m_positionZ);
  m_outputTree->Branch("positionR", &m_positionR);
  // - the momentum information
  m_outputTree->Branch("momentumPhi", &m_momentumPhi);
  m_outputTree->Branch("momentumEta", &m_momentumEta);
  m_outputTree->Branch("momentumPt", &m_momentumPt);
}

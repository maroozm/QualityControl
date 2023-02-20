// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file   TrackingTask.cxx
/// \author My Name
///

#include <TCanvas.h>
#include <TH1.h>
#include <TMath.h>

#include "QualityControl/QcInfoLogger.h"
#include "DataFormatsTRD/TrackTRD.h"
#include "DataFormatsTRD/TrackTriggerRecord.h"
#include "DataFormatsTRD/Constants.h"
#include "DataFormatsTRD/Tracklet64.h"
#include "DataFormatsGlobalTracking/RecoContainer.h"
#include "DataFormatsTRD/CalibratedTracklet.h"
#include "TRDQC/Tracking.h"
#include "TRD/TrackingTask.h"
#include <Framework/InputRecord.h>
#include <Framework/InputRecordWalker.h>
#include <Framework/DataRefUtils.h>
#include <Framework/DataSpecUtils.h>
#include "CCDB/BasicCCDBManager.h"

using namespace o2::trd;
using namespace o2::trd::constants;

namespace o2::quality_control_modules::trd
{

TrackingTask::~TrackingTask()
{
  delete mTracksEta;
  delete mTracksPhi;
  delete mTracksPt;
  delete mTracksChi2;
  delete mNtracks;
  delete mNtracklets;
  delete mTracksEtaPhiPerLayer[NLAYER];
  delete mTracksEtaPhi;
  delete mTrackletsEtaPhi;
  delete mResidualsY;
  delete mResidualsZ;
  delete mResidualsYDet;
}

void TrackingTask::retrieveCCDBSettings()
{
  if (auto param = mCustomParameters.find("ccdbtimestamp"); param != mCustomParameters.end()) {
    mTimestamp = std::stol(mCustomParameters["ccdbtimestamp"]);
    ILOG(Info, Support) << "configure() : using ccdbtimestamp = " << mTimestamp << ENDM;
  } else {
    mTimestamp = o2::ccdb::getCurrentTimestamp();
    ILOG(Info, Support) << "configure() : using default timestam of now = " << mTimestamp << ENDM;
  }
}

void TrackingTask::initialize(o2::framework::InitContext& /*ctx*/)
{
  ILOG(Info, Support) << "initialize TRD TrackingTask" << ENDM;

  retrieveCCDBSettings();
  buildHistograms();
  publishHistograms();

}

void TrackingTask::startOfActivity(Activity& activity)
{
  ILOG(Info, Support) << "startOfActivity " << activity.mId << ENDM;
}

void TrackingTask::startOfCycle()
{
  ILOG(Info, Support) << "startOfCycle" << ENDM;
}

void TrackingTask::monitorData(o2::framework::ProcessingContext& ctx)
{
  ILOG(Info, Support) << "monitorData" << ENDM;

  auto trackArr = ctx.inputs().get<gsl::span<o2::trd::TrackQC>>("tracks");
  auto tracktrigArr = ctx.inputs().get<gsl::span<o2::trd::TrackTriggerRecord>>("trigrectrk");
  
  for (int itrt = 0; itrt < tracktrigArr.size(); itrt++)
  {
    int first = tracktrigArr[itrt].getFirstTrack();
    int end = first + tracktrigArr[itrt].getNumberOfTracks();
    mNtracks->Fill(tracktrigArr[itrt].getNumberOfTracks());
    for (int itrk = first; itrk < end; itrk++)
    {
      mTracksPt->Fill(trackArr[itrk].pt);
      mTracksChi2->Fill(trackArr[itrk].chi2);
      mNtracklets->Fill(trackArr[itrk].nTracklets);
     // if(!(trackArr[itrk].hasPadrowCrossing)) continue;
      for(int iLayer = 0; iLayer < NLAYER; iLayer++){
       // if(trackArr[itrk].trackletIndex[iLayer] != -1)
       if(trackArr[itrk].findable[iLayer] && trackArr[itrk].trackX[iLayer] > 10) 
        {
          mTracksEta->Fill(trackArr[itrk].trackEta[iLayer]);
          mTracksPhi->Fill(trackArr[itrk].trackPhi[iLayer]);
          mTrackletsChi2->Fill(trackArr[itrk].trackletChi2[iLayer]);
          mTracksEtaPhiPerLayer[iLayer]->Fill(trackArr[itrk].trackEta[iLayer], trackArr[itrk].trackPhi[iLayer]);
          mTracksEtaPhi->Fill(trackArr[itrk].trackEta[iLayer], trackArr[itrk].trackPhi[iLayer]);
          mResidualsY->Fill(trackArr[itrk].trackY[iLayer] - trackArr[itrk].trackletY[iLayer]);
          mResidualsYDet->Fill(trackArr[itrk].trackletDet[iLayer], trackArr[itrk].trackY[iLayer] - trackArr[itrk].trackletY[iLayer]);
          mResidualsZ->Fill(trackArr[itrk].trackZ[iLayer] - trackArr[itrk].trackletZ[iLayer]);
          mTrackletsEtaPhi->Fill(trackArr[itrk].trackEta[iLayer], trackArr[itrk].trackPhi[iLayer], trackArr[itrk].nTracklets);
        }
      }
    }
  }

}

void TrackingTask::endOfCycle()
{
  ILOG(Info, Support) << "endOfCycle" << ENDM;
}

void TrackingTask::endOfActivity(Activity& /*activity*/)
{
  ILOG(Info, Support) << "endOfActivity" << ENDM;
}

void TrackingTask::reset()
{
 ILOG(Info, Support) << "Resetting the histogram" << ENDM;
  mTracksEta->Reset();
  mTracksPhi->Reset();
  mTracksPt->Reset();
  mTracksChi2->Reset();
  mTrackletsChi2->Reset();
  mNtracks->Reset();
  mNtracklets->Reset();
  for (auto h : mTracksEtaPhiPerLayer) {
    h->Reset();
  }
  mTracksEtaPhi->Reset();
  mTrackletsEtaPhi->Reset();
  mResidualsY->Reset();
  mResidualsZ->Reset();
  mResidualsYDet->Reset();
}

void TrackingTask::buildHistograms()
{
  mNtracks = new TH1F("tracks", "Tracks; # of tracks; counts", 100, 0.0f, 100.0f);
  addObject(mNtracks);
  mNtracks->SetStats(0);
  mNtracklets = new TH1F("tracklets", "Tracklets; # of tracklets; counts", 100, 0.0f, 100.0f);
  addObject(mNtracklets);
  mNtracklets->SetStats(0);
  mTracksEta = new TH1F("tracksEta", "EtaDistribution; #eta; counts", 40, -2.0f, 2.0f);
  addObject(mTracksEta);
  mTracksEta->SetStats(0);
  mTracksPhi = new TH1D("tracksPhi", "PhiDistribution; #phi; counts", 60, 0, TMath::TwoPi());
  addObject(mTracksPhi);
  mTracksPhi->SetStats(0);
   mTracksEtaPhi = new TH2F("tracksEtaPhi", "EtaPhiDistribution; #eta; #phi; counts", 20, -0.81f, 0.81f, 60, 0, TMath::TwoPi());
  addObject(mTracksEtaPhi);
  mTracksEtaPhi->SetStats(0);
  for (int ilyr = 0; ilyr < NLAYER; ilyr++)
  {
  mTracksEtaPhiPerLayer[ilyr] = new TH2F(Form("tracksEtaPhilayer%i", ilyr), Form("EtaPhiDistribution%i; #eta; #phi", ilyr), 20, -1.0f, 1.0f, 60, 0, TMath::TwoPi());
  addObject(mTracksEtaPhiPerLayer[ilyr]);
  mTracksEtaPhiPerLayer[ilyr]->SetStats(0);
  }
  mTracksPt = new TH1F("tracksPt", "pTDistribution; p_{T} (GeV/c); counts", 100, 0.0f, 10.0f);
  addObject(mTracksPt);
  mTracksPt->SetStats(0);
  mTracksChi2 = new TH1F("tracksChi2", "Chi2Distribution; #chi^{2}; counts", 100, 0.0f, 100.0f);
  addObject(mTracksChi2);
  mTracksChi2->SetStats(0);
  mTrackletsChi2 = new TH1F("trackletsChi2", "Chi2Distribution; #chi^{2}; counts", 100, 0.0f, 100.0f);
  addObject(mTrackletsChi2);
  mTrackletsChi2->SetStats(0);
  mTrackletsEtaPhi = new TProfile2D("trackletEtaPhi", "EtaPhiDistribution; #eta; #phi; Average # of tracklets per track", 20, -0.81f, 0.81f, 60, 0, TMath::TwoPi(), 0, 6);
  addObject(mTrackletsEtaPhi);
  mTrackletsEtaPhi->SetStats(0);
  mResidualsY = new TH1F("residualsY", "ResidualsY; #Delta y (cm); counts", 200, -10.0f, 10.f);
  addObject(mResidualsY);
  mResidualsY->SetStats(0);
  mResidualsZ = new TH1F("residualsZ", "ResidualsZ; #Delta z (cm); counts", 200, -10.0f, 10.f);
  addObject(mResidualsZ);
  mResidualsZ->SetStats(0);
  mResidualsYDet = new TH2F("residualsYDet", "ResidualsY vs chambers; chamber number; #Delta y (cm); counts", 540, 0, 540, 200, -10.0f, 10.f);
  addObject(mResidualsYDet);
  mResidualsYDet->SetStats(0);
  
  
}

void TrackingTask::addObject(TObject* aObject)
{
  if (!aObject) {
    ILOG(Info, Support) << " ERROR: trying to add a non-existent histogram " << ENDM;
    return;
  } else {
    mPublishedObjects.push_back(aObject);
  }
}


void TrackingTask::publishHistograms()
{
  for (unsigned int nobj = 0; nobj < mPublishedObjects.size(); nobj++) {
    getObjectsManager()->startPublishing(mPublishedObjects.at(nobj));
    ILOG(Info, Support) << " Object will be published: " << mPublishedObjects.at(nobj)->GetName() << ENDM;
  }
}

} // namespace o2::quality_control_modules::trd

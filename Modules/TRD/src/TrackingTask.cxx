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
  delete mHistogram;
  delete mTracksEta;
  delete mTracksPhi;
  delete mTracksPt;
  delete mTracksChi2;
  delete mNtracks;
  delete mNtracklets;
  delete mTracksEtaPhi;

  delete mTracksEtatpc;
  delete mTracksPhitpc;
  delete mTracksPttpc;
  delete mTracksChi2tpc;
  delete mNtrackstpc;
  delete mNtrackletstpc;
  delete mTracksEtaPhitpc;

  delete mTrackletsEtaPhi;
  delete mTrackletsEtaPhi2;
  delete mResidualsX;
  delete mResidualsY;
  delete mResidualsZ;
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

 // mRecoCont.collectData(ctx, *mDataRequest.get());

  auto trackArr = ctx.inputs().get<gsl::span<o2::trd::TrackQC>>("tracks");
 // auto tracktpcArr = ctx.inputs().get<gsl::span<o2::trd::TrackTRD>>("trackstpc");
//  auto tracksArrtpc = ctx.inputs().get<gsl::span<o2::trd::TrackTRD>>("trackstpc");
 // auto trackletArr = ctx.inputs().get<gsl::span<o2::globaltracking::RecoContainer>>("tracklets");
//  auto trackletArr = ctx.inputs().get<gsl::span<o2::trd::CalibratedTracklet>>("tracklets");
  auto tracktrigArr = ctx.inputs().get<gsl::span<o2::trd::TrackTriggerRecord>>("trigrectrk");
//  auto tracktrigtpcArr = ctx.inputs().get<gsl::span<o2::trd::TrackTriggerRecord>>("trigrectrktpc");

/*
for (int itrttpc = 0; itrttpc < tracktrigtpcArr.size(); itrttpc++)
  {
    int first = tracktrigtpcArr[itrttpc].getFirstTrack();
    int end = first + tracktrigtpcArr[itrttpc].getNumberOfTracks();
    mNtrackstpc->Fill(tracktrigtpcArr[itrttpc].getNumberOfTracks());
    for (int itrk = first; itrk < end; itrk++)
    {
      mTracksEtatpc->Fill(tracktpcArr[itrk].getEta());
      mTracksPhitpc->Fill(tracktpcArr[itrk].getPhi());
      mTracksPttpc->Fill(tracktpcArr[itrk].getPt());
      mTracksChi2tpc->Fill(tracktpcArr[itrk].getChi2());
      mTracksEtaPhitpc->Fill(tracktpcArr[itrk].getEta(), tracktpcArr[itrk].getPhi());
      mNtrackletstpc->Fill(tracktpcArr[itrk].getNtracklets());

    }
  }
*/
  
  for (int itrt = 0; itrt < tracktrigArr.size(); itrt++)
  {
    int first = tracktrigArr[itrt].getFirstTrack();
    int end = first + tracktrigArr[itrt].getNumberOfTracks();
    mNtracks->Fill(tracktrigArr[itrt].getNumberOfTracks());
    for (int itrk = first; itrk < end; itrk++)
    {
      mTracksEta->Fill(trackArr[itrk].getEta());
      mTracksPhi->Fill(trackArr[itrk].getPhi());
      mTracksPt->Fill(trackArr[itrk].getPt());
      mTracksChi2->Fill(trackArr[itrk].getChi2());
      mTracksEtaPhi->Fill(trackArr[itrk].getEta(), trackArr[itrk].getPhi());
      mNtracklets->Fill(trackArr[itrk].getNtracklets());

    }
  }

 // mTracksChi2->Fill(tracktrigArr.size());

 // auto tracklet64 = trackletArr.getTRDTracklets();
 //mTrackletsCalib = mRecoCont.getTRDCalibratedTracklets();
 //int mTrackletsc = trackletArr.getCalibratedTracklets();
 /*  for (int itrt = 0; itrt < tracktrigArr.size(); itrt++)
  {
    int first = tracktrigArr[itrt].getFirstTrack();
    int end = first + tracktrigArr[itrt].getNumberOfTracks();
    mNtracks->Fill(tracktrigArr[itrt].getNumberOfTracks());
    for (int itrk = first; itrk < end; itrk++)
    {
     // mNtracks->Fill(trackArr.size());
     /*  mTracksEta->Fill(trackArr[itrk].getEta());
      mTracksPhi->Fill(trackArr[itrk].getPhi());
      mTracksPt->Fill(trackArr[itrk].getPt());
      mTracksChi2->Fill(trackArr[itrk].getChi2()); */
     /* mTracksEtaPhi->Fill(trackArr[itrk].getEta(), trackArr[itrk].getPhi());
      mNtracklets->Fill(trackArr[itrk].getNtracklets());

      for (int ilyr = 0; ilyr < NLAYER; ilyr++)
      {
        int trklet = trackArr[itrk].getTrackletIndex(ilyr);
       // if (trackArr[itrk].getTrackletIndex(ilyr) > -1)
       if (ilyr == 0)
        {
          mResidualsX->Fill(trackArr[itrk].getX()-trackletArr[trklet].getX());
          mResidualsY->Fill(trackArr[itrk].getY()-trackletArr[trklet].getY());
          mResidualsZ->Fill(trackArr[itrk].getZ()-trackletArr[trklet].getZ());
          mTracksEta->Fill(trackArr[itrk].getEta());
          mTracksPhi->Fill(trackArr[itrk].getPhi());
          mTracksPt->Fill(trackArr[itrk].getPt());
          mTracksChi2->Fill(trackArr[itrk].getChi2());
          mTrackletsEtaPhi->Fill(trackArr[itrk].getEta(), trackArr[itrk].getPhi());
         // mTrackletsEtaPhi->Fill(trackArr[itrk].getEta(), trackArr[itrk].getPhi());
        }
        if (ilyr == 1)
        {
          mTrackletsEtaPhi2->Fill(trackArr[itrk].getEta(), trackArr[itrk].getPhi());
        }
        
      }
      
    } 
  }*/
  
  /* mNtracks->Fill(tracks.size());
  for (auto& track : tracks) {
    mTracksEta->Fill(track.getEta());
    mTracksPhi->Fill(track.getPhi());
    mTracksPt->Fill(track.getPt());
    mTracksChi2->Fill(track.getChi2());
    mTracksEtaPhi->Fill(track.getEta(), track.getPhi());
    mNtracklets->Fill(track.getNtracklets());
  } */
 /*  for (auto&& input : ctx.inputs()) {
    if (input.spec->binding == "tracks") {
      auto tracks = ctx.inputs().get<gsl::span<o2::trd::TrackTRD>>("tracks");
      for (auto& track : tracks) {
        mTracksEta->Fill(track.getEta());
        mTracksPhi->Fill(track.getPhi());
        mTracksPt->Fill(track.getPt());
      }
    }
  } */
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
  mNtracks->Reset();
  mNtracklets->Reset();
  mTracksEtaPhi->Reset();
}

void TrackingTask::buildHistograms()
{
  mNtracks = new TH1F("tracks", "Tracks; # of tracks; counts", 100, 0.0f, 100.0f);
  addObject(mNtracks);
  mNtrackstpc = new TH1F("trackstpc", "TracksTPC; # of tracks; counts", 100, 0.0f, 100.0f);
  addObject(mNtrackstpc);
  
  mNtracklets = new TH1F("tracklets", "Tracklets; # of tracklets; counts", 100, 0.0f, 100.0f);
  addObject(mNtracklets);
  mNtrackletstpc = new TH1F("trackletstpc", "TrackletsTPC; # of tracklets; counts", 100, 0.0f, 100.0f);
  addObject(mNtrackletstpc);

  mTracksEta = new TH1F("tracksEta", "EtaDistribution; #eta; counts", 40, -2.0f, 2.0f);
  addObject(mTracksEta);
  mTracksEtatpc = new TH1F("tracksEtatpc", "EtaDistributionTPC; #eta; counts", 40, -2.0f, 2.0f);
  addObject(mTracksEtatpc);
  mTracksPhi = new TH1D("tracksPhi", "PhiDistribution; #phi; counts", 60, 0, TMath::TwoPi());
  addObject(mTracksPhi);
  mTracksPhitpc = new TH1D("tracksPhitpc", "PhiDistributionTPC; #phi; counts", 60, 0, TMath::TwoPi());
  addObject(mTracksPhitpc);
  mTracksPt = new TH1F("tracksPt", "pTDistribution; p_{T} (GeV/c); counts", 100, 0.0f, 10.0f);
  addObject(mTracksPt);
  mTracksPttpc = new TH1F("tracksPttpc", "pTDistributionTPC; p_{T} (GeV/c); counts", 100, 0.0f, 10.0f);
  addObject(mTracksPttpc);
  mTracksChi2 = new TH1F("tracksChi2", "Chi2Distribution; #chi^{2}; counts", 100, 0.0f, 100.0f);
  addObject(mTracksChi2);
  mTracksChi2tpc = new TH1F("tracksChi2tpc", "Chi2DistributionTPC; #chi^{2}; counts", 100, 0.0f, 100.0f);
  addObject(mTracksChi2tpc);
  mTracksEtaPhi = new TH2F("tracksEtaPhi", "EtaPhiDistribution; #eta; #phi", 40, -2.0f, 2.0f, 60, 0, TMath::TwoPi());
  addObject(mTracksEtaPhi);
  mTracksEtaPhitpc = new TH2F("tracksEtaPhitpc", "EtaPhiDistributionTPC; #eta; #phi", 40, -2.0f, 2.0f, 60, 0, TMath::TwoPi());
  addObject(mTracksEtaPhitpc);


  mTrackletsEtaPhi = new TH2F("trackletEtaPhi", "EtaPhiDistribution; #eta; #phi", 40, -2.0f, 2.0f, 60, 0, TMath::TwoPi());
  addObject(mTrackletsEtaPhi);
  mTrackletsEtaPhi2 = new TH2F("trackletEtaPhi2", "EtaPhiDistribution; #eta; #phi", 40, -2.0f, 2.0f, 60, 0, TMath::TwoPi());
  addObject(mTrackletsEtaPhi2);
  mResidualsX = new TH1F("residualsX", "ResidualsX; #Delta x (cm); counts", 200, -10.0f, 10.f);
  addObject(mResidualsX);
  mResidualsY = new TH1F("residualsY", "ResidualsY; #Delta y (cm); counts", 200, -10.0f, 10.f);
  addObject(mResidualsY);
  mResidualsZ = new TH1F("residualsZ", "ResidualsZ; #Delta z (cm); counts", 200, -10.0f, 10.f);
  addObject(mResidualsZ);
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

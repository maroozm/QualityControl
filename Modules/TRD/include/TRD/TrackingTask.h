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
/// \file   TrackingTask.h
/// \author My Name
///

#ifndef QC_MODULE_TRD_TRDTRACKINGTASK_H
#define QC_MODULE_TRD_TRDTRACKINGTASK_H

#include "QualityControl/TaskInterface.h"
#include "TH1D.h"
#include "TH2F.h"
#include <Framework/TimingInfo.h>

class TH1F;
class TH2F;
class TH1D;

using namespace o2::quality_control::core;

namespace o2::quality_control_modules::trd
{

/// \brief Example Quality Control DPL Task
/// \author My Name
class TrackingTask final : public TaskInterface
{
 public:
  /// \brief Constructor
  TrackingTask() = default;
  /// Destructor
  ~TrackingTask() override;

  // Definition of the methods for the template method pattern
  void retrieveCCDBSettings();
  void initialize(o2::framework::InitContext& ctx) override;
  void startOfActivity(Activity& activity) override;
  void startOfCycle() override;
  void monitorData(o2::framework::ProcessingContext& ctx) override;
  void endOfCycle() override;
  void endOfActivity(Activity& activity) override;
  void reset() override;

 private:
 void buildHistograms();
 void publishHistograms();
  void addObject(TObject* aObject);
  long int mTimestamp;
    std::vector<TObject*> mPublishedObjects;

   // o2::globaltracking::RecoContainer mRecoCont;
 //    gsl::span<const o2::trd::CalibratedTracklet> mTrackletsCalib; // tracklets

  TH1F* mHistogram = nullptr;
  TH1F* mTracksEta;
  TH1D* mTracksPhi;
  TH1F* mTracksPt;
  TH1F* mTracksChi2;
  TH1F* mNtracks;
  TH1F* mNtracklets;
  TH2F* mTracksEtaPhi;

  TH1F* mTracksEtatpc;
  TH1D* mTracksPhitpc;
  TH1F* mTracksPttpc;
  TH1F* mTracksChi2tpc;
  TH1F* mNtrackstpc;
  TH1F* mNtrackletstpc;
  TH2F* mTracksEtaPhitpc;

  TH2F* mTrackletsEtaPhi;
  TH2F* mTrackletsEtaPhi2;
  TH1F* mResidualsX;
  TH1F* mResidualsY;
  TH1F* mResidualsZ;
};

} // namespace o2::quality_control_modules::trd

#endif // QC_MODULE_TRD_TRDTRACKINGTASK_H

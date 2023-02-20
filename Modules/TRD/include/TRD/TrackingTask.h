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
#include "TProfile2D.h"
#include <Framework/TimingInfo.h>

class TH1F;
class TH2F;
class TH1D;
class TProfile2D;

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

    static constexpr int NLayers = 6;


  TH1F* mTracksEta = nullptr;
  TH1D* mTracksPhi = nullptr;
  TH1F* mTracksPt = nullptr;
  TH1F* mTracksChi2 = nullptr;
  TH1F* mTrackletsChi2 = nullptr;
  TH1F* mNtracks = nullptr;
  TH1F* mNtracklets = nullptr;
  TH2F* mTracksEtaPhi = nullptr;
  TProfile2D* mTrackletsEtaPhi = nullptr;
  TH1F* mResidualsY = nullptr;
  TH1F* mResidualsZ = nullptr;
  TH2F* mResidualsYDet = nullptr;
  TH1F* mResidualsZDet = nullptr;

  std::array<TH2F*, NLayers> mTracksEtaPhiPerLayer;
};

} // namespace o2::quality_control_modules::trd

#endif // QC_MODULE_TRD_TRDTRACKINGTASK_H

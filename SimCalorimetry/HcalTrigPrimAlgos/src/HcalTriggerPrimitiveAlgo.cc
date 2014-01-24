#include "SimCalorimetry/HcalTrigPrimAlgos/interface/HcalTriggerPrimitiveAlgo.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "CalibFormats/CaloTPG/interface/CaloTPGTranscoder.h"
#include "CalibFormats/CaloTPG/interface/CaloTPGRecord.h"

#include <iostream>
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "Geometry/HcalTowerAlgo/interface/HcalTrigTowerGeometry.h"
#include "DataFormats/HcalDetId/interface/HcalTrigTowerDetId.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/HcalDetId/interface/HcalElectronicsId.h"
#include "EventFilter/HcalRawToDigi/interface/HcalDCCHeader.h"
#include "EventFilter/HcalRawToDigi/interface/HcalHTRData.h"

using namespace std;

std::pair<double, double>
timing(const HcalUpgradeDataFrame& frame) {
   int n = frame.size();
   double ft = -999.;
   double rt = -999.;
   int sig_bx = frame.presamples();
   int dir = -1;
   int step = 1;
   int i = sig_bx;
   int nbins = 50;

   while ((i > 2) && (i < frame.size() - 2) && (i < n) && ((rt < -998.) || (ft < 998.))) {
      unsigned rise = frame.tdc(i) % 100;
      unsigned fall = frame.tdc(i) / 100;

      if (rt < -998. && rise != 62 && rise != 63) {
         rt = rise * 25. / nbins + (i - sig_bx) * 25.;
      }
      if (((ft < -998.) || (ft < rt)) && 
            (fall != 62) && (fall != 63)) {
         ft = fall * 25. / nbins + (i - sig_bx) * 25.;
      }

      i += dir * step;
      ++step;
      dir *= -1;
   }

   /* if (rt > -998 or ft > -998) */
   /*    std::cout << "rise " << rt << " fall " << ft << std::endl; */

   return {rt, ft};
}

void
update(HcalUpgradeTriggerPrimitiveDigi& digi, const Sample& sample, int soi)
{
   std::vector<double> rise_avg;
   std::vector<double> rise_rms;
   std::vector<double> fall_avg;
   std::vector<double> fall_rms;
   std::vector<int> linearized;
   std::vector<int> oot;

   for (int i = 0; i < 5; ++i) {
      auto rise = sample.rise(i);
      if (rise.size() > 0) {
         double avg = std::accumulate(rise.begin(), rise.end(), 0.) / rise.size();
         double sqrs = std::accumulate(rise.begin(), rise.end(), 0., [](double x, double y) { return x + y * y; });

         rise_avg.push_back(avg);
         rise_rms.push_back(sqrt(sqrs / rise.size() - avg * avg));
      } else {
         rise_avg.push_back(-1e6);
         rise_rms.push_back(-1e6);
      }

      auto fall = sample.fall(i);
      if (fall.size() > 0) {
         double avg = std::accumulate(fall.begin(), fall.end(), 0.) / fall.size();
         double sqrs = std::accumulate(fall.begin(), fall.end(), 0., [](double x, double y) { return x + y * y; });

         fall_avg.push_back(avg);
         fall_rms.push_back(sqrt(sqrs / fall.size() - avg * avg));
      } else {
         fall_avg.push_back(-1e6);
         fall_rms.push_back(-1e6);
      }

      /* std::cout << "rise "; */
      /* for (const auto& v: rise) */
      /*    std::cout << v << " "; */
      /* std::cout << "-> " << rise_avg.back() << std::endl; */
      /* std::cout << "fall "; */
      /* for (const auto& v: fall) */
      /*    std::cout << v << " "; */
      /* std::cout << "-> " << fall_avg.back() << std::endl; */

      linearized.push_back(sample[i][soi]);
      oot.push_back(sample(i)[soi]);
   }

   digi.setDepthData(linearized);
   digi.setOOTData(oot);
   digi.setTimingData(rise_avg, rise_rms, fall_avg, fall_rms);
}

HcalTriggerPrimitiveAlgo::HcalTriggerPrimitiveAlgo( bool pf, const std::vector<double>& w, int latency,
                                                    uint32_t FG_threshold, uint32_t ZS_threshold,
                                                    int numberOfSamples, int numberOfPresamples,
                                                    uint32_t minSignalThreshold, uint32_t PMT_NoiseThreshold,
                                                    bool upgrade)
                                                   : incoder_(0), outcoder_(0), coder_(0),
                                                   theThreshold(0), peakfind_(pf), weights_(w), latency_(latency),
                                                   FG_threshold_(FG_threshold), ZS_threshold_(ZS_threshold),
                                                   numberOfSamples_(numberOfSamples),
                                                   numberOfPresamples_(numberOfPresamples),
                                                   minSignalThreshold_(minSignalThreshold),
                                                   PMT_NoiseThreshold_(PMT_NoiseThreshold),
                                                   upgrade_(upgrade),
                                                   sample_mask_(upgrade ? 0xFFFF : 0x3FF),
                                                   peak_finder_algorithm_(2)
{
   //No peak finding setting (for Fastsim)
   if (!peakfind_){
      numberOfSamples_ = 1; 
      numberOfPresamples_ = 0;
   }
   // Switch to integer for comparisons - remove compiler warning
   ZS_threshold_I_ = ZS_threshold_;
}


HcalTriggerPrimitiveAlgo::~HcalTriggerPrimitiveAlgo() {
}


void HcalTriggerPrimitiveAlgo::addSignal(const HBHEDataFrame & frame) {
   //Hack for 300_pre10, should be removed.
   if (frame.id().depth()==5) return;

   std::vector<HcalTrigTowerDetId> ids = theTrigTowerGeometry->towerIds(frame.id());
   assert(ids.size() == 1 || ids.size() == 2);
   IntegerCaloSamples samples1(ids[0], int(frame.size()));

   samples1.setPresamples(frame.presamples());
   incoder_->adc2Linear(frame, samples1);

   std::vector<bool> msb;
   incoder_->lookupMSB(frame, msb);

   if(ids.size() == 2) {
      // make a second trigprim for the other one, and split the energy
      IntegerCaloSamples samples2(ids[1], samples1.size());
      for(int i = 0; i < samples1.size(); ++i) {
         samples1[i] = uint32_t(samples1[i]*0.5);
         samples2[i] = samples1[i];
      }
      samples2.setPresamples(frame.presamples());
      addSignal(samples2);
      addFG(ids[1], msb);
   }
   addSignal(samples1);
   addFG(ids[0], msb);
}


void HcalTriggerPrimitiveAlgo::addSignal(const HFDataFrame & frame) {

   if(frame.id().depth() == 1 || frame.id().depth() == 2) {
      std::vector<HcalTrigTowerDetId> ids = theTrigTowerGeometry->towerIds(frame.id());
      assert(ids.size() == 1);
      IntegerCaloSamples samples(ids[0], frame.size());
      samples.setPresamples(frame.presamples());
      incoder_->adc2Linear(frame, samples);

      // Don't add to final collection yet
      // HF PMT veto sum is calculated in analyzerHF()
      IntegerCaloSamples zero_samples(ids[0], frame.size());
      zero_samples.setPresamples(frame.presamples());
      addSignal(zero_samples);

      // Mask off depths: fgid is the same for both depths
      uint32_t fgid = (frame.id().rawId() | 0x1c000) ;

      if ( theTowerMapFGSum.find(ids[0]) == theTowerMapFGSum.end() ) {
         SumFGContainer sumFG;
         theTowerMapFGSum.insert(std::pair<HcalTrigTowerDetId, SumFGContainer >(ids[0], sumFG));
      }

      SumFGContainer& sumFG = theTowerMapFGSum[ids[0]];
      SumFGContainer::iterator sumFGItr;
      for ( sumFGItr = sumFG.begin(); sumFGItr != sumFG.end(); ++sumFGItr) {
         if (sumFGItr->id() == fgid) break;
      }
      // If find
      if (sumFGItr != sumFG.end()) {
         for (int i=0; i<samples.size(); ++i) (*sumFGItr)[i] += samples[i];
      }
      else {
         //Copy samples (change to fgid)
         IntegerCaloSamples sumFGSamples(DetId(fgid), samples.size());
         sumFGSamples.setPresamples(samples.presamples());
         for (int i=0; i<samples.size(); ++i) sumFGSamples[i] = samples[i];
         sumFG.push_back(sumFGSamples);
      }

      // set veto to true if Long or Short less than threshold
      if (HF_Veto.find(fgid) == HF_Veto.end()) {
         vector<bool> vetoBits(samples.size(), false);
         HF_Veto[fgid] = vetoBits;
      }
      for (int i=0; i<samples.size(); ++i)
         if (samples[i] < minSignalThreshold_)
            HF_Veto[fgid][i] = true;
   }
}


void HcalTriggerPrimitiveAlgo::addSignal(const HcalUpgradeDataFrame& frame) {
   int depth = HcalDetId(frame.id()).depth();
   auto ids = theTrigTowerGeometry->towerIds(frame.id());

   if (ids.size() > 0 && ids[0].ietaAbs() >= theTrigTowerGeometry->firstHFTower()) {
      // HF
      if (true) {
      /* if(frame.id().depth() == 1 || frame.id().depth() == 2) { */
         assert(ids.size() == 1);
         IntegerCaloSamples samples(ids[0], frame.size());
         samples.setPresamples(frame.presamples());
         incoder_->adc2Linear(frame, samples);

         // Don't add to final collection yet
         // HF PMT veto sum is calculated in analyzerHF()
         IntegerCaloSamples zero_samples(ids[0], frame.size());
         zero_samples.setPresamples(frame.presamples());
         addSignal(zero_samples, depth, timing(frame));

         // Mask off depths: fgid is the same for both depths
         uint32_t fgid = (frame.id().rawId() | 0x1c000) ;

         if ( theTowerMapFGSum.find(ids[0]) == theTowerMapFGSum.end() ) {
            SumFGContainer sumFG;
            theTowerMapFGSum.insert(std::pair<HcalTrigTowerDetId, SumFGContainer >(ids[0], sumFG));

            // Add a depth level
            DepthFGMap depthFGmap;
            theTowerMapFGDepth.insert(std::pair<HcalTrigTowerDetId, DepthFGMap>(ids[0], depthFGmap));
         }

         SumFGContainer& sumFG = theTowerMapFGSum[ids[0]];
         SumFGContainer::iterator sumFGItr;
         for ( sumFGItr = sumFG.begin(); sumFGItr != sumFG.end(); ++sumFGItr) {
            if (sumFGItr->id() == fgid) break;
         }
         // If find
         if (sumFGItr != sumFG.end()) {
            for (int i=0; i<samples.size(); ++i) (*sumFGItr)[i] += samples[i];
         }
         else {
            //Copy samples (change to fgid)
            IntegerCaloSamples sumFGSamples(DetId(fgid), samples.size());
            sumFGSamples.setPresamples(samples.presamples());
            for (int i=0; i<samples.size(); ++i) sumFGSamples[i] = samples[i];
            sumFG.push_back(sumFGSamples);
         }

         DepthFGMap& depthFGmap = theTowerMapFGDepth[ids[0]];
         if (depthFGmap.find(fgid) == depthFGmap.end()) {
            depthFGmap.insert(std::make_pair(fgid, Sample()));
         }

         depthFGmap[fgid].add(depth, samples, timing(frame));

         // set veto to true if Long or Short less than threshold
         if (HF_Veto.find(fgid) == HF_Veto.end()) {
            vector<bool> vetoBits(samples.size(), false);
            HF_Veto[fgid] = vetoBits;
         }
         for (int i=0; i<samples.size(); ++i)
            if (samples[i] < minSignalThreshold_)
               HF_Veto[fgid][i] = true;
      }
   } else if (ids.size() > 0) {
      // HBHE
      assert(ids.size() == 1 || ids.size() == 2);

      IntegerCaloSamples samples1(ids[0], int(frame.size()));

      samples1.setPresamples(frame.presamples());
      incoder_->adc2Linear(frame, samples1);

      std::vector<bool> msb;
      incoder_->lookupMSB(frame, msb);

      if (ids.size() == 2) {
         IntegerCaloSamples samples2(ids[1], samples1.size());
         for (int i = 0; i < samples1.size(); ++i) {
            // FIXME if the original value of samples1[i] is odd, we loose a
            // count.
            samples1[i] = uint32_t(samples1[i] * 0.5);
            samples2[i] = samples1[i];
         }
         samples2.setPresamples(frame.presamples());
         addSignal(samples2, depth, timing(frame));
         addFG(ids[1], msb);
      }

      addSignal(samples1, depth, timing(frame));
      addFG(ids[0], msb);
   }
}


void HcalTriggerPrimitiveAlgo::addSignal(const IntegerCaloSamples & samples, int depth, const std::pair<double, double>& tdc) {
   HcalTrigTowerDetId id(samples.id());
   SumMap::iterator itr = theSumMap.find(id);
   if(itr == theSumMap.end()) {
      theSumMap.insert(std::make_pair(id, samples));
      theDepthMap.insert(std::make_pair(id, Sample()));
   }
   else {
      // wish CaloSamples had a +=
      for(int i = 0; i < samples.size(); ++i) {
         (itr->second)[i] += samples[i];
      }
   }

   theDepthMap[id].add(depth, samples, tdc);
}


void HcalTriggerPrimitiveAlgo::analyze(IntegerCaloSamples & samples, HcalTriggerPrimitiveDigi & result) {}

void HcalTriggerPrimitiveAlgo::analyze(IntegerCaloSamples & samples, HcalUpgradeTriggerPrimitiveDigi & result) {
   int shrink = weights_.size() - 1;
   std::vector<bool>& msb = fgMap_[samples.id()];
   IntegerCaloSamples sum(samples.id(), samples.size());

   //slide algo window
   for(int ibin = 0; ibin < int(samples.size())- shrink; ++ibin) {
      int algosumvalue = 0;
      for(unsigned int i = 0; i < weights_.size(); i++) {
         //add up value * scale factor
         algosumvalue += int(samples[ibin+i] * weights_[i]);
      }
      if (algosumvalue<0) sum[ibin]=0;            // low-side
                                                  //high-side
      //else if (algosumvalue>0x3FF) sum[ibin]=0x3FF;
      else sum[ibin] = algosumvalue;              //assign value to sum[]
   }

   std::vector<int> depth_sums(5, 0);

   // Align digis and TP
   int dgPresamples=samples.presamples(); 
   int tpPresamples=numberOfPresamples_;
   int shift = dgPresamples - tpPresamples;
   int dgSamples=samples.size();
   int tpSamples=numberOfSamples_;
   if(peakfind_){
       if((shift<shrink) || (shift + tpSamples + shrink > dgSamples - (peak_finder_algorithm_ - 1) )   ){
	    edm::LogInfo("HcalTriggerPrimitiveAlgo::analyze") << 
		"TP presample or size from the configuration file is out of the accessible range. Using digi values from data instead...";
	    shift=shrink;
	    tpPresamples=dgPresamples-shrink;
	    tpSamples=dgSamples-(peak_finder_algorithm_-1)-shrink-shift;
       }
   }

   std::vector<bool> finegrain(tpSamples,false);

   IntegerCaloSamples output(samples.id(), tpSamples);
   output.setPresamples(tpPresamples);

   for (int ibin = 0; ibin < tpSamples; ++ibin) {
      // ibin - index for output TP
      // idx - index for samples + shift
      int idx = ibin + shift;

      //Peak finding
      if (peakfind_) {
         bool isPeak = false;
         switch (peak_finder_algorithm_) {
            case 1 :
               isPeak = (samples[idx] > samples[idx-1] && samples[idx] >= samples[idx+1] && samples[idx] > theThreshold);
               break;
            case 2:
               isPeak = (sum[idx] > sum[idx-1] && sum[idx] >= sum[idx+1] && sum[idx] > theThreshold);
               break;
            default:
               break;
         }

         if (isPeak){
            output[ibin] = std::min<unsigned int>(sum[idx], sample_mask_);
            finegrain[ibin] = msb[idx];
            // Only provide depth information for the SOI.  This is the
            // energy value used downstream, even if the peak is found for
            // another sample.
            if (ibin == numberOfPresamples_) {
               for (int d = 0; d < 5; ++d) {
                  auto algosumvalue = 0;
                  for (unsigned int i = 0; i < weights_.size(); ++i)
                     algosumvalue += int(theDepthMap[samples.id()][d][idx + i] * weights_[i]);
                  depth_sums[d] += std::min<unsigned int>(algosumvalue, sample_mask_);
               }
            }
         } else {
            output[ibin] = 0;
         }
      }
      else { // No peak finding, just output running sum
         output[ibin] = std::min<unsigned int>(sum[idx], sample_mask_);
         finegrain[ibin] = msb[idx];

         // See comment above.
         if (ibin == numberOfPresamples_) {
            for (int d = 0; d < 5; ++d) {
               auto algosumvalue = 0;
               for (unsigned int i = 0; i < weights_.size(); ++i)
                  algosumvalue += int(theDepthMap[samples.id()][d][idx + i] * weights_[i]);
               depth_sums[d] += std::min<unsigned int>(algosumvalue, sample_mask_);
            }
         }
      }

      // Only Pegged for 1-TS algo.
      if (peak_finder_algorithm_ == 1) {
         if (samples[idx] >= sample_mask_)
            output[ibin] = sample_mask_;
      }
   }

   update(result, theDepthMap[samples.id()], numberOfPresamples_);
}


void HcalTriggerPrimitiveAlgo::analyzeHF(IntegerCaloSamples & samples, HcalTriggerPrimitiveDigi & result, float rctlsb) {}

void HcalTriggerPrimitiveAlgo::analyzeHF(IntegerCaloSamples & samples, HcalUpgradeTriggerPrimitiveDigi & result, float rctlsb) {
   HcalTrigTowerDetId detId(samples.id());

   // Align digis and TP
   int dgPresamples=samples.presamples(); 
   int tpPresamples=numberOfPresamples_;
   int shift = dgPresamples - tpPresamples;
   int dgSamples=samples.size();
   int tpSamples=numberOfSamples_;
   if(shift<0 || shift+tpSamples>dgSamples){
	edm::LogInfo("HcalTriggerPrimitiveAlgo::analyzeHF") << 
	    "TP presample or size from the configuration file is out of the accessible range. Using digi values from data instead...";
	tpPresamples=dgPresamples;
	shift=0;
	tpSamples=dgSamples;
   }

   std::vector<bool> finegrain(tpSamples, false);

   TowerMapFGSum::const_iterator tower2fg = theTowerMapFGSum.find(detId);
   assert(tower2fg != theTowerMapFGSum.end());

   Sample depths;

   const SumFGContainer& sumFG = tower2fg->second;
   // Loop over all L+S pairs that mapped from samples.id()
   // Note: 1 samples.id() = 6 x (L+S) without noZS
   for (SumFGContainer::const_iterator sumFGItr = sumFG.begin(); sumFGItr != sumFG.end(); ++sumFGItr) {
      const std::vector<bool>& veto = HF_Veto[sumFGItr->id().rawId()];

      // FIXME still needed?
      if (!veto[numberOfSamples_])
         depths += theTowerMapFGDepth[detId][sumFGItr->id()];

      for (int ibin = 0; ibin < tpSamples; ++ibin) {
         int idx = ibin + shift;
         // if not vetod, add L+S to total sum and calculate FG
	 bool vetoed = idx<int(veto.size()) && veto[idx];
         if (!(vetoed && (*sumFGItr)[idx] > PMT_NoiseThreshold_)) {
            samples[idx] += (*sumFGItr)[idx];
            finegrain[ibin] = (finegrain[ibin] || (*sumFGItr)[idx] >= FG_threshold_);
         }
      }
   }

   IntegerCaloSamples output(samples.id(), tpSamples);
   output.setPresamples(tpPresamples);

   for (int ibin = 0; ibin < tpSamples; ++ibin) {
      int idx = ibin + shift;
      output[ibin] = samples[idx] / (rctlsb == 0.25 ? 4 : 8);
      if (output[ibin] > sample_mask_) output[ibin] = sample_mask_;
   }
   outcoder_->compress(output, finegrain, result);

   update(result, depths, numberOfPresamples_);
}

void HcalTriggerPrimitiveAlgo::runZS(HcalTrigPrimDigiCollection & result){
   for (HcalTrigPrimDigiCollection::iterator tp = result.begin(); tp != result.end(); ++tp){
      bool ZS = true;
      for (int i=0; i<tp->size(); ++i) {
         if (tp->sample(i).compressedEt()  > ZS_threshold_I_) {
            ZS=false;
            break;
         }
      }
      if (ZS) tp->setZSInfo(false,true);
      else tp->setZSInfo(true,false);
   }
}

void HcalTriggerPrimitiveAlgo::runFEFormatError(const FEDRawDataCollection* rawraw,
                                                const HcalElectronicsMap *emap,
                                                HcalTrigPrimDigiCollection & result
                                                ){
  std::set<uint32_t> FrontEndErrors;

  for(int i=FEDNumbering::MINHCALFEDID; i<=FEDNumbering::MAXHCALFEDID; ++i) {
    const FEDRawData& raw = rawraw->FEDData(i);
    if (raw.size()<12) continue;
    const HcalDCCHeader* dccHeader=(const HcalDCCHeader*)(raw.data());
    if(!dccHeader) continue;
    HcalHTRData htr;
    for (int spigot=0; spigot<HcalDCCHeader::SPIGOT_COUNT; spigot++) {
      if (!dccHeader->getSpigotPresent(spigot)) continue;
      dccHeader->getSpigotData(spigot,htr,raw.size());
      int dccid = dccHeader->getSourceId();
      int errWord = htr.getErrorsWord() & 0x1FFFF;
      bool HTRError = (!htr.check() || htr.isHistogramEvent() || (errWord & 0x800)!=0);

      if(HTRError) {
        bool valid =false;
        for(int fchan=0; fchan<3 && !valid; fchan++) {
          for(int fib=0; fib<9 && !valid; fib++) {
            HcalElectronicsId eid(fchan,fib,spigot,dccid-FEDNumbering::MINHCALFEDID);
            eid.setHTR(htr.readoutVMECrateId(),htr.htrSlot(),htr.htrTopBottom());
            DetId detId = emap->lookup(eid);
            if(detId.null()) continue;
            HcalSubdetector subdet=(HcalSubdetector(detId.subdetId()));
            if (detId.det()!=4||
              (subdet!=HcalBarrel && subdet!=HcalEndcap &&
              subdet!=HcalForward )) continue;
            std::vector<HcalTrigTowerDetId> ids = theTrigTowerGeometry->towerIds(detId);
            for (std::vector<HcalTrigTowerDetId>::const_iterator triggerId=ids.begin(); triggerId != ids.end(); ++triggerId) {
              FrontEndErrors.insert(triggerId->rawId());
            }
            //valid = true;
          }
        }
      }
    }
  }

  // Loop over TP collection
  // Set TP to zero if there is FE Format Error
  HcalTriggerPrimitiveSample zeroSample(0);
  for (HcalTrigPrimDigiCollection::iterator tp = result.begin(); tp != result.end(); ++tp){
    if (FrontEndErrors.find(tp->id().rawId()) != FrontEndErrors.end()) {
      for (int i=0; i<tp->size(); ++i) tp->setSample(i, zeroSample);
    }
  }
}

void HcalTriggerPrimitiveAlgo::addFG(const HcalTrigTowerDetId& id, std::vector<bool>& msb){
   FGbitMap::iterator itr = fgMap_.find(id);
   if (itr != fgMap_.end()){
      std::vector<bool>& _msb = itr->second;
      for (size_t i=0; i<msb.size(); ++i)
         _msb[i] = _msb[i] || msb[i];
   }
   else fgMap_[id] = msb;
}

void HcalTriggerPrimitiveAlgo::setPeakFinderAlgorithm(int algo){
   if (algo <=0 && algo>2)
      throw cms::Exception("ERROR: Only algo 1 & 2 are supported.") << std::endl;
   peak_finder_algorithm_ = algo;
}

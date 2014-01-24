#ifndef HcalSimAlgos_HcalTriggerPrimitiveAlgo_h
#define HcalSimAlgos_HcalTriggerPrimitiveAlgo_h

#include "DataFormats/HcalDetId/interface/HcalTrigTowerDetId.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "Geometry/HcalTowerAlgo/interface/HcalTrigTowerGeometry.h"
#include "CalibFormats/CaloObjects/interface/CaloSamples.h"
#include "CalibFormats/CaloObjects/interface/IntegerCaloSamples.h"
//#include "CalibFormats/HcalObjects/interface/HcalTPGCoder.h"
#include "CalibCalorimetry/HcalTPGAlgos/interface/HcaluLUTTPGCoder.h"
#include "CalibFormats/CaloTPG/interface/HcalTPGCompressor.h"
#include "CondFormats/HcalObjects/interface/HcalElectronicsMap.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "CalibFormats/CaloTPG/interface/CaloTPGTranscoder.h"

#include <map>
#include <vector>
class CaloGeometry;
class IntegerCaloSamples;

class Sample {
   public:
      Sample() : samples_(5), oot_(5), rising_(5), falling_(5) {};
      // Depth levels in the DetId start with 1
      void add(int depth, const IntegerCaloSamples& samples, const std::pair<double, double>& tdc) {
         for (int i = 0; i < samples.size(); ++i)
            samples_[depth - 1][i] += samples[i];
         if (tdc.first > -998.)
            rising_[depth - 1].push_back(tdc.first);
         if (tdc.second > -998.)
            falling_[depth - 1].push_back(tdc.second);

         if (((tdc.first < tdc.second || tdc.first < -998.) && tdc.second < 0. && tdc.second > -998.)
               || ((tdc.first < tdc.second || tdc.second < 0.) && tdc.first > 25.)) {
            for (int i = 0; i < samples.size(); ++i)
               oot_[depth - 1][i] += samples[i];
         }
      };
      Sample& operator+=(const Sample& o) {
         for (unsigned d = 0; d < samples_.size(); ++d) {
            for (int i = 0; i < samples_[d].size(); ++i) {
               samples_[d][i] += o.samples_[d][i];
               oot_[d][i] += o.oot_[d][i];
            }
            rising_[d].insert(rising_[d].end(), o.rising_[d].begin(), o.rising_[d].end());
            falling_[d].insert(falling_[d].end(), o.falling_[d].begin(), o.falling_[d].end());
         }
         return *this;
      };
      const IntegerCaloSamples& operator[](int i) const {
         return samples_[i];
      };
      const IntegerCaloSamples& operator()(int i) const {
         return oot_[i];
      };
      unsigned int size() const {
         return samples_.size();
      };
      const std::vector<double>& rise(int i) const {
         return rising_[i];
      };
      const std::vector<double>& fall(int i) const {
         return falling_[i];
      };
   private:
      std::vector<IntegerCaloSamples> samples_;
      std::vector<IntegerCaloSamples> oot_;
      std::vector<std::vector<double>> rising_;
      std::vector<std::vector<double>> falling_;
};

class HcalTriggerPrimitiveAlgo {
public:
  HcalTriggerPrimitiveAlgo(bool pf, const std::vector<double>& w, 
                           int latency,
                           uint32_t FG_threshold, uint32_t ZS_threshold,
                           int numberOfSamples, int numberOfPresamples,
                           uint32_t minSignalThreshold=0, uint32_t PMT_NoiseThreshold=0, bool upgrade=false);
  ~HcalTriggerPrimitiveAlgo();

  template<typename HBHEColl, typename HFColl, typename TPColl>
  void run(const HcalTPGCoder* incoder,
           const HcalTPGCompressor* outcoder,
           const CaloTPGTranscoder* coder,
           const HBHEColl& hbheDigis,
           const HFColl& hfDigis,
           TPColl& result,
	   const HcalTrigTowerGeometry* trigTowerGeometry,
           float rctlsb);

  void runZS(HcalTrigPrimDigiCollection& tp);
  void runFEFormatError(const FEDRawDataCollection* rawraw,
                        const HcalElectronicsMap* emap,
                        HcalTrigPrimDigiCollection & result);
  void setPeakFinderAlgorithm(int algo);
 private:

  /// adds the signal to the map
  void addSignal(const HBHEDataFrame & frame);
  void addSignal(const HFDataFrame & frame);
  void addSignal(const HcalUpgradeDataFrame& frame);
  void addSignal(const IntegerCaloSamples & samples, int depth=0, const std::pair<double, double>& tdc={0, 0});
  void addFG(const HcalTrigTowerDetId& id, std::vector<bool>& msb);

  /// adds the actual RecHits
  void analyze(IntegerCaloSamples & samples, HcalTriggerPrimitiveDigi & result);
  void analyze(IntegerCaloSamples & samples, HcalUpgradeTriggerPrimitiveDigi & result);
  void analyzeHF(IntegerCaloSamples & samples, HcalTriggerPrimitiveDigi & result, float rctlsb);
  void analyzeHF(IntegerCaloSamples & samples, HcalUpgradeTriggerPrimitiveDigi & result, float rctlsb);

   // Member initialized by constructor
  const HcaluLUTTPGCoder* incoder_;
  const HcalTPGCompressor* outcoder_;
  const CaloTPGTranscoder* coder_;
  double theThreshold;
  bool peakfind_;
  std::vector<double> weights_;
  int latency_;
  uint32_t FG_threshold_;
  uint32_t ZS_threshold_;
  int ZS_threshold_I_;
  int numberOfSamples_;
  int numberOfPresamples_;
  uint32_t minSignalThreshold_;
  uint32_t PMT_NoiseThreshold_; 

  bool upgrade_;
  const unsigned int sample_mask_;

  // Algo1: isPeak = TS[i-1] < TS[i] && TS[i] >= TS[i+1]
  // Algo2: isPeak = TSS[i-1] < TSS[i] && TSS[i] >= TSS[i+1],
  // TSS[i] = TS[i] + TS[i+1]
  // Default: Algo2
  int peak_finder_algorithm_;

  // Member not initialzed
  //std::vector<HcalTrigTowerDetId> towerIds(const HcalDetId & id) const;

  const HcalTrigTowerGeometry * theTrigTowerGeometry;

  typedef std::map<HcalTrigTowerDetId, IntegerCaloSamples> SumMap;
  SumMap theSumMap;  

  typedef std::map<HcalTrigTowerDetId, Sample> DepthMap;
  DepthMap theDepthMap;
  
  typedef std::vector<IntegerCaloSamples> SumFGContainer;
  typedef std::map< HcalTrigTowerDetId, SumFGContainer > TowerMapFGSum;
  TowerMapFGSum theTowerMapFGSum;

  typedef std::map< uint32_t, Sample > DepthFGMap;
  typedef std::map< HcalTrigTowerDetId, DepthFGMap > TowerMapFGDepth;
  TowerMapFGDepth theTowerMapFGDepth;

  // ==============================
  // =  HF Veto
  // ==============================
  // Sum = Long + Short;" // intermediate calculation. 
  //  if ((Short < MinSignalThresholdET OR Long  < MinSignalThresholdET)
  //     AND Sum > PMTNoiseThresholdET) VetoedSum = 0; 
  //  else VetoedSum = Sum; 
  // ==============================
  // Map from FG id to veto booleans
  typedef std::map<uint32_t, std::vector<bool> > TowerMapVeto;
  TowerMapVeto HF_Veto;

  typedef std::map<HcalTrigTowerDetId, std::vector<bool> > FGbitMap;
  FGbitMap fgMap_;
};

template<typename HBHEColl, typename HFColl, typename TPColl>
void HcalTriggerPrimitiveAlgo::run(const HcalTPGCoder* incoder,
                                   const HcalTPGCompressor* outcoder,
                                   const CaloTPGTranscoder* coder,
                                   const HBHEColl& hbheDigis,
                                   const HFColl& hfDigis,
                                   TPColl& result,
				   const HcalTrigTowerGeometry* trigTowerGeometry,
                                   float rctlsb) {
   theTrigTowerGeometry = trigTowerGeometry;
    
   incoder_=dynamic_cast<const HcaluLUTTPGCoder*>(incoder);
   outcoder_=outcoder;
   coder_=coder;

   theDepthMap.clear();
   theTowerMapFGDepth.clear();

   theSumMap.clear();
   theTowerMapFGSum.clear();
   HF_Veto.clear();
   fgMap_.clear();

   // do the HB/HE digis
   for(const auto& digi: hbheDigis) {
      addSignal(digi);
   }

   // and the HF digis
   for(const auto& digi: hfDigis) {
      addSignal(digi);
   }

   for(SumMap::iterator mapItr = theSumMap.begin(); mapItr != theSumMap.end(); ++mapItr) {
      result.push_back((typename TPColl::value_type)(mapItr->first));
      HcalTrigTowerDetId detId(mapItr->second.id());
      if (detId.ietaAbs() >= theTrigTowerGeometry->firstHFTower()) {
         analyzeHF(mapItr->second, result.back(), rctlsb);
      } else{
         analyze(mapItr->second, result.back());
      }
   }

   return;
}

#endif

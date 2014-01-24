#ifndef CALIBFORMATS_CALOTPG_HCALTPGCOMPRESSOR_H
#define CALIBFORMATS_CALOTPG_HCALTPGCOMPRESSOR_H 1

#include "CalibFormats/CaloObjects/interface/IntegerCaloSamples.h"
#include "DataFormats/HcalDigi/interface/HcalTriggerPrimitiveDigi.h"
#include "DataFormats/HcalDigi/interface/HcalUpgradeTriggerPrimitiveDigi.h"
class CaloTPGTranscoder;

/** \class HcalTPGCompressor
  *  
  * \author J. Mans - Minnesota
  */
class HcalTPGCompressor {
public:
  HcalTPGCompressor(const CaloTPGTranscoder* coder);
  void compress(const IntegerCaloSamples& ics, const std::vector<bool>& fineGrain, HcalTriggerPrimitiveDigi& digi) const;
  void compress(const IntegerCaloSamples& ics, const std::vector<bool>& fineGrain, HcalUpgradeTriggerPrimitiveDigi& digi) const;
  HcalTriggerPrimitiveSample compress(const HcalTrigTowerDetId& id, unsigned int sample, bool fineGrain) const;
  HcalUpgradeTriggerPrimitiveSample compressUpgrade(const HcalTrigTowerDetId& id, unsigned int sample, bool fineGrain) const;
  const CaloTPGTranscoder* getCoder() { return coder_; };
private:
  const CaloTPGTranscoder* coder_;
};

#endif

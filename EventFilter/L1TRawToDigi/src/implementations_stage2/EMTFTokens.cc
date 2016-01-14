#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "EMTFTokens.h"

namespace l1t {
   namespace stage2 {
      EMTFTokens::EMTFTokens(const edm::ParameterSet& cfg, edm::ConsumesCollector& cc) : PackerTokens(cfg, cc)
      {
         // auto bmtfTag = cfg.getParameter<edm::InputTag>("BMTFInputLabel");
         // auto omtfTag = cfg.getParameter<edm::InputTag>("OMTFInputLabel");
         auto emtfTag = cfg.getParameter<edm::InputTag>("EMTFInputLabelAWB");
         auto tag = cfg.getParameter<edm::InputTag>("InputLabel");

         // regionalMuonCandTokenBMTF_ = cc.consumes<RegionalMuonCandBxCollection>(bmtfTag);
         // regionalMuonCandTokenOMTF_ = cc.consumes<RegionalMuonCandBxCollection>(omtfTag);
         regionalMuonCandTokenEMTF_ = cc.consumes<RegionalMuonCandBxCollection>(emtfTag);
         EMTFMuonCandToken_ = cc.consumes<EMTFMuonCandBxCollection>(tag); // Does this need a tag? which one? - AWB 11.01.15
         muonToken_ = cc.consumes<MuonBxCollection>(tag);
      }
   }
}

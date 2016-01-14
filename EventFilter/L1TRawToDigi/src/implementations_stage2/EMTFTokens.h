
#ifndef EMTFTokens_h
#define EMTFTokens_h

#include "DataFormats/L1TMuon/interface/RegionalMuonCand.h"
#include "DataFormats/L1TMuon/interface/EMTFMuonCand.h"
#include "DataFormats/L1Trigger/interface/Muon.h"

#include "EventFilter/L1TRawToDigi/interface/PackerTokens.h"

namespace l1t {
   namespace stage2 {
      class EMTFTokens : public PackerTokens {
         public:
            EMTFTokens(const edm::ParameterSet&, edm::ConsumesCollector&);

            inline const edm::EDGetTokenT<RegionalMuonCandBxCollection>& getRegionalMuonCandTokenEMTF() const { return regionalMuonCandTokenEMTF_; };
            inline const edm::EDGetTokenT<EMTFMuonCandBxCollection>& getEMTFMuonCandToken() const { return EMTFMuonCandToken_; };
            inline const edm::EDGetTokenT<MuonBxCollection>& getMuonToken() const { return muonToken_; };

         private:

            edm::EDGetTokenT<RegionalMuonCandBxCollection> regionalMuonCandTokenEMTF_;
            edm::EDGetTokenT<EMTFMuonCandBxCollection> EMTFMuonCandToken_;
            edm::EDGetTokenT<MuonBxCollection> muonToken_;

      };
   }
}

#endif

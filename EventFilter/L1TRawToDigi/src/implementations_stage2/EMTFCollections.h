
#ifndef EMTFCollections_h
#define EMTFCollections_h

#include "DataFormats/L1TMuon/interface/RegionalMuonCand.h"
#include "DataFormats/L1TMuon/interface/EMTFMuonCand.h"
#include "DataFormats/L1Trigger/interface/Muon.h"

#include "EventFilter/L1TRawToDigi/interface/UnpackerCollections.h"

namespace l1t {
   namespace stage2 {
      class EMTFCollections : public UnpackerCollections {
         public:
            EMTFCollections(edm::Event& e) :
               UnpackerCollections(e),
               regionalMuonCandsEMTF_(new RegionalMuonCandBxCollection()),
               EMTFMuonCands_(new EMTFMuonCandBxCollection()),
               muons_(new MuonBxCollection()) {};

            virtual ~EMTFCollections();

            inline RegionalMuonCandBxCollection* getRegionalMuonCandsEMTF() { return regionalMuonCandsEMTF_.get(); };
            inline EMTFMuonCandBxCollection* getEMTFMuonCands() { return EMTFMuonCands_.get(); };
            inline MuonBxCollection* getMuons() { return muons_.get(); };

         private:

            std::auto_ptr<RegionalMuonCandBxCollection> regionalMuonCandsEMTF_;
            std::auto_ptr<EMTFMuonCandBxCollection> EMTFMuonCands_;
            std::auto_ptr<MuonBxCollection> muons_;

      };
   }
}

#endif

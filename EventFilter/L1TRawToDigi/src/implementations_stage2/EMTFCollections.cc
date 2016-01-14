#include "FWCore/Framework/interface/Event.h"

#include "EMTFCollections.h"

namespace l1t {
   namespace stage2 {
      EMTFCollections::~EMTFCollections()
      {
         // event_.put(regionalMuonCandsBMTF_, "BMTF");
         // event_.put(regionalMuonCandsOMTF_, "OMTF");
         event_.put(regionalMuonCandsEMTF_, "EMTF");
         event_.put(EMTFMuonCands_);
         event_.put(muons_);
      }
   }
}

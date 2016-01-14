
#include "FWCore/Framework/interface/stream/EDProducerBase.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "EventFilter/L1TRawToDigi/interface/Packer.h"
#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"

#include "EventFilter/L1TRawToDigi/interface/PackingSetup.h"

#include "EMTFCollections.h"
#include "EMTFTokens.h"

namespace l1t {
   namespace stage2 {
      class EMTFSetup : public PackingSetup {
         public:
            virtual std::unique_ptr<PackerTokens> registerConsumes(const edm::ParameterSet& cfg, edm::ConsumesCollector& cc) override {
               return std::unique_ptr<PackerTokens>(new EMTFTokens(cfg, cc));
            };

            virtual void fillDescription(edm::ParameterSetDescription& desc) override {
               // desc.addOptional<edm::InputTag>("BMTFInputLabel")->setComment("for stage2");
               // desc.addOptional<edm::InputTag>("OMTFInputLabel")->setComment("for stage2");
               desc.addOptional<edm::InputTag>("EMTFInputLabelAWB")->setComment("for stage2");
            };

            virtual PackerMap getPackers(int fed, unsigned int fw) override {
               PackerMap res;

               if (fed == 1402) {
                  // Use amc_no and board id 1 for packing
                  res[{1, 1}] = {
 		    // "RegionalMuonEMTFPacker" should be defined in RegionalMuonEMTFPacker.cc - AWB 11.01.15
		    PackerFactory::get()->make("stage2::RegionalMuonEMTFPacker"), 
		    // Should we even be doing a MuonPacker? = AWB 11.01.15
		    PackerFactory::get()->make("stage2::MuonPacker"),
                  };
               }

               return res;
            };

            virtual void registerProducts(edm::stream::EDProducerBase& prod) override {
               prod.produces<RegionalMuonCandBxCollection>("EMTF");
               prod.produces<EMTFMuonCandBxCollection>();
               prod.produces<MuonBxCollection>();
            };

            virtual std::unique_ptr<UnpackerCollections> getCollections(edm::Event& e) override {
               return std::unique_ptr<UnpackerCollections>(new EMTFCollections(e));
            };

            virtual UnpackerMap getUnpackers(int fed, int board, int amc, unsigned int fw) override {
	    // Presumably need some logic based on fed, amc, etc (c.f. CaloSetup.cc) - AWB 11.01.16
               UnpackerMap res;

	       // "RegionalMuonEMTFPacker" should be defined in RegionalMuonEMTFPacker.cc - AWB 11.01.15
               auto emtf_in_unp = UnpackerFactory::get()->make("stage2::RegionalMuonEMTFUnpacker");
	       // Should we even be doing a MuonPacker? = AWB 11.01.15
               auto emtf_out_unp = UnpackerFactory::get()->make("stage2::MuonUnpacker");

               // // input muons
               // for (int iLink = 72; iLink < 144; iLink += 2)
               //     res[iLink] = emtf_in_unp;
               // // output muons
               // for (int oLink = 1; oLink < 9; oLink += 2)
               //     res[oLink] = emtf_out_unp;
               // // internal muons
               // //for (int oLink = 9; oLink < 24; oLink += 2)
               // //    res[oLink] = emtf_out_unp;

               return res;
            };
      };
   }
}

DEFINE_L1T_PACKING_SETUP(l1t::stage2::EMTFSetup);

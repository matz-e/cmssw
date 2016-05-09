#include "FWCore/Framework/interface/Event.h"

#include "EventFilter/L1TRawToDigi/interface/Packer.h"

namespace l1t {
   namespace stage2 {
      class EMTFDummyHeaderPacker : public Packer {
         public:
            virtual Blocks pack(const edm::Event&, const PackerTokens*) override { return {Block(511, {
                  0x1001, 0x1002, 0x1003, 0x1004, 0x2005, 0x2006, 0x2007, 0x2008, 9, 10, 11, 12
                  }, 0, MTF7)}; };
      };
      class EMTFDummyTrailerPacker : public Packer {
         public:
            virtual Blocks pack(const edm::Event&, const PackerTokens*) override { return {Block(255, {1,2,3,4,5,6,7,8}, 0, MTF7)}; };
      };
      class EMTFDummyCounterPacker : public Packer {
         public:
            virtual Blocks pack(const edm::Event&, const PackerTokens*) override { return {Block(2, {1,2,3,4}, 0, MTF7)}; };
      };
      class EMTFDummyMEPacker : public Packer {
         public:
            virtual Blocks pack(const edm::Event&, const PackerTokens*) override { return {Block(3, {1,2,3,4}, 0, MTF7)}; };
      };
      class EMTFDummyRPCPacker : public Packer {
         public:
            virtual Blocks pack(const edm::Event&, const PackerTokens*) override { return {Block(4, {1,2,3,4}, 0, MTF7)}; };
      };
      class EMTFDummySPPacker : public Packer {
         public:
            virtual Blocks pack(const edm::Event&, const PackerTokens*) override { return {Block(101, {1,2,3,4,5,6,7,8}, 0, MTF7)}; };
      };
   }
}

DEFINE_L1T_PACKER(l1t::stage2::EMTFDummyHeaderPacker);
DEFINE_L1T_PACKER(l1t::stage2::EMTFDummyCounterPacker);
DEFINE_L1T_PACKER(l1t::stage2::EMTFDummyMEPacker);
DEFINE_L1T_PACKER(l1t::stage2::EMTFDummyRPCPacker);
DEFINE_L1T_PACKER(l1t::stage2::EMTFDummySPPacker);
DEFINE_L1T_PACKER(l1t::stage2::EMTFDummyTrailerPacker);

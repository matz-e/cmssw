#include <iomanip>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "EventFilter/L1TRawToDigi/interface/Block.h"

#define EDM_ML_DEBUG 1

namespace l1t {
   uint32_t
   BlockHeader::raw(block_t type) const
   {
      if (type_ == MP7) {
         LogTrace("L1T") << "Writing MP7 link header";
         return ((id_ & ID_mask) << ID_shift) | ((size_ & size_mask) << size_shift) | ((capID_ & capID_mask) << capID_shift);
      }
      // if (type_ == MTF7) {
      //    LogTrace("L1T") << "Writing MTF7 link header";
      //    return ((id_ & ID_mask) << ID_shift) | ((size_ & size_mask) << size_shift) | ((capID_ & capID_mask) << capID_shift);
      // }
      LogTrace("L1T") << "Writing CTP7 link header";
      return ((id_ & CTP7_mask) << CTP7_shift);
   }

   std::auto_ptr<Block>
   Payload::getBlock()
   {
      if (end_ - data_ < getHeaderSize()) {
         LogDebug("L1T") << "Reached end of payload";
         return std::auto_ptr<Block>();
      }

      if (data_[0] == 0xffffffff) {
         LogDebug("L1T") << "Skipping padding word";
         ++data_;
         return getBlock();
      }

      auto header = getHeader();

      if (end_ - data_ < header.getSize()) {
         edm::LogError("L1T")
            << "Expecting a block size of " << header.getSize()
            << " but only " << (end_ - data_) << " words remaining";
         return std::auto_ptr<Block>();
      }

      LogTrace("L1T") << "Creating block with size " << header.getSize();

      auto res = std::auto_ptr<Block>(new Block(header, data_, data_ + header.getSize()));
      data_ += header.getSize();
      return res;
   }

   MP7Payload::MP7Payload(const uint32_t * data, const uint32_t * end, bool legacy_mc) : Payload(data, end)
   {
      // For legacy MC (74 first MC campaigns) skip one empty word that was
      // reserved for the header.  With data, read out infrastructure
      // version and algorithm version.
      if (legacy_mc) {
         LogTrace("L1T") << "Skipping " << std::hex << *data_;
         ++data_;
      } else {
         infra_ = data_[0];
         algo_ = data_[1];
         data_ += 2;
      }
   }

   BlockHeader
   MP7Payload::getHeader()
   {
      LogTrace("L1T") << "Getting header from " << std::hex << std::setw(8) << *data_;

      return BlockHeader(data_++);
   }

   MTF7Payload::MTF7Payload(const uint32_t * data, const uint32_t * end) : Payload(data, end)
   {
      const uint16_t * data16 = reinterpret_cast<const uint16_t*>(data);
      const uint16_t * end16 = reinterpret_cast<const uint16_t*>(end);

      if (end16 - data16 < header_size + counter_size + trailer_size) {
         edm::LogError("L1T") << "MTF7 payload smaller than allowed!";
         data_ = end_;
      } else if (
            ((data16[0] >> 12) != 0x9) || ((data16[1] >> 12) != 0x9) ||
            ((data16[2] >> 12) != 0x9) || ((data16[3] >> 12) != 0x9) ||
            ((data16[4] >> 12) != 0xA) || ((data16[5] >> 12) != 0xA) ||
            ((data16[6] >> 12) != 0xA) || ((data16[7] >> 12) != 0xA) ||
            ((data16[8] >> 9) != 0b1000000) || ((data16[9] >> 11) != 0) ||
            ((data16[10] >> 11) != 0) || ((data16[11] >> 11) != 0)) {
         edm::LogError("L1T") << "MTF7 payload has invalid header!";
         data_ = end_;
      } else if (
            ((data16[12] >> 15) != 0) || ((data16[13] >> 15) != 1) ||
            ((data16[14] >> 15) != 0) || ((data16[15] >> 15) != 0)) {
         edm::LogError("L1T") << "MTF7 payload has invalid counter block!";
         data_ = end_;
      } else if (
            false) {
         // TODO: check trailer
      } else {
         data_ += (header_size + counter_size) / 2;
         end_ -= trailer_size / 2;
      }

      block_sizes_ = {
         {0b0011, {1, 0b0011}},
         {0b0100, {1, 0b0100}},
         {0b0101, {2, 0b01100101}}
      };
   }

   std::auto_ptr<Block>
   MTF7Payload::getBlock()
   {
      if (end_ - data_ < 2)
         return std::auto_ptr<Block>(0);

      const uint16_t * data16 = reinterpret_cast<const uint16_t*>(data_);

      unsigned int id = (data16[0] >> 15) | ((data16[1] >> 15) << 1) | ((data16[2] >> 15) << 2) | ((data16[3] >> 15) << 3);
      unsigned int check = 0;

      if (block_sizes_.find(id) == block_sizes_.end()) {
         edm::LogWarning("L1T") << "MTF7 block with unrecognized id " << id;
         return std::auto_ptr<Block>(0);
      } else if (end_ - data_ < 2 * block_sizes_[id].first) {
         edm::LogWarning("L1T") << "MTF7 block with size exceeding payload (id: " << id << ")";
         return std::auto_ptr<Block>(0);
      }

      std::vector<uint32_t> payload;
      for (unsigned int i = 0; i < block_sizes_[id].first * 4; ++i) {
         check |= (data16[i] >> 15) << i;
         payload.push_back(data16[i]);
      }
      data_ += block_sizes_[id].first * 2;

      if (check != block_sizes_[id].second) {
         edm::LogWarning("L1T") << "Incorrect full id " << check;
      }

      return std::auto_ptr<Block>(new Block(block_sizes_[id].second, payload, 0, MTF7));
   }

   CTP7Payload::CTP7Payload(const uint32_t * data, const uint32_t * end) : Payload(data, end)
   {
      ++data_;
      size_ = (*data >> size_shift) & size_mask;
      ++data_;
   }

   BlockHeader
   CTP7Payload::getHeader()
   {
      return BlockHeader(data_++, size_);
   }
}

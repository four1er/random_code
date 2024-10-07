#include <iostream>

#include "slice.h"
#include "status.h"

namespace leveldb {

/**
 * @brief BlockHandle
 *
 * @details A BlockHandle is used to reference a block by offset and block size.
 * kMaxEncodedLength = 20; because BlockHandle warps 2 varint64.
 *
 */
class BlockHandle {
 public:
  enum { kMaxEncodedLength = 10 + 10 };
  BlockHandle();
  uint64_t offset() const { return offset_; }
  void set_offset(const uint64_t& offset) { offset_ = offset; }
  uint64_t size() const { return size_; }
  void set_size(const uint64_t& size) { size_ = size; }

  void EncodeTo(std::string* dst) const;

  Status DecodeFrom(Slice* input);

 private:
  uint64_t offset_;
  uint64_t size_;
};

}  // namespace leveldb

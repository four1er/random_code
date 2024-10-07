#include <iostream>

namespace leveldb {

struct BlockContents;
class Comparators;
class Iter;
class Iterator;
class Block {
 public:
  explicit Block(const BlockContents&);
  Block(const Block&) = delete;
  Block& operator=(const Block&) = delete;
  ~Block();

  size_t size() const { return size_; };
  Iterator* NewIterator(const Comparators&);

 private:
  uint32_t NumRestarts() const;
  const char* data_;
  size_t size_;
  uint32_t restart_offset_;
  bool owned_;
};
};  // namespace leveldb
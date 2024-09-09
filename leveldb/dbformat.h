#pragma once

#include <iostream>

#include "version_edit.h"

namespace leveldb {
typedef uint64_t SequenceNumber;

class InternalKey {
 private:
  std::string rep_;
};
}  // namespace leveldb
#pragma once

#include <iostream>

#include "leveldb/slice.h"

namespace leveldb {
void PutVarint32(std::string* dst, uint32_t v);
void PutVarint64(std::string* dst, uint64_t v);

char* EncodeVarint32(char* dst, uint32_t v);

void PutLengthPrefixedSlice(std::string* dst, const Slice& value);
}  // namespace leveldb
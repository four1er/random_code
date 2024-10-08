#pragma once

#include <iostream>
#include <set>
#include <vector>

#include "dbformat.h"
#include "slice.h"

namespace leveldb {

class VersionSet;

struct FileMetaData {
  FileMetaData() : refs(0), allowd_seeks(1 << 30), file_size(0) {}

  int refs;
  int allowd_seeks;
  uint64_t number;
  uint64_t file_size;
  InternalKey smallest;
  InternalKey largest;
};

class VersionEdit {
 public:
  VersionEdit() { Clear(); };

  ~VersionEdit() = default;

  void Clear();

  void SetComparatorName(const Slice& name) {
    has_comparator_ = true;
    comparator_ = name.ToString();
  }

  void SetLogNumber(uint64_t num) {
    has_log_number_ = true;
    log_number_ = num;
  }
  void SetPrevLogNumber(uint64_t num) {
    has_prev_log_number_ = true;
    prev_log_number_ = num;
  }
  void SetNextFile(uint64_t num) {
    has_next_file_number_ = true;
    next_file_number_ = num;
  }
  void SetLastSequence(SequenceNumber seq) {
    has_last_sequence_ = true;
    last_sequence_ = seq;
  }

  void SetCompactPointer(int level, const InternalKey& key) {
    compact_pointers_.push_back({level, key});
  }

  void AddFile(int level, uint64_t file, uint64_t file_size,
               const InternalKey& smallest, const InternalKey& largest) {
    FileMetaData f;
    f.number = file;
    f.file_size = file_size;
    f.smallest = smallest;
    f.largest = largest;
    new_files_.push_back({level, f});
  }

  void RemoveFile(int level, uint64_t file) {
    deleted_files_.insert({level, file});
  }

  void EncodeTo(std::string* dst) const;

  std::string DebugString() const;

 private:
  friend class VersionSet;

  typedef std::set<std::pair<int, uint64_t>> DeletedFileSet;

  std::string comparator_;
  uint64_t log_number_;
  uint64_t prev_log_number_;
  uint64_t next_file_number_;

  SequenceNumber last_sequence_;

  bool has_comparator_;
  bool has_log_number_;
  bool has_prev_log_number_;
  bool has_next_file_number_;
  bool has_last_sequence_;

  std::vector<std::pair<int, InternalKey>> compact_pointers_;
  DeletedFileSet deleted_files_;
  std::vector<std::pair<int, FileMetaData>> new_files_;
};

};  // namespace leveldb
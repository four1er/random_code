#include "version_edit.h"

#include "utils/coding.h"

namespace leveldb {

// Tag numbers for serialized VersionEdit.  These numbers are written to
// disk and should not be changed.
enum Tag {
  kComparator = 1,
  kLogNumber = 2,
  kNextFileNumber = 3,
  kLastSequence = 4,
  kCompactPointer = 5,
  kDeletedFile = 6,
  kNewFile = 7,
  // 8 was used for large value refs
  kPrevLogNumber = 9
};

void VersionEdit::Clear() {
  comparator_.clear();
  log_number_ = 0;
  prev_log_number_ = 0;
  last_sequence_ = 0;
  next_file_number_ = 0;
  has_comparator_ = false;
  has_log_number_ = false;
  has_prev_log_number_ = false;
  has_last_sequence_ = false;
  has_next_file_number_ = false;
  compact_pointers_.clear();
  deleted_files_.clear();
  new_files_.clear();
}

void VersionEdit::EncodeTo(std::string* dst) const {
  if (has_comparator_) {
    PutVarint32(dst, kComparator);
    PutLengthPrefixedSlice(dst, comparator_);
  }
  if (has_log_number_) {
    PutVarint32(dst, kLogNumber);
    PutVarint64(dst, log_number_);
  }
  if (has_prev_log_number_) {
    PutVarint32(dst, kPrevLogNumber);
    PutVarint64(dst, prev_log_number_);
  }
  if (has_next_file_number_) {
    PutVarint32(dst, kNextFileNumber);
    PutVarint64(dst, next_file_number_);
  }
  if (has_last_sequence_) {
    PutVarint32(dst, kLastSequence);
    PutVarint64(dst, last_sequence_);
  }

  for (size_t i = 0; i < compact_pointers_.size(); i++) {
    PutVarint32(dst, kCompactPointer);
    PutVarint32(dst, compact_pointers_[i].first);  // level
    PutLengthPrefixedSlice(dst, compact_pointers_[i].second.Encode());
  }

  for (const auto& deleted_file_kvp : deleted_files_) {
    PutVarint32(dst, kDeletedFile);
    PutVarint32(dst, deleted_file_kvp.first);   // level
    PutVarint64(dst, deleted_file_kvp.second);  // file number
  }

  for (size_t i = 0; i < new_files_.size(); i++) {
    const FileMetaData& f = new_files_[i].second;
    PutVarint32(dst, kNewFile);
    PutVarint32(dst, new_files_[i].first);  // level
    PutVarint64(dst, f.number);
    PutVarint64(dst, f.file_size);
    PutLengthPrefixedSlice(dst, f.smallest.Encode());
    PutLengthPrefixedSlice(dst, f.largest.Encode());
  }

  std::string VersionEdit::DebugString() const {
    std::string r;
    r.append("VersionEdit {");
    if (has_comparator_) {
      r.append("\n  Comparator: ");
      r.append(comparator_);
    }
    if (has_log_number_) {
      r.append("\n  LogNumber: ");
      AppendNumberTo(&r, log_number_);
    }
    if (has_prev_log_number_) {
      r.append("\n  PrevLogNumber: ");
      AppendNumberTo(&r, prev_log_number_);
    }
    if (has_next_file_number_) {
      r.append("\n  NextFile: ");
      AppendNumberTo(&r, next_file_number_);
    }
    if (has_last_sequence_) {
      r.append("\n  LastSeq: ");
      AppendNumberTo(&r, last_sequence_);
    }
    for (size_t i = 0; i < compact_pointers_.size(); i++) {
      r.append("\n  CompactPointer: ");
      AppendNumberTo(&r, compact_pointers_[i].first);
      r.append(" ");
      r.append(compact_pointers_[i].second.DebugString());
    }
    for (const auto& deleted_files_kvp : deleted_files_) {
      r.append("\n  RemoveFile: ");
      AppendNumberTo(&r, deleted_files_kvp.first);
      r.append(" ");
      AppendNumberTo(&r, deleted_files_kvp.second);
    }
    for (size_t i = 0; i < new_files_.size(); i++) {
      const FileMetaData& f = new_files_[i].second;
      r.append("\n  AddFile: ");
      AppendNumberTo(&r, new_files_[i].first);
      r.append(" ");
      AppendNumberTo(&r, f.number);
      r.append(" ");
      AppendNumberTo(&r, f.file_size);
      r.append(" ");
      r.append(f.smallest.DebugString());
      r.append(" .. ");
      r.append(f.largest.DebugString());
    }
    r.append("\n}\n");
    return r;
  }
}

}  // namespace leveldb
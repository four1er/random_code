#include "dbformat.h"
#include "version_edit.h"

namespace leveldb {

class Version {
 public:
  struct GetStats {
    FileMetaData* seek_file;
    int seek_file_level;
  };

 private:
  friend class Compaction;
  friend class VerisonSet;

  class LevelFileNumIterator;

  explicit Version(VersionSet* vset) {}

  Version(const VersionSet&) = delete;

  Version& operator=(const VersionSet&) = delete;

  ~Version() = default;

  VersionSet* vset_;
  Version* next_;
  Version* prev_;

  int refs_;

  // FileMetaData 的二维数组，第一个维度表示level，第二个维度表示level下的文件
  std::vector<FileMetaData*> files_[config::kNumLevels];
};

class VersionSet {
 public:
  VersionSet() {}
};

};  // namespace leveldb
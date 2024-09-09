#include <sys/inotify.h>
#include <unistd.h>

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "yaml-cpp/yaml.h"

class YamlDoubleBuffer {
 public:
  static YamlDoubleBuffer& getInstance(const std::string& filename) {
    std::call_once(init_flag_,
                   [&]() { instance_ = new YamlDoubleBuffer(filename); });
    return *instance_;
  }

  const YAML::Node& getYamlNode() const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    return *buffers_[current_buffer_];
  }

 private:
  YamlDoubleBuffer(const std::string& filename)
      : filename_(filename), current_buffer_(false) {
    // 初始化两个缓冲区
    buffers_[0] = std::make_shared<YAML::Node>(YAML::LoadFile(filename_));
    buffers_[1] = std::make_shared<YAML::Node>(YAML::LoadFile(filename_));

    // 启动文件监控线程
    file_monitor_thread_ = std::thread(&YamlDoubleBuffer::fileMonitor, this);
  }

  ~YamlDoubleBuffer() {
    stop_monitor_ = true;
    if (file_monitor_thread_.joinable()) {
      file_monitor_thread_.join();
    }
  }

  void fileMonitor() {
    int fd = inotify_init();
    if (fd < 0) {
      std::cerr << "Failed to initialize inotify" << std::endl;
      return;
    }

    int wd =
        inotify_add_watch(fd, filename_.c_str(), IN_MODIFY | IN_DELETE_SELF);
    if (wd < 0) {
      std::cerr << "Failed to add watch for file" << std::endl;
      close(fd);
      return;
    }

    while (!stop_monitor_) {
      char buffer[4096];
      int length = read(fd, buffer, sizeof(buffer));
      if (length < 0) {
        std::cerr << "Failed to read inotify events" << std::endl;
        break;
      }

      for (char* ptr = buffer; ptr < buffer + length;) {
        struct inotify_event* event =
            reinterpret_cast<struct inotify_event*>(ptr);
        if (event->mask & IN_MODIFY) {
          // 文件被修改，重新加载并更新缓冲区
          reloadYamlFile();
        } else if (event->mask & IN_DELETE_SELF) {
          // 文件被删除，重新加载并更新缓冲区
          reloadYamlFile();
        }
        ptr += sizeof(struct inotify_event) + event->len;
      }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
  }

  void reloadYamlFile() {
    try {
      std::unique_lock<std::shared_mutex> lock(rw_mutex_);
      // 切换缓冲区
      current_buffer_ = !current_buffer_;
      // 重新加载文件并更新缓冲区
      *buffers_[current_buffer_] = YAML::LoadFile(filename_);
    } catch (const YAML::Exception& e) {
      std::cerr << "Failed to reload YAML file: " << e.what() << std::endl;
      // 如果加载失败，切换回原来的缓冲区
      current_buffer_ = !current_buffer_;
    }
  }

  std::string filename_;
  std::shared_ptr<YAML::Node> buffers_[2];
  bool current_buffer_;
  std::thread file_monitor_thread_;
  std::atomic<bool> stop_monitor_{false};
  mutable std::shared_mutex rw_mutex_;

  static std::once_flag init_flag_;
  static YamlDoubleBuffer* instance_;
};

std::once_flag YamlDoubleBuffer::init_flag_;
YamlDoubleBuffer* YamlDoubleBuffer::instance_ = nullptr;

int main() {
  // 获取单例实例
  YamlDoubleBuffer& yaml_buffer = YamlDoubleBuffer::getInstance("test.yaml");

  // 模拟多次读取
  while (1) {
    const YAML::Node& yaml_node = yaml_buffer.getYamlNode();
    std::cout << "Current YAML content:" << std::endl;
    std::cout << yaml_node << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  return 0;
}
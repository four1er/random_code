#include <iostream>

namespace tf {
class Demo1 {
 public:
  Demo1(int mem_a) : mem_a(mem_a) {}
  ~Demo1() {}

 private:
  int mem_a;
};
};  // namespace tf

int main() {
  tf::Demo1 d1(1);
  return 0;
}
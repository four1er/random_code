#include <cassert>
#include <string>
#include <type_traits>

namespace tf {
template <typename T>
T max(const T& a, const T& b) {
  return a < b ? b : a;
}

template <typename RT, typename T, typename U>
RT max(const T& a, const U& b) {
  return a < b ? b : a;
}

template <typename T, typename U, typename RT = std::common_type_t<T, U>>
RT max(const T& a, const U& b) {
  return a < b ? b : a;
}
};  // namespace tf

int main() {
  assert(tf::max<int>(1, 3) == 3);
  assert(tf::max<double>(1.0, 3.0) == 3.0);
  assert(tf::max<std::string>("test", "tast") == "test");
}
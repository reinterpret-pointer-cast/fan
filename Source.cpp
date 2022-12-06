// Creates window, opengl context

#include 	<string.h>
#include <tchar.h>
#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#define FAN_INCLUDE_PATH C:/libs/fan/include
#define fan_debug 0
#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)
#include _FAN_PATH(window/window.h)

struct s0_t {
  int a;
};

struct s1_t : s0_t {
  s1_t(const s0_t& s) : s0_t(s) {}
  s1_t(s0_t&& s) : s0_t(std::move(s)) {}
  int b;
  int c;
};

int main() {
  std::unique_ptr<s0_t> s0 = std::make_unique<s0_t>(s0_t());
  s0.get()->a = 5;
  std::unique_ptr<s1_t> s1 = std::make_unique<s1_t>(s1_t(*std::move(s0)));
  fan::print(s1->a, s0->a);

  return 0;
}
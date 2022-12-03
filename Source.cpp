// Creates window, opengl context

#include 	<string.h>
#include <tchar.h>
#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#define FAN_INCLUDE_PATH C:/libs/fan/include
#define fan_debug 0
#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)
#include _FAN_PATH(window/window.h)

#include <C:/libs/WITCH/WITCH.h>
#include <C:/libs/WITCH/MD/Mice.h>


int main() {
  MD_Mice_t m;
  Sleep(1000);
  MD_Mice_open(&m);
  MD_Mice_Button_write(&m, 3, 0);

  return 0;
}
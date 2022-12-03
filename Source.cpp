// Creates window, opengl context

#include 	<string.h>
#include <tchar.h>
#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#define FAN_INCLUDE_PATH C:/libs/fan/include
#define fan_debug 0
#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)
#include _FAN_PATH(window/window.h)

#include <C:/libs/WITCH/WITCH.h>
#include <C:/libs/WITCH/MD/Keyboard.h>


int main() {
  MD_Keyboard_t kd;
  MD_Keyboard_open(&kd);
  Sleep(1000);
  MD_Keyboard_press_write(&kd, L'ğ');

  return 0;
}
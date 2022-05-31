// Creates window and opengl context

#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#define FAN_INCLUDE_PATH C:/libs/fan/include
#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)

#include _FAN_PATH(graphics/graphics.h)
#include _FAN_PATH(graphics/gui/fgm/fgm.h)

int main() {

  fan_2d::graphics::gui::fgm::pile_t pile;

  pile.open();

  pile.context.set_vsync(&pile.window, 0);

  while (1) {

    uint32_t window_event = pile.window.handle_events();
    if (window_event & fan::window_t::events::close) {
      pile.window.close();
      break;
    }

    pile.context.process();
    pile.context.render(&pile.window);
  }

  return 0;
}
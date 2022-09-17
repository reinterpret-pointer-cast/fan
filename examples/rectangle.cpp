// Creates window, opengl context and renders a rectangle

#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#define FAN_INCLUDE_PATH C:/libs/fan/include
#define fan_debug 0
#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)

#include _FAN_PATH(graphics/graphics.h)

#define loco_window
#define loco_context

#define loco_rectangle
#include _FAN_PATH(graphics/loco.h)

struct pile_t {
  
  static constexpr fan::vec2 ortho_x = fan::vec2(-1, 1);
  static constexpr fan::vec2 ortho_y = fan::vec2(-1, 1);

  void open() {
    loco.open(loco_t::properties_t());
    fan::vec2 window_size = loco.get_window()->get_size();
    fan::graphics::open_matrices(
      loco.get_context(),
      &matrices,
      ortho_x,
      ortho_y
    );
    loco.get_window()->add_resize_callback([&](fan::window_t* window, const fan::vec2i& size) {
      fan::vec2 window_size = window->get_size();
      fan::vec2 ratio = window_size / window_size.max();
      std::swap(ratio.x, ratio.y);
      matrices.set_ortho(
        ortho_x * ratio.x, 
        ortho_y * ratio.y
      );
    });
    viewport.open(loco.get_context());
    viewport.set(loco.get_context(), 0, window_size, window_size);
  }
  fan::opengl::matrices_t matrices;
  fan::opengl::viewport_t viewport;

  loco_t loco;
  fan::opengl::cid_t cid;
};

int main() {

  pile_t* pile = new pile_t;
  pile->open();

  loco_t::rectangle_t::properties_t p;

  p.size = 0.5;
  //p.block_properties.
  p.get_matrices() = &pile->matrices;
  p.get_viewport() = &pile->viewport;

  p.position = fan::vec2(0, 0);
  p.color = fan::random::color();
  pile->loco.rectangle.push_back(&pile->cid, p);

  pile->loco.set_vsync(false);

  pile->loco.loop([&] {
    pile->loco.get_fps();
  });
  return 0;
}
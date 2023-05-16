// Creates window, opengl context and renders a rectangle

#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#ifndef FAN_INCLUDE_PATH
  #define FAN_INCLUDE_PATH C:/libs/fan/include
#endif
#define fan_debug 0
#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)
#define loco_window
#define loco_context

#define loco_sprite
#define loco_rectangle
#define loco_responsive_text
#define loco_text
#include _FAN_PATH(graphics/loco.h)

struct pile_t {

  static constexpr fan::vec2 ortho_x = fan::vec2(0, 800);
  static constexpr fan::vec2 ortho_y = fan::vec2(0, 800);

  pile_t() {
    loco.open_camera(
      &camera,
      ortho_x,
      ortho_y
    );
    loco.get_window()->add_resize_callback([&](const fan::window_t::resize_cb_data_t& d) {
      viewport.set(0, d.size, d.size);
    });
    viewport.open();
    viewport.set(0, loco.get_window()->get_size(), loco.get_window()->get_size());
  }

  loco_t loco;
  loco_t::camera_t camera;
  loco_t::viewport_t viewport;
};

struct sprite_responsive_t : loco_t::responsive_text_t {
  using responsive_text_t::responsive_text_t;

  sprite_responsive_t(const loco_t::text_t::properties_t& tp, const loco_t::sprite_t::properties_t& p) {
    loco_t::responsive_text_t::properties_t rp;
    *(loco_t::text_t::properties_t *)&rp = tp;
    rp.position = p.position;
    rp.boundary = p.size;
    rp.camera = p.camera;
    rp.viewport = p.viewport;
    *(loco_t::responsive_text_t*)this = rp;
    base = p;
  }

  void set_size(const fan::vec2& s) {
    base.set_size(s);
    responsive_text_t::set_size(s);
  }

  loco_t::shape_t base;
};

int main() {
  pile_t* pile = new pile_t;

  loco_t::image_t image;
  image.load("images/brick.webp");

  loco_t::sprite_t::properties_t pp;

  pp.size = fan::vec2(1);
  pp.camera = &pile->camera;
  pp.viewport = &pile->viewport;  
  pp.image = &image;
  
  pp.position = fan::vec2(400, 400);
  pp.size = fan::vec2(100, 100);
  pp.color.a = 1;
  pp.blending = true;

  loco_t::rectangle_t::properties_t rp;
  rp.camera = &pile->camera;
  rp.viewport = &pile->viewport;

  rp.position = fan::vec3(400, 400, 0);
  rp.size = fan::vec2(100);
  rp.color = fan::colors::red;
  
  loco_t::text_t::properties_t tp;
  tp.text = "WWiWWWWWWWWWWWWWWWWWWWWWWWW";
  for (uint32_t i = 1; i--;) tp.text += tp.text;
  tp.font_size = 1;
  sprite_responsive_t responsive_box(tp, pp);

  /*{
    uint32_t i = 0;
    while (responsive_box.does_text_fit(std::to_string(i))) {
      responsive_box.push_back(std::to_string(i));
      i++;
    }
  }*/


  //fan::vec2
//  responsive_box.set_size(fan::vec2(1, 100));

  gloco->get_window()->add_keys_callback([&](const auto& d) {
    if (d.key == fan::key_up) {
      pp.size.x += 1;
      responsive_box.set_size(pp.size);
    }
    if (d.key == fan::key_down) {
      pp.size.x -= 1;
      responsive_box.set_size(pp.size);
    }
  });

  pile->loco.loop([&] {
    pile->loco.get_fps();

    //r0.set_position(pile->loco.get_mouse_position(pile->camera, pile->viewport));
  });

  return 0;
}
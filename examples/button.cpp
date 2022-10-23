// rectangle text button using loco

#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#ifndef FAN_INCLUDE_PATH
  #define FAN_INCLUDE_PATH C:/libs/fan/include
#endif

#define fan_debug 0

#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)

#define loco_window
#define loco_context

#define loco_post_process
#define loco_button
#include _FAN_PATH(graphics/loco.h)

struct pile_t {

  void open() {
    loco.open(loco_t::properties_t());
    fan::vec2 window_size = loco.get_window()->get_size();
    fan::vec2 ratio = window_size / window_size.max();
    fan::graphics::open_matrices(
      loco.get_context(),
      &matrices,
      fan::vec2(-1, 1) * ratio.x,
      fan::vec2(-1, 1) * ratio.y
    );
    loco.get_window()->add_resize_callback([&](const fan::window_t::resize_cb_data_t& d) {
      fan::vec2 window_size = d.window->get_size();
      fan::vec2 ratio = window_size / window_size.max();
      //std::swap(ratio.x, ratio.y);
      //pile_t* pile = (pile_t*)userptr;
      matrices.set_ortho(
        fan::vec2(-1, 1) * ratio.x,
        fan::vec2(-1, 1) * ratio.y
      );
      viewport.set(loco.get_context(), 0, loco.get_window()->get_size(), loco.get_window()->get_size());
     });

    fan::vec2 position = 0;
    fan::vec2 size = loco.get_window()->get_size();
    //position.y -= 200;
    //position.y += size.y / 2;
    //size.y /= 2;
    viewport.open(loco.get_context());
    viewport.set(loco.get_context(), position, size, loco.get_window()->get_size());
  }

  loco_t loco;
  fan::opengl::matrices_t matrices;
  fan::opengl::viewport_t viewport;
};

int main() {

  pile_t pile;
  pile.open();

  loco_t::button_t::properties_t tp;
  tp.get_matrices() = &pile.matrices;
  tp.get_viewport() = &pile.viewport;
 // tp.position = 400;
  tp.position = 0;
  //tp.position.y = 0;
 // tp.position.z = 50;
  tp.font_size = 0.4;
  tp.size = fan::vec2(0.3, 0.1) 
    //* 300
    ;
  tp.text = L"Testiteksti";
  //tp.font_size = 32;
  tp.mouse_move_cb = [] (const loco_t::mouse_move_data_t& mm_d) -> int {
    //fan::print(mm_d.position, (int)mm_d.mouse_stage);
    return 0;
  };
  tp.mouse_button_cb = [](const loco_t::mouse_button_data_t& ii_d) -> int {
   /* if (ii_d.button_state == fan::key_state::press) {
      ii_d.flag->ignore_move_focus_check = true;
    }
    else {
      ii_d.flag->ignore_move_focus_check = false;
    }*/
    return 0;
  };
  fan_2d::graphics::gui::theme_t gray_theme = fan_2d::graphics::gui::themes::deep_red();
  gray_theme.open(pile.loco.get_context());
  tp.theme = &gray_theme;
  constexpr auto count = 10;
  fan::opengl::cid_t cids[count];
  fan::print(loco_bdbt_usage(&pile.loco.bdbt));
  pile.loco.button.push_back(&cids[0], tp);
  //pile.loco.get_context()->opengl.glPolygonMode(fan::opengl::GL_FRONT_AND_BACK, fan::opengl::GL_LINE);
  //pile.loco.get_window()->set_background_color(fan::colors::red);
  //pile.loco.button.set_theme(&cids[0], &gray_theme, 0.1);
  pile.loco.loop([&] {
    //pile.loco.button.set(&cids[0], &loco_t::button_t::instance_t::position, fan::vec2(-0.5, .5));
  });

  return 0;
}
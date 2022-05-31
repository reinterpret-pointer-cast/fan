case fan::key_delete: {
  switch (key_state) {
    case fan::key_state::press: {
      switch (pile->editor.selected_type) {
        case builder_draw_type_t::rectangle_text_button_sized: {
          #include _FAN_PATH(graphics/gui/fgm/rectangle_sized_text_button/erase_active.h)
          break;
        }
        case builder_draw_type_t::text_renderer_clickable: {
          #include _FAN_PATH(graphics/gui/fgm/text_renderer_clickable/erase_active.h)
          break;
        }
      }
    }
  }
}
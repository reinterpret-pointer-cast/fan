void open(auto& loco) {
  
}

void close(auto& loco){
  
}

void window_resize_callback(auto& loco){
		
}

void update(auto& loco){
	
}

int button0_mouse_button_cb(const loco_t::mouse_button_data_t& mb) {
  if (mb.button != fan::mouse_left) {
    return 0;
  }
  if (mb.button_state != fan::mouse_state::release) {
    return 0;
  }
  game::pile->StageList.Remove(this->parent_id);
  game::pile->StageList.Remove(this->stage_id);
  game::pile->StageList.Add<game::pile_t::stage2_t>();
  return 1;
}

int button0_mouse_move_cb(const loco_t::mouse_move_data_t& mb) {
  return 0;
}

int button0_keyboard_cb(const loco_t::keyboard_data_t& mb) {
  return 0;
}

int button0_text_cb(const loco_t::text_data_t& mb) {
  return 0;
}

int button1_mouse_button_cb(const loco_t::mouse_button_data_t& mb) {
  return 0;
}

int button1_mouse_move_cb(const loco_t::mouse_move_data_t& mb) {
  return 0;
}

int button1_keyboard_cb(const loco_t::keyboard_data_t& mb) {
  return 0;
}

int button1_text_cb(const loco_t::text_data_t& mb) {
  return 0;
}
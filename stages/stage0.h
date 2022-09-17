stage_common_t stage_common = {
	.open = [this] () {
		
	},
	.close = [this] {
		
	},
	.window_resize_callback = [this] {
		
	},
	.update = [this] {
		
	}
};

static void lib_open(loco_t* loco, stage_common_t* sc, const stage_common_t::open_properties_t& op) {

	sc->instances.open();

	std::string fgm_name = fan::file_name(__FILE__);
	fgm_name.pop_back(); // remove
	fgm_name.pop_back(); // .h
	std::string full_path = std::string("stages/") + fgm_name + ".fgm";
	std::string f;
	if (!fan::io::file::exists(full_path)) {
		return;
	}
	fan::io::file::read(full_path, &f);
	uint64_t off = 0;
	uint32_t instance_count = fan::io::file::read_data<uint32_t>(f, off);
	for (uint32_t i = 0; i < instance_count; i++) {
		auto p = fan::io::file::read_data<fan::vec3>(f, off);
		auto s = fan::io::file::read_data<fan::vec2>(f, off);
		auto fs = fan::io::file::read_data<f32_t>(f, off);
		auto text = fan::io::file::read_data<std::string>(f, off);
		fan::io::file::read_data<fan_2d::graphics::gui::theme_t>(f, off);
		typename loco_t::button_t::properties_t bp;
		bp.position = p;
		bp.size = s;
		bp.font_size = fs;
		bp.text = text;
		bp.theme = op.theme;
		bp.get_matrices() = op.matrices;
		bp.get_viewport() = op.viewport;
		bp.mouse_button_cb = mouse_button_cb0;
		auto nr = sc->instances.NewNodeLast();

		loco->button.push_back(&sc->instances[nr].cid, bp);
	}
}

static void lib_close(stage_common_t* sc) {
	sc->instances.close();
}

static int mouse_button_cb0(const loco_t::mouse_button_data_t& mb){

	if (mb.button != fan::mouse_left) {
		return 0;
	}
	if (mb.button_state != fan::key_state::release) {
		return 0;
	}

	pile_t* pile = OFFSETLESS(OFFSETLESS(mb.vfi, loco_t, vfi), pile_t, loco);

	using sl = pile_t::stage_loader_t;

	sl::stage_common_t::open_properties_t op;
	op.matrices = &pile->matrices;
	op.viewport = &pile->viewport;
	op.theme = &pile->theme;
	pile->stage_loader.push_and_open_stage<sl::stage::stage1_t>(op);

  return 0;
}
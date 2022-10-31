struct button_t {

  static constexpr f32_t inactive = 1.0;
  static constexpr f32_t hover = 1.2;
  static constexpr f32_t press = 1.4;

  struct instance_t {
    fan::vec3 position = 0;
    f32_t angle = 0;
    fan::vec2 size = 0;
    fan::vec2 rotation_point = 0;
    fan::color color = fan::colors::white;
    fan::color outline_color;
    fan::vec3 rotation_vector = fan::vec3(0, 0, 1);
    f32_t outline_size;
  };

  static constexpr uint32_t max_instance_size = fan::min(256, 4096 / (sizeof(instance_t) / 4));

  #define hardcode0_t loco_t::matrices_list_NodeReference_t
  #define hardcode0_n matrices
  #define hardcode1_t fan::graphics::viewport_list_NodeReference_t
  #define hardcode1_n viewport
  #include _FAN_PATH(graphics/opengl/2D/objects/hardcode_open.h)

  struct instance_properties_t {
    struct key_t : parsed_masterpiece_t {}key;

    expand_get_functions

    uint8_t selected;
    fan::graphics::theme_list_NodeReference_t theme;
    uint32_t text_id;
    loco_t::vfi_t::shape_id_t vfi_id;
    uint64_t udata;
  };

  struct properties_t : instance_t, instance_properties_t {
    properties_t() {
      selected = 0;
    }

    fan::wstring text;
    f32_t font_size = 0.1;

    loco_t::vfi_t::iflags_t vfi_flags;

    bool disable_highlight = false;

    loco_t::mouse_button_cb_t mouse_button_cb = [](const loco_t::mouse_button_data_t&) -> int { return 0; };
    loco_t::mouse_move_cb_t mouse_move_cb = [](const loco_t::mouse_move_data_t&) -> int { return 0; };
    loco_t::keyboard_cb_t keyboard_cb = [](const loco_t::keyboard_data_t&) -> int { return 0; };
  };

  void push_back(fan::graphics::cid_t* cid, properties_t& p) {
    loco_t* loco = get_loco();

    #if defined(loco_vulkan)
      auto& matrices = loco->matrices_list[p.get_matrices()];
      if (matrices.matrices_index.button == (decltype(matrices.matrices_index.button))-1) {
        matrices.matrices_index.button = m_matrices_index++;
        m_shader.set_matrices(loco, matrices.matrices_id, &loco->m_write_queue, matrices.matrices_index.button);
      }
    #endif

    auto theme = loco->button.get_theme(p.theme);
    loco_t::text_t::properties_t tp;
    tp.color = theme->button.text_color;
    tp.font_size = p.font_size;
    tp.position = p.position;
    tp.text = p.text;
    tp.position.z += p.position.z + 0.5;
    tp.get_viewport() = p.get_viewport();
    tp.get_matrices() = p.get_matrices();
    auto block = sb_push_back(cid, p);
    block->p[cid->instance_id].text_id = loco->text.push_back(tp);

    set_theme(cid, theme, inactive);

    loco_t::vfi_t::properties_t vfip;
    vfip.shape_type = loco_t::vfi_t::shape_t::rectangle;
    vfip.shape.rectangle.matrices = p.get_matrices();
    vfip.shape.rectangle.viewport = p.get_viewport();
    vfip.shape.rectangle.position = p.position;
    vfip.shape.rectangle.size = p.size;
    vfip.flags = p.vfi_flags;
    if (!p.disable_highlight) {
      vfip.mouse_move_cb = [cb = p.mouse_move_cb, udata = p.udata, cid_ = cid](const loco_t::vfi_t::mouse_move_data_t& mm_d) -> int {
        loco_t* loco = OFFSETLESS(mm_d.vfi, loco_t, vfi_var_name);
        loco_t::mouse_move_data_t mmd = mm_d;
        auto block = loco->button.sb_get_block(cid_);
        if (mm_d.flag->ignore_move_focus_check == false && !block->p[cid_->instance_id].selected) {
          if (mm_d.mouse_stage == loco_t::vfi_t::mouse_stage_e::inside) {
            loco->button.set_theme(cid_, loco->button.get_theme(cid_), hover);
          }
          else {
            loco->button.set_theme(cid_, loco->button.get_theme(cid_), inactive);
          }
        }
        mmd.cid = cid_;
        cb(mmd);
        return 0;
      };
      vfip.mouse_button_cb = [cb = p.mouse_button_cb, udata = p.udata, cid_ = cid](const loco_t::vfi_t::mouse_button_data_t& ii_d) -> int {
        loco_t* loco = OFFSETLESS(ii_d.vfi, loco_t, vfi_var_name);
        auto block = loco->button.sb_get_block(cid_);
        if (ii_d.flag->ignore_move_focus_check == false && !block->p[cid_->instance_id].selected) {
          if (ii_d.button == fan::mouse_left && ii_d.button_state == fan::mouse_state::press) {
            loco->button.set_theme(cid_, loco->button.get_theme(cid_), press);
            ii_d.flag->ignore_move_focus_check = true;
            loco->vfi.set_focus_keyboard(loco->vfi.get_focus_mouse());
          }
        }
        else if (!block->p[cid_->instance_id].selected) {
          if (ii_d.button == fan::mouse_left && ii_d.button_state == fan::mouse_state::release) {
            if (ii_d.mouse_stage == loco_t::vfi_t::mouse_stage_e::inside) {
              loco->button.set_theme(cid_, loco->button.get_theme(cid_), hover);
            }
            else {
              loco->button.set_theme(cid_, loco->button.get_theme(cid_), inactive);
            }
            ii_d.flag->ignore_move_focus_check = false;
          }
        }

        loco_t::mouse_button_data_t mid = ii_d;
        mid.cid = cid_;
        cb(mid);

        return 0;
      };
      vfip.keyboard_cb = [cb = p.keyboard_cb, udata = p.udata, cid_ = cid](const loco_t::vfi_t::keyboard_data_t& kd) -> int {
        loco_t* loco = OFFSETLESS(kd.vfi, loco_t, vfi_var_name);
        loco_t::keyboard_data_t kd_ = kd;
        auto block = loco->button.sb_get_block(cid_);
        kd_.cid = cid_;
        cb(kd_);
        return 0;
      };
    }

    block->p[cid->instance_id].vfi_id = loco->vfi.push_shape(vfip);
  }
  void erase(fan::graphics::cid_t* cid) {
    loco_t* loco = get_loco();
    auto block = sb_get_block(cid);
    instance_properties_t* p = &block->p[cid->instance_id];
    loco->text.erase(p->text_id);
    loco->vfi.erase(block->p[cid->instance_id].vfi_id);
    sb_erase(cid);
  }

  instance_properties_t* get_instance_properties(fan::graphics::cid_t* cid) {
    return &sb_get_block(cid)->p[cid->instance_id];
  }

  void draw() {
    sb_draw();
  }

  #if defined(loco_opengl)
    #define sb_shader_vertex_path _FAN_PATH(graphics/glsl/opengl/2D/objects/button.vs)
  #define sb_shader_fragment_path _FAN_PATH(graphics/glsl/opengl/2D/objects/button.fs)
  #elif defined(loco_vulkan)
    #define vulkan_buffer_count 2
    #define sb_shader_vertex_path _FAN_PATH_QUOTE(graphics/glsl/vulkan/2D/objects/button.vert.spv)
    #define sb_shader_fragment_path _FAN_PATH_QUOTE(graphics/glsl/vulkan/2D/objects/button.frag.spv)
  #endif

  #include _FAN_PATH(graphics/shape_builder.h)

  button_t() {
     #if defined(loco_opengl)
      sb_open();
    #elif defined(loco_vulkan)
      std::array<fan::vulkan::write_descriptor_set_t, vulkan_buffer_count> ds_properties;

      ds_properties[0].binding = 0;
      ds_properties[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      ds_properties[0].flags = VK_SHADER_STAGE_VERTEX_BIT;
      ds_properties[0].range = VK_WHOLE_SIZE;
      ds_properties[0].common = &m_ssbo.common;
      ds_properties[0].dst_binding = 0;

      ds_properties[1].binding = 1;
      ds_properties[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      ds_properties[1].flags = VK_SHADER_STAGE_VERTEX_BIT;
      ds_properties[1].common = &m_shader.projection_view_block.common;
      ds_properties[1].range = sizeof(fan::mat4) * 2;
      ds_properties[1].dst_binding = 1;

      sb_open(ds_properties);
    #endif
  }
  ~button_t() {
    sb_close();
  }

  fan_2d::graphics::gui::theme_t* get_theme(fan::graphics::theme_list_NodeReference_t nr) {
    loco_t* loco = get_loco();
    return loco->get_context()->theme_list[nr].theme_id;
  }
  fan_2d::graphics::gui::theme_t* get_theme(fan::graphics::cid_t* cid) {
    return get_theme(blocks[*(bll_block_NodeReference_t*)&cid->block_id].block.p[cid->instance_id].theme);
  }
  void set_theme(fan::graphics::cid_t* cid, fan_2d::graphics::gui::theme_t* theme, f32_t intensity) {
    loco_t* loco = get_loco();
    fan_2d::graphics::gui::theme_t t = *theme;
    t = t * intensity;
    set(cid, &instance_t::color, t.button.color);
    set(cid, &instance_t::outline_color, t.button.outline_color);
    set(cid, &instance_t::outline_size, t.button.outline_size);
    auto block = sb_get_block(cid);
    block->p[cid->instance_id].theme = theme;
    loco->text.set(block->p[cid->instance_id].text_id, 
      &loco_t::letter_t::instance_t::outline_color, t.button.text_outline_color);
    loco->text.set(block->p[cid->instance_id].text_id, 
      &loco_t::letter_t::instance_t::outline_size, t.button.text_outline_size);
  }

  template <typename T>
  T get_button(fan::graphics::cid_t* cid, T instance_t::* member) {
    loco_t* loco = get_loco();
    return loco->button.get(cid, member);
  }
  template <typename T, typename T2>
  void set_button(fan::graphics::cid_t* cid, T instance_t::*member, const T2& value) {
    loco_t* loco = get_loco();
    loco->button.set(cid, member, value);
  }

  template <typename T>
  T get_text_renderer(fan::graphics::cid_t* cid, T loco_t::letter_t::instance_t::* member) {
    loco_t* loco = get_loco();
    auto block = sb_get_block(cid);
    return loco->text.get(block->p[cid->instance_id].text_id, member);
  }
  template <typename T, typename T2>
  void set_text_renderer(fan::graphics::cid_t* cid, T loco_t::letter_t::instance_t::*member, const T2& value) {
    loco_t* loco = get_loco();
    auto block = sb_get_block(cid);
    loco->text.set(block->p[cid->instance_id].text_id, member, value);
  }

  void set_position(fan::graphics::cid_t* cid, const fan::vec3& position) {
    loco_t* loco = get_loco();
    auto block = sb_get_block(cid);
    loco->text.set_position(&block->p[cid->instance_id].text_id, position + fan::vec3(0, 0, 0.5));
    set_button(cid, &instance_t::position, position);
    loco->vfi.set_rectangle(
      block->p[cid->instance_id].vfi_id,
      &loco_t::vfi_t::set_rectangle_t::position,
      position
    );
  }
  void set_size(fan::graphics::cid_t* cid, const fan::vec3& size) {
    loco_t* loco = get_loco();
    auto block = sb_get_block(cid);
    set_button(cid, &instance_t::size, size);
    loco->vfi.set_rectangle(
      block->p[cid->instance_id].vfi_id,
      &loco_t::vfi_t::set_rectangle_t::size,
      size
    );
  }

  loco_t::matrices_t* get_matrices(fan::graphics::cid_t* cid) {
    auto block = sb_get_block(cid);
    loco_t* loco = get_loco();
    return loco->matrices_list[*block->p[cid->instance_id].key.get_value<0>()].matrices_id;
  }
  void set_matrices(fan::graphics::cid_t* cid, loco_t::matrices_list_NodeReference_t n) {
    sb_set_key<instance_properties_t::key_t::get_index_with_type<decltype(n)>()>(cid, n);
    loco_t* loco = get_loco();
    auto block = sb_get_block(cid);
    loco->text.set_matrices(block->p[cid->instance_id].text_id, n);
  }

  fan::graphics::viewport_t* get_viewport(fan::graphics::cid_t* cid) {
    loco_t* loco = get_loco();
    auto block = sb_get_block(cid);
    return loco->get_context()->viewport_list[*block->p[cid->instance_id].key.get_value<1>()].viewport_id;
  }
  void set_viewport(fan::graphics::cid_t* cid, fan::graphics::viewport_list_NodeReference_t n) {
    loco_t* loco = get_loco();
    sb_set_key<instance_properties_t::key_t::get_index_with_type<decltype(n)>()>(cid, n);
    auto block = sb_get_block(cid);
    loco->text.set_viewport(block->p[cid->instance_id].text_id, n);
  }

  void set_theme(fan::graphics::cid_t* cid, f32_t state) {
    loco_t* loco = get_loco();
    loco->button.set_theme(cid, loco->button.get_theme(cid), state);
  }

    // gets udata from current focus
  /*uint64_t get_id_udata(loco_t::vfi_t::shape_id_t id) {
    loco_t* loco = get_loco();
    auto udata = loco->vfi.get_id_udata(id);
    fan::opengl::cid_t* cid = (fan::opengl::cid_t*)udata;
    auto block = sb_get_block(cid);
    return block->p[cid->instance_id].udata;
  }*/

  void set_selected(fan::graphics::cid_t* cid, bool flag) {
    auto block = sb_get_block(cid);
    block->p[cid->instance_id].selected = flag;
  }

  fan::wstring get_text(fan::graphics::cid_t* cid) {
    loco_t* loco = get_loco();
    auto block = sb_get_block(cid);
    return loco->text.get_properties(block->p[cid->instance_id].text_id).text;
  }
  void set_text(fan::graphics::cid_t* cid, const fan::wstring& text) {
    loco_t* loco = get_loco();
    auto block = sb_get_block(cid);
    loco->text.set_text(&block->p[cid->instance_id].text_id, text);
  }

  #if defined(loco_vulkan)
  uint32_t m_matrices_index = 0;
  #endif
};

#include _FAN_PATH(graphics/opengl/2D/objects/hardcode_close.h)
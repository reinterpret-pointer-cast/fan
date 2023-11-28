static constexpr fan::vec4_wrap_t<fan::vec2> default_texture_coordinates = fan::vec4_wrap_t<fan::vec2>(
  fan::vec2(0, 0),
  fan::vec2(1, 0),
  fan::vec2(1, 1),
  fan::vec2(0, 1)
);

struct image_t {

  struct format {
    static constexpr auto b8g8r8a8_unorm = fan::opengl::GL_BGRA;
    static constexpr auto r8_unorm = fan::opengl::GL_RED;
    static constexpr auto rg8_unorm = fan::opengl::GL_RG;
  };

  struct sampler_address_mode {
    static constexpr auto repeat = fan::opengl::GL_REPEAT;
    static constexpr auto mirrored_repeat = fan::opengl::GL_MIRRORED_REPEAT;
    static constexpr auto clamp_to_edge = fan::opengl::GL_CLAMP_TO_EDGE;
    static constexpr auto clamp_to_border = fan::opengl::GL_CLAMP_TO_BORDER;
    static constexpr auto mirrored_clamp_to_edge = fan::opengl::GL_MIRROR_CLAMP_TO_EDGE;
  };

  struct filter {
    static constexpr auto nearest = fan::opengl::GL_NEAREST;
    static constexpr auto linear = fan::opengl::GL_LINEAR;
  };

  struct load_properties_defaults {
    static constexpr uint32_t visual_output = fan::opengl::GL_REPEAT;
    static constexpr uint32_t internal_format = fan::opengl::GL_RGBA;
    static constexpr uint32_t format = fan::opengl::GL_RGBA;
    static constexpr uint32_t type = fan::opengl::GL_UNSIGNED_BYTE;
    static constexpr uint32_t min_filter = fan::opengl::GL_NEAREST;
    static constexpr uint32_t mag_filter = fan::opengl::GL_NEAREST;
  };

  struct load_properties_t {
    uint32_t            visual_output = load_properties_defaults::visual_output;
    uintptr_t           internal_format = load_properties_defaults::internal_format;
    uintptr_t           format = load_properties_defaults::format;
    uintptr_t           type = load_properties_defaults::type;
    uintptr_t           min_filter = load_properties_defaults::min_filter;
    uintptr_t           mag_filter = load_properties_defaults::mag_filter;
  };

  /*
        void open(fan::opengl::context_t* context, const fan::vec2& viewport_position_, const fan::vec2& viewport_size_) {
    viewport_reference = viewport_list_NewNode(&context.viewport_list);
    auto node = viewport_list_GetNodeByReference(&context.viewport_list, viewport_reference);
    node->data.viewport_id = this;
  }
  void close(fan::opengl::context_t* context) {
    viewport_list_Recycle(&context.viewport_list, viewport_reference);
  }
  */

  image_t() {
    texture_reference.NRI = -1;
  }
  //image_t() {
  //  fan::webp::image_info_t image_info;
  //  image_info.data = 0;
  //  image_info.size = 0;
  //  load_properties_t p;
  //  load(image_info, p);
  //}

  image_t(const fan::webp::image_info_t image_info) {
    load(image_info, load_properties_t());
  }

  image_t(const fan::webp::image_info_t image_info, load_properties_t p) {
    load(image_info, p);
  }

  image_t(const char* path) {
    load(path);
  }

  bool is_invalid() const {
    return texture_reference.NRI == (decltype(texture_reference.NRI))-1;
  }

  void create_texture() {
    auto& context = gloco->get_context();
    texture_reference = gloco->image_list.NewNode();
    gloco->image_list[texture_reference].image = this;
    context.opengl.call(context.opengl.glGenTextures, 1, &get_texture());
  }
  void erase_texture() {
    auto& context = gloco->get_context();
    context.opengl.glDeleteTextures(1, &get_texture());
    gloco->image_list.Recycle(texture_reference);
    texture_reference.NRI = -1;
  }

  void bind_texture() {
    auto& context = gloco->get_context();
    context.opengl.call(context.opengl.glBindTexture, fan::opengl::GL_TEXTURE_2D, get_texture());
  }

  void unbind_texture() {
    auto& context = gloco->get_context();
    context.opengl.call(context.opengl.glBindTexture, fan::opengl::GL_TEXTURE_2D, 0);
  }

  fan::opengl::GLuint& get_texture() {
    return gloco->image_list[texture_reference].texture_id;
  }

  bool load(fan::webp::image_info_t image_info) {
    return load(image_info, load_properties_t());
  }

  bool load(fan::webp::image_info_t image_info, load_properties_t p) {

    auto& context = gloco->get_context();

    create_texture();
    bind_texture();
    
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_S, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_T, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MIN_FILTER, p.min_filter);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MAG_FILTER, p.mag_filter);

    size = image_info.size;

    context.opengl.call(context.opengl.glTexImage2D, fan::opengl::GL_TEXTURE_2D, 0, p.internal_format, size.x, size.y, 0, p.format, p.type, image_info.data);

    switch (p.min_filter) {
      case fan::opengl::GL_LINEAR_MIPMAP_LINEAR:
      case fan::opengl::GL_NEAREST_MIPMAP_LINEAR:
      case fan::opengl::GL_LINEAR_MIPMAP_NEAREST:
      case fan::opengl::GL_NEAREST_MIPMAP_NEAREST: {
        context.opengl.call(context.opengl.glGenerateMipmap, fan::opengl::GL_TEXTURE_2D);
        break;
      }
    }
        
    return 0;
  }

  // returns 0 on success
  bool load(const fan::string& path) {
    return load(path, load_properties_t());
  }

  bool load(const fan::string& path, const load_properties_t& p) {

    #if fan_assert_if_same_path_loaded_multiple_times
    
    static std::unordered_map<fan::string, bool> existing_images;

    if (existing_images.find(path) != existing_images.end()) {
      fan::throw_error("image already existing " + path);
    }

    existing_images[path] = 0;

    #endif

    fan::webp::image_info_t image_info;
    if (fan::webp::load(path, &image_info)) {
      return true;
    }
    bool ret = load(image_info, p);
    fan::webp::free_image(image_info.data);
    return ret;
  }

  bool load(fan::color* colors, const fan::vec2ui& size_) {
    return load( colors, size_, load_properties_t());
  }

  bool load(fan::color* colors, const fan::vec2ui& size_, load_properties_t p) {

    auto& context = gloco->get_context();

    create_texture();
    bind_texture();

    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_S, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_T, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MIN_FILTER, p.min_filter);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MAG_FILTER, p.mag_filter);

    size = size_;

    context.opengl.call(context.opengl.glTexImage2D, fan::opengl::GL_TEXTURE_2D, 0, fan::opengl::GL_RGBA32F, size.x, size.y, 0, p.format, fan::opengl::GL_FLOAT, (uint8_t*)colors);

    return 0;
  }

  void reload_pixels(const fan::webp::image_info_t& image_info) {
    reload_pixels(image_info, load_properties_t());
  }

  void reload_pixels(const fan::webp::image_info_t& image_info, const load_properties_t& p) {

    auto& context = gloco->get_context();

    bind_texture();

    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_S, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_T, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MIN_FILTER, p.min_filter);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MAG_FILTER, p.mag_filter);

    size = image_info.size;
    context.opengl.call(context.opengl.glTexImage2D, fan::opengl::GL_TEXTURE_2D, 0, p.internal_format, size.x, size.y, 0, p.format, p.type, image_info.data);
    
  }

  void unload() {
    erase_texture();
  }

  void create(const fan::color& color, const fan::vec2& size_) {
    create(color, size_, load_properties_t());
  }

  // creates single colored text size.x*size.y sized
  void create(const fan::color& color, const fan::vec2& size_, load_properties_t p) {
    auto& context = gloco->get_context();


    size = size_;

    uint8_t* pixels = (uint8_t*)malloc(sizeof(uint8_t) * (size.x * size.y * fan::color::size()));
    for (int y = 0; y < size_.y; y++) {
      for (int x = 0; x < size_.x; x++) {
        for (int p = 0; p < fan::color::size(); p++) {
          *pixels = color[p] * 255;
          pixels++;
        }
      }
    }

    pixels -= (int)size.x * (int)size.y * fan::color::size();

    create_texture();
    bind_texture();

    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_S, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_T, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MIN_FILTER, p.min_filter);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MAG_FILTER, p.mag_filter);

    context.opengl.call(context.opengl.glTexImage2D, fan::opengl::GL_TEXTURE_2D, 0, p.internal_format, size.x, size.y, 0, p.format, p.type, pixels);

    free(pixels);

    context.opengl.call(context.opengl.glGenerateMipmap, fan::opengl::GL_TEXTURE_2D);
  }

  void create_missing_texture() {
    create_missing_texture(load_properties_t());
  }

  void create_missing_texture(load_properties_t p) {
    auto& context = gloco->get_context();

    uint8_t* pixels = (uint8_t*)malloc(sizeof(uint8_t) * (2 * 2 * fan::color::size()));
    uint32_t pixel = 0;

    pixels[pixel++] = 0;
    pixels[pixel++] = 0;
    pixels[pixel++] = 0;
    pixels[pixel++] = 255;

    pixels[pixel++] = 255;
    pixels[pixel++] = 0;
    pixels[pixel++] = 220;
    pixels[pixel++] = 255;

    pixels[pixel++] = 255;
    pixels[pixel++] = 0;
    pixels[pixel++] = 220;
    pixels[pixel++] = 255;

    pixels[pixel++] = 0;
    pixels[pixel++] = 0;
    pixels[pixel++] = 0;
    pixels[pixel++] = 255;

    p.visual_output = fan::opengl::GL_REPEAT;

    create_texture();
    bind_texture();

    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_S, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_T, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MIN_FILTER, p.min_filter);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MAG_FILTER, p.mag_filter);

    size = fan::vec2i(2, 2);

    context.opengl.call(context.opengl.glTexImage2D, fan::opengl::GL_TEXTURE_2D, 0, p.internal_format, 2, 2, 0, p.format, p.type, pixels);

    free(pixels);

    context.opengl.call(context.opengl.glGenerateMipmap, fan::opengl::GL_TEXTURE_2D);
  }

  void create_transparent_texture() {
    load_properties_t p;
    auto& context = gloco->get_context();

    uint8_t* pixels = (uint8_t*)malloc(sizeof(uint8_t) * (2 * 2 * fan::color::size()));
    uint32_t pixel = 0;

    pixels[pixel++] = 60;
    pixels[pixel++] = 60;
    pixels[pixel++] = 60;
    pixels[pixel++] = 255;

    pixels[pixel++] = 40;
    pixels[pixel++] = 40;
    pixels[pixel++] = 40;
    pixels[pixel++] = 255;

    pixels[pixel++] = 40;
    pixels[pixel++] = 40;
    pixels[pixel++] = 40;
    pixels[pixel++] = 255;

    pixels[pixel++] = 60;
    pixels[pixel++] = 60;
    pixels[pixel++] = 60;
    pixels[pixel++] = 255;

    p.visual_output = fan::opengl::GL_REPEAT;

    create_texture();
    bind_texture();

    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_S, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_WRAP_T, p.visual_output);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MIN_FILTER, p.min_filter);
    context.opengl.call(context.opengl.glTexParameteri, fan::opengl::GL_TEXTURE_2D, fan::opengl::GL_TEXTURE_MAG_FILTER, p.mag_filter);

    size = fan::vec2i(2, 2);

    context.opengl.call(context.opengl.glTexImage2D, fan::opengl::GL_TEXTURE_2D, 0, p.internal_format, 2, 2, 0, p.format, p.type, pixels);

    free(pixels);

    context.opengl.call(context.opengl.glGenerateMipmap, fan::opengl::GL_TEXTURE_2D);
  }

  fan::vec4_wrap_t<fan::vec2> calculate_aspect_ratio(const fan::vec2& size, f32_t scale) {

    fan::vec4_wrap_t<fan::vec2> tc = {
      fan::vec2(0, 1),
      fan::vec2(1, 1),
      fan::vec2(1, 0),
      fan::vec2(0, 0)
    };

    f32_t a = size.x / size.y;
    fan::vec2 n = size.normalize();

    for (uint32_t i = 0; i < 8; i++) {
      if (size.x < size.y) {
        tc[i % 4][i / 4] *= n[i / 4] / a * scale;
      }
      else {
        tc[i % 4][i / 4] *= n[i / 4] * a * scale;
      }
    }
    return tc;
  }

  void get_pixel_data(void* data, fan::opengl::GLenum format) {
    auto& context = gloco->get_context();

    bind_texture();

    context.opengl.call(
      context.opengl.glGetTexImage, 
      fan::opengl::GL_TEXTURE_2D,
      0,
      format,
      fan::opengl::GL_UNSIGNED_BYTE,
      data
    );
  }

  // slow
  std::unique_ptr<uint8_t[]> get_pixel_data(fan::opengl::GLenum format, fan::vec2 uvp = 0, fan::vec2 uvs = 1) {
    auto& context = gloco->get_context();

    bind_texture();

    fan::vec2ui uv_size = {
     (uint32_t)(size.x * uvs.x),
     (uint32_t)(size.y * uvs.y)
    };

    auto full_ptr = std::make_unique<uint8_t[]>(size.x * size.y * 4); // assuming rgba

    context.opengl.call(
      context.opengl.glGetTexImage,
      fan::opengl::GL_TEXTURE_2D,
      0,
      format,
      fan::opengl::GL_UNSIGNED_BYTE,
      full_ptr.get()
    );

    auto ptr = std::make_unique<uint8_t[]>(uv_size.x * uv_size.y * 4); // assuming rgba

    for (uint32_t y = 0; y < uv_size.y; ++y) {
      for (uint32_t x = 0; x < uv_size.x; ++x) {
        uint32_t full_index = ((y + uvp.y * size.y) * size.x + (x + uvp.x * size.x)) * 4;
        uint32_t index = (y * uv_size.x + x) * 4;
        ptr[index + 0] = full_ptr[full_index + 0];
        ptr[index + 1] = full_ptr[full_index + 1];
        ptr[index + 2] = full_ptr[full_index + 2];
        ptr[index + 3] = full_ptr[full_index + 3];
      }
    }

    return ptr;
  }


  loco_t::image_list_NodeReference_t texture_reference;
//public:
  fan::vec2 size;
};
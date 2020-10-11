﻿#pragma once
//#ifndef __INTELLISENSE__ 

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define REQUIRE_GRAPHICS
#include <FAN/global_vars.hpp>

//#define FAN_PERFORMANCE
#define RAM_SAVER
#if defined(_WIN64) || defined(_WIN32) && !defined(FAN_WINDOWS)
#define FAN_WINDOWS
#endif

#include <FAN/types.h>

#include <vector>
#include <array>
#include <map>

#include <FAN/input.hpp>
#include <FAN/math.hpp>
#include <FAN/shader.h>
#include <FAN/time.hpp>
#include <FAN/network.hpp>
#include <FAN/SOIL2/SOIL2.h>
#include <FAN/SOIL2/stb_image.h>

#ifdef _MSC_VER
#pragma warning (disable : 26495)
#endif

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#define COLORSIZE 4

static constexpr int block_size = 50;
//constexpr fan::vec2i grid_size(WINDOWSIZE.x / block_size, WINDOWSIZE.y / block_size);

#if SYSTEM_BIT == 32
constexpr auto GL_FLOAT_T = GL_FLOAT;
#else
// for now
constexpr auto GL_FLOAT_T = GL_FLOAT;
#endif


typedef std::vector<std::vector<std::vector<bool>>> map_t;

struct bmp {
	unsigned char* data;
	unsigned char* image;
};

struct Texture {
	Texture();

	unsigned int texture;
	int width, height;
	unsigned int VBO, VAO;
};

namespace BMP_Offsets {
	constexpr::ptrdiff_t PIXELDATA = 0xA;
	constexpr::ptrdiff_t WIDTH = 0x12;
	constexpr::ptrdiff_t HEIGHT = 0x16;
}

bmp LoadBMP(const char* path, Texture& texture);
uint64_t _2d_1d(fan::vec2 position = cursor_position);

fan::vec2i window_position();

class Camera {
public:
	Camera(
		fan::vec3 position = fan::vec3(0, 0, 0),
		fan::vec3 up = fan::vec3(0.0f, 1.0f, 0.0f),
		float yaw = 0,
		float pitch = 0.0f
	);

	void move(bool noclip, f32_t movement_speed);
	void rotate_camera(bool when);

	fan::mat4 get_view_matrix();
	fan::mat4 get_view_matrix(fan::mat4 m);

	fan::vec3 get_position() const;
	void set_position(const fan::vec3& position);

	fan::vec3 position;
	fan::vec3 front;

	float yaw;
	float pitch;
	fan::vec3 right;
	fan::vec3 up;
	fan::vec3 velocity;
	bool firstMouse = true;

	static constexpr auto friction = 12;

	void update_vectors();

private:

	fan::vec3 worldUp;
};

int load_texture(const std::string_view path, const std::string& directory = std::string(), bool flip_image = false, bool alpha = false);

void write_vbo(unsigned int buffer, void* data, std::uint64_t size);

template <typename _Vector>
class basic_shape_vector {
public:

	basic_shape_vector(const Shader& shader);
	basic_shape_vector(const Shader& shader, const _Vector& position, const _Vector& size);
	~basic_shape_vector();

	_Vector get_size(std::uint64_t i) const;
	void set_size(std::uint64_t i, const _Vector& size, bool queue = false);

	std::vector<_Vector> get_positions() const;
	void set_positions(const std::vector<_Vector>& positions);

	_Vector get_position(std::uint64_t i) const;
	void set_position(std::uint64_t i, const _Vector& position, bool queue = false);

	void basic_push_back(const _Vector& position, const _Vector& size, bool queue = false);

	void erase(std::uint64_t i);

	std::uint64_t size() const;

	bool empty() const;

	void write_data(bool position, bool size);

protected:

	void initialize_buffers();

	void basic_draw(unsigned int mode, std::uint64_t count, std::uint64_t primcount, std::uint64_t i = -1);


	unsigned int vao;
	unsigned int position_vbo;
	unsigned int size_vbo;

	std::vector<_Vector> m_position;
	std::vector<_Vector> m_size;

	Shader m_shader;

};

class basic_shape_color_vector {
public:

	basic_shape_color_vector();
	basic_shape_color_vector(const fan::color& color);
	~basic_shape_color_vector();

	fan::color get_color(std::uint64_t i);
	void set_color(std::uint64_t i, const fan::color& color, bool queue = false);

protected:

	void basic_push_back(const fan::color& color, bool queue = false);

	void write_data();

	void initialize_buffers(bool divisor = true);

	unsigned int color_vbo;

	std::vector<fan::color> m_color;

};

constexpr fan::da_t<f32_t, 4, 2> get_square_corners(const fan::da_t<f32_t, 2, 2>& squ) {
	return fan::da_t<f32_t, 4, 2>{
		fan::da_t<f32_t, 2>(squ[0]),
			fan::da_t<f32_t, 2>(squ[1][0], squ[0][1]),
			fan::da_t<f32_t, 2>(squ[0][0], squ[1][1]),
			fan::da_t<f32_t, 2>(squ[1])
	};
}

enum class shape_types {
	LINE,
	SQUARE,
	TRIANGLE
};

namespace fan_2d {

	extern fan::mat4 frame_projection;
	extern fan::mat4 frame_view;
	extern Camera camera;

	namespace shader_paths {
		constexpr auto text_renderer_vs("FAN/GLSL/2D/text.vs");
		constexpr auto text_renderer_fs("FAN/GLSL/2D/text.fs");

		constexpr auto single_shapes_path_vs("FAN/GLSL/2D/shapes.vs");
		constexpr auto single_shapes_path_fs("FAN/GLSL/2D/shapes.fs");
		constexpr auto single_bloom_shapes_path_vs("FAN/GLSL/2D/bloom_shape.vs");
		constexpr auto single_bloom_shapes_path_fs("FAN/GLSL/2D/bloom_shape.fs");

		constexpr auto single_sprite_path_vs("FAN/GLSL/2D/sprite.vs");
		constexpr auto single_sprite_path_fs("FAN/GLSL/2D/sprite.fs");

		constexpr auto shape_vector_vs("FAN/GLSL/2D/shape_vector.vs");
		constexpr auto shape_vector_fs("FAN/GLSL/2D/shapes.fs");
		constexpr auto sprite_vector_vs("FAN/GLSL/2D/sprite_vector.vs");
		constexpr auto sprite_vector_fs("FAN/GLSL/2D/sprite_vector.fs");
	}

	void move_object(fan::vec2& position, fan::vec2& velocity, f32_t speed, f32_t gravity, f32_t jump_force = -800, f32_t friction = 10);

	class basic_single_shape {
	public:

		basic_single_shape();
		basic_single_shape(const Shader& shader, const fan::vec2& position, const fan::vec2& size);

		~basic_single_shape();

		fan::vec2 get_position() const;
		fan::vec2 get_size() const;
		fan::vec2 get_velocity() const;

		void set_size(const fan::vec2& size);
		void set_position(const fan::vec2& position);
		void set_velocity(const fan::vec2& velocity);

		void basic_draw(GLenum mode, GLsizei count);

		void move(f32_t speed, f32_t gravity = 0, f32_t jump_force = -800, f32_t friction = 10);

		bool inside() const;

	protected:
		fan::vec2 position;
		fan::vec2 size;

		fan::vec2 velocity;

		Shader shader;

		unsigned int vao;
	};

	struct basic_single_color {

		basic_single_color();
		basic_single_color(const fan::color& color);

		fan::color get_color();
		void set_color(const fan::color& color);

		fan::color color;

	};

	struct line : public basic_single_shape, basic_single_color {

		line();
		line(const fan::mat2& begin_end, const fan::color& color);

		void draw();

		void set_position(const fan::mat2& begin_end);

	private:
		using basic_single_shape::set_position;
		using basic_single_shape::set_size;
	};

	struct square : public basic_single_shape, basic_single_color {
		square();
		square(const fan::vec2& position, const fan::vec2& size, const fan::color& color);

		fan::vec2 center() const;

		void draw();
	};

	class bloom_square : public basic_single_shape, public basic_single_color {
	public:

		bloom_square();
		bloom_square(const fan::vec2& position, const fan::vec2& size, const fan::color& color);

		void bind_fbo() const;

		void draw();

	private:

		unsigned int m_hdr_fbo;
		unsigned int m_rbo;
		unsigned int m_color_buffers[2];
		unsigned int m_pong_fbo[2];
		unsigned int m_pong_color_buffer[2];

		//Shader m_shader_light = Shader("GLSL/bloom.vs", "GLSL/light_box.fs");
		Shader m_shader_blur = Shader("GLSL/blur.vs", "GLSL/blur.fs");
		Shader m_shader_bloom = Shader("GLSL/bloom_final.vs", "GLSL/bloom_final.fs");
		
	};

	struct image_info {
		fan::vec2i image_size;
		unsigned int texture_id;
	};

	class sprite : public basic_single_shape {
	public:
		sprite();

		// scale with default is sprite size
		sprite(const std::string& path, const fan::vec2& position, const fan::vec2& size = 0);
		sprite(unsigned char* pixels, const fan::vec2& position, const fan::vec2i& size = 0);

		void reload_image(unsigned char* pixels, const fan::vec2i& size);
		void reload_image(const std::string& path, const fan::vec2i& size);

		void draw();

		f32_t get_rotation();
		void set_rotation(f32_t degrees);

		static image_info load_image(const std::string& path, bool flip_image = false);
		static image_info load_image(unsigned char* pixels, const fan::vec2i& size);

	private:

		f32_t m_rotation;

		unsigned int texture;
	};

	class animation : public basic_single_shape {
	public:

		animation(const fan::vec2& position, const fan::vec2& size);

		void add(const std::string& path);

		void draw(std::uint64_t texture);

	private:
		std::vector<unsigned int> m_textures;
	};

	class line_vector : public basic_shape_vector<fan::vec2>, public basic_shape_color_vector {
	public:
		line_vector();
		line_vector(const fan::mat2& begin_end, const fan::color& color);

		void push_back(const fan::mat2& begin_end, const fan::color& color, bool queue = false);

		void draw();

		void set_position(std::uint64_t i, const fan::mat2& begin_end, bool queue = false);

		void release_queue(bool position, bool color);

	private:
		using basic_shape_vector::set_position;
		using basic_shape_vector::set_size;
	};

	struct triangle_vector : public basic_shape_vector<fan::vec2>, public basic_shape_color_vector {

		triangle_vector();
		triangle_vector(const fan::mat3x2& corners, const fan::color& color);
		
		void set_position(std::uint64_t i, const fan::mat3x2& corners);
		void push_back(const fan::mat3x2& corners, const fan::color& color);

		void draw();

	private:
		std::vector<fan::vec2> m_lcorners;
		std::vector<fan::vec2> m_mcorners;
		std::vector<fan::vec2> m_rcorners;

		uint32_t l_vbo, m_vbo, r_vbo;

	};

	class square_vector : public basic_shape_vector<fan::vec2>, public basic_shape_color_vector {
	public:

		square_vector();
		square_vector(const fan::vec2& position, const fan::vec2& size, const fan::color& color);
		~square_vector();

		fan_2d::square construct(uint_t i);

		void release_queue(bool position, bool size, bool color);

		void push_back(const fan::vec2& position, const fan::vec2& size, const fan::color& color, bool queue = false);
		void erase(uint_t i);

		void draw(std::uint64_t i = -1);

		fan::vec2 center(uint_t i) const;

		std::vector<fan::mat2> get_icorners() const;

		void move(uint_t i, f32_t speed, f32_t gravity = 0, f32_t jump_force = -800, f32_t friction = 10);

	private:

		std::vector<fan::mat2> m_icorners;
		std::vector<fan::vec2> m_velocity;

	};

	class sprite_vector : public basic_shape_vector<fan::vec2> {
	public:

		sprite_vector();
		sprite_vector(const std::string& path, const fan::vec2& position, const fan::vec2& size = 0);
		~sprite_vector();

		void push_back(const fan::vec2& position, const fan::vec2& size = 0, bool queue = false);

		void draw();

		void release_queue(bool position, bool size);

	private:

		unsigned int texture;
		fan::vec2i original_image_size;

	};

	struct particle {
		fan::vec2 m_velocity;
		Timer m_timer; // milli
	};

	class particles : public fan_2d::square_vector {
	public:

		void add(
			const fan::vec2& position, 
			const fan::vec2& size, 
			const fan::vec2& velocity, 
			const fan::color& color, 
			std::uint64_t time
		);

		void update();

	private:

		std::vector<fan_2d::particle> m_particles;
	};
}

namespace fan_3d {

	namespace shader_paths {
		constexpr auto triangle_vector_vs("FAN/GLSL/3D/terrain_generator.vs");
		constexpr auto triangle_vector_fs("FAN/GLSL/3D/terrain_generator.fs");

		constexpr auto shape_vector_vs("FAN/GLSL/3D/shape_vector.vs");
		constexpr auto shape_vector_fs("FAN/GLSL/3D/shape_vector.fs");

		constexpr auto model_vs("FAN/GLSL/3D/models.vs");
		constexpr auto model_fs("FAN/GLSL/3D/models.fs");

		constexpr auto skybox_vs("FAN/GLSL/3D/skybox.vs");
		constexpr auto skybox_fs("FAN/GLSL/3D/skybox.fs");
		constexpr auto skybox_model_vs("FAN/GLSL/3D/skybox_model.vs");
		constexpr auto skybox_model_fs("FAN/GLSL/3D/skybox_model.fs");
	}

	extern Camera camera;
	extern fan::mat4 frame_projection;
	extern fan::mat4 frame_view;

	void add_camera_movement_callback();

	class line_vector : public basic_shape_vector<fan::vec3>, public basic_shape_color_vector {
	public:

		line_vector();
		line_vector(const fan::mat2x3& begin_end, const fan::color& color);

		void push_back(const fan::mat2x3& begin_end, const fan::color& color, bool queue = false);

		void draw();

		void set_position(std::uint64_t i, const fan::mat2x3 begin_end, bool queue = false);
		
		void release_queue(bool position, bool color);

	private:

		using basic_shape_vector::set_position;
		using basic_shape_vector::set_size;

	};

	using triangle_vertices_t = fan::vec3;

	class terrain_generator : public basic_shape_color_vector {
	public:

		terrain_generator(const std::string& path, const fan::vec2& map_size, uint32_t triangle_size, const fan::vec2& mesh_size);

		void insert(const std::vector<triangle_vertices_t>& vertices, const std::vector<fan::color>& color, bool queue = false);
		void push_back(const triangle_vertices_t& vertices, const fan::color& color, bool queue = false);

		triangle_vertices_t get_vertices(std::uint64_t i);

		void edit_data(std::uint64_t i, const triangle_vertices_t& vertices, const fan::color& color);

		void release_queue();

		void draw();

		void erase_all();

		uint_t size();

	private:

		Shader m_shader;

		uint32_t m_texture;
		uint32_t m_texture_vbo;
		uint32_t m_vao;
		uint32_t m_vertices_vbo;
		uint32_t m_ebo;
		uint32_t m_triangle_size;
		uint32_t m_normals_vbo;

		std::vector<triangle_vertices_t> m_triangle_vertices;
		std::vector<unsigned int> m_indices;
		static constexpr auto m_vertice_size = sizeof(triangle_vertices_t);

	};

	class square_vector : public basic_shape_vector<fan::vec3> {
	public:

		square_vector(const std::string& path, std::uint64_t block_size);

		void push_back(const fan::vec3& position, const fan::vec3& size, const fan::vec2& texture_id, bool queue = false);

		void draw();

		void set_texture(std::uint64_t i, const fan::vec2& texture_id, bool queue = false);

		void generate_textures(const std::string& path, const fan::vec2& block_size);

		void write_textures();

		void release_queue(bool position, bool size, bool textures);

	private:

		unsigned int m_texture;
		unsigned int m_texture_ssbo;
		unsigned int m_texture_id_ssbo;

		fan::vec2i block_size;

		std::vector<int> m_textures;

	};

	class skybox {
	public:
		skybox(
			const std::string& left,
			const std::string& right,
			const std::string& front,
			const std::string back,
			const std::string bottom,
			const std::string& top
		);

		~skybox();

		void draw();

	private:
		unsigned int texture_id;
		unsigned int skybox_vao, skybox_vbo;


		Shader shader;
		Camera* camera;
		static constexpr float skyboxVertices[108] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};
	};

	struct mesh_vertex {
		fan::vec3 position;
		fan::vec3 normal;
		fan::vec2 texture_coordinates;
	};

	struct mesh_texture {
		unsigned int id;
		std::string type;
		aiString path;
	};

	class model_mesh {
	public:
		std::vector<mesh_vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<mesh_texture> textures;
		unsigned int vao, vbo, ebo;

		model_mesh(
			const std::vector<mesh_vertex>& vertices,
			const std::vector<unsigned int>& indices,
			const std::vector<mesh_texture>& textures
		);

	private:

		void initialize_mesh();
	};

	class model_loader {
	protected:
		model_loader(const std::string& path, const fan::vec3& size);

		std::vector<model_mesh> meshes;
		std::vector<mesh_texture> textures_loaded;
	private:
		void load_model(const std::string& path, const fan::vec3& size);

		void process_node(aiNode* node, const aiScene* scene, const fan::vec3& size);

		model_mesh process_mesh(aiMesh* mesh, const aiScene* scene, const fan::vec3& size);

		std::vector<mesh_texture> load_material_textures(aiMaterial* mat, aiTextureType type, const std::string& type_name);

		std::string directory;
	};

	class model : public model_loader {
	public:
		model() :model_loader("", fan::vec3()), m_shader(fan_3d::shader_paths::model_vs, fan_3d::shader_paths::model_fs) {}
		model(const std::string& path, const fan::vec3& position, const fan::vec3& size);

		void draw();

		fan::vec3 get_position();
		void set_position(const fan::vec3& position);

		fan::vec3 get_size();
		void set_size(const fan::vec3& size);

	private:
		Shader m_shader;

		fan::vec3 m_position;
		fan::vec3 m_size;

	};

}

void GetFps(bool title = true, bool print = false);

enum class e_cube {
	left,
	right,
	front,
	back,
	down,
	up
};

extern std::vector<float> g_distances;

fan::vec3 intersection_point3d(const fan::vec3& plane_position, const fan::vec3& plane_size, const fan::vec3& position, e_cube side);

fan::vec3 line_plane_intersection3d(const fan::da_t<f32_t, 2, 3>& line, const fan::da_t<f32_t, 4, 3>& square);

#define maxPrimeIndex 10
inline int primeIndex = 0;

constexpr int numOctaves = 7;

constexpr int primes[maxPrimeIndex][3] = {
  { 995615039, 600173719, 701464987 },
  { 831731269, 162318869, 136250887 },
  { 174329291, 946737083, 245679977 },
  { 362489573, 795918041, 350777237 },
  { 457025711, 880830799, 909678923 },
  { 787070341, 177340217, 593320781 },
  { 405493717, 291031019, 391950901 },
  { 458904767, 676625681, 424452397 },
  { 531736441, 939683957, 810651871 },
  { 997169939, 842027887, 423882827 }
};

inline double persistence = 0.5;

constexpr double Noise(int i, int x, int y) {
	int n = x + y * 57;
	n = (n << 13) ^ n;
	int a = primes[i][0], b = primes[i][1], c = primes[i][2];
	int t = (n * (n * n * a + b) + c) & 0x7fffffff;
	return 1.0 - (double)(t) / 1073741824.0;
}

constexpr double SmoothedNoise(int i, int x, int y) {
	double corners = (Noise(i, x - 1, y - 1) + Noise(i, x + 1, y - 1) +
		Noise(i, x - 1, y + 1) + Noise(i, x + 1, y + 1)) / 16,
		sides = (Noise(i, x - 1, y) + Noise(i, x + 1, y) + Noise(i, x, y - 1) +
			Noise(i, x, y + 1)) / 8,
		center = Noise(i, x, y) / 4;
	return corners + sides + center;
}

inline double Interpolate(double a, double b, double x) {
	double ft = x * 3.1415927,
		f = (1 - cos(ft)) * 0.5;
	return  a * (1 - f) + b * f;
}

inline double InterpolatedNoise(int i, double x, double y) {
	int integer_X = x;
	double fractional_X = x - integer_X;
	int integer_Y = y;
	double fractional_Y = y - integer_Y;

	double v1 = SmoothedNoise(i, integer_X, integer_Y),
		v2 = SmoothedNoise(i, integer_X + 1, integer_Y),
		v3 = SmoothedNoise(i, integer_X, integer_Y + 1),
		v4 = SmoothedNoise(i, integer_X + 1, integer_Y + 1),
		i1 = Interpolate(v1, v2, fractional_X),
		i2 = Interpolate(v3, v4, fractional_X);
	return Interpolate(i1, i2, fractional_Y);
}


inline double ValueNoise_2D(double x, double y) {
	double total = 0,
		frequency = pow(2, numOctaves),
		amplitude = 1;
	for (int i = 0; i < numOctaves; ++i) {
		frequency /= 2;
		amplitude *= persistence;
		total += InterpolatedNoise((primeIndex + i) % maxPrimeIndex,
			x / frequency, y / frequency) * amplitude;
	}
	return total / frequency;
}

struct hash_vector_operators {
	size_t operator()(const fan::vec3& k) const {
		return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
	}

	bool operator()(const fan::vec3& a, const fan::vec3& b) const {
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}
};

constexpr int world_size = 150;

#define pass_array(d_map, d_position) \
		d_map[d_position.x] \
			 [d_position.y] \
			 [d_position.z]

template <typename T>
constexpr auto grid_direction(const T& src, const T& dst) {
	T vector(src - dst);
	return vector / vector.abs().max();
}

template <template <typename> typename T>
struct grid_raycast_s {
	T<f32_t> direction, begin;
	T<int> grid;
};

template <template <typename> typename T>
constexpr bool grid_raycast_single(grid_raycast_s<T>& caster, f32_t grid_size) {
	T position(caster.begin % grid_size);
	for (uint8_t i = 0; i < T<f32_t>::size(); i++) {
		position[i] = ((caster.direction[i] < 0) ? position[i] : grid_size - position[i]);
		position[i] = std::abs((!position[i] ? grid_size : position[i]) / caster.direction[i]);
	}
	caster.grid = (caster.begin += caster.direction * position.min()) / grid_size;
	for (uint8_t i = 0; i < T<f32_t>::size(); i++)
		caster.grid[i] -= ((caster.direction[i] < 0) & (position[i] == position.min()));
	return 1;
}

template <template <typename> typename T>
constexpr T<int> grid_raycast(const T<f32_t>& start, const T<f32_t>& end, const map_t& map, f32_t block_size) {
	if (start == end) {
		return start;
	}
	grid_raycast_s raycast = { grid_direction(end, start), start, T<int>() };
	T distance = end - start;
	auto max = distance.abs().max();
	for (int i = 0; i < max; i++) {
		grid_raycast_single(raycast, block_size);
		if (raycast.grid[0] < 0 || raycast.grid[1] < 0 || raycast.grid[2] < 0 ||
			raycast.grid[0] >= world_size || raycast.grid[1] >= world_size || raycast.grid[2] >= world_size) {
			continue;
		}
		if (map[raycast.grid[0]][raycast.grid[1]][raycast.grid[2]]) {
			return raycast.grid;
		}
	}
	return T(fan::RAY_DID_NOT_HIT);
}

#define d_grid_raycast(start, end, raycast, block_size) \
	grid_raycast_s raycast = { grid_direction(end, start), start, fan::vec3() }; \
	if (!(start == end)) \
		while(grid_raycast_single(raycast, block_size))

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
	std::vector<uint8_t> data;
	uint_t datasize;

	uint_t fontsize;
	fan::vec2i offset;
}suckless_font_t;

typedef struct {
	fan::vec2i pos;
	uint_t width;
	uint_t height;
}letter_info_t;

typedef struct {
	fan::vec2 pos;
	f32_t width;
}letter_info_opengl_t;

static letter_info_opengl_t letter_to_opengl(const suckless_font_t& font, const letter_info_t& letter) 
{
	letter_info_opengl_t ret;
	ret.pos = (fan::vec2)letter.pos / (fan::vec2)font.datasize;
	ret.width = (f32_t)letter.width / font.datasize;
	return ret;
}

inline void emplace_vertex_data(std::vector<fan::vec2>& vector, const fan::vec2& position, const fan::vec2& size) 
{
	vector.emplace_back(fan::vec2(position.x, position.y));
	vector.emplace_back(fan::vec2(position.x, position.y + size.y));
	vector.emplace_back(fan::vec2(position.x + size.x, position.y + size.y));
	vector.emplace_back(fan::vec2(position.x, position.y));
	vector.emplace_back(fan::vec2(position.x + size.x, position.y + size.y));
	vector.emplace_back(fan::vec2(position.x + size.x, position.y));
}

inline void edit_vertex_data(std::uint64_t offset, std::vector<fan::vec2>& vector, const fan::vec2& position, const fan::vec2& size) 
{
	vector[offset] =     fan::vec2(position.x, position.y);
	vector[offset + 1] = fan::vec2(position.x, position.y + size.y);
	vector[offset + 2] = fan::vec2(position.x + size.x, position.y + size.y);
	vector[offset + 3] = fan::vec2(position.x, position.y);
	vector[offset + 4] = fan::vec2(position.x + size.x, position.y + size.y);
	vector[offset + 5] = fan::vec2(position.x + size.x, position.y);
}

inline void erase_vertex_data(std::uint64_t offset, std::vector<fan::vec2>& vector, std::uint64_t size) {
	vector.erase(vector.begin() + offset * (size * 6), vector.begin() + (offset * ((size * 6))) + size * 6);
}

template <typename T>
constexpr auto vector_2d_to_1d(const std::vector<std::vector<T>>& vector) {
	std::vector<T> new_vector(vector.size());
	for (auto i : vector) {
		new_vector.insert(new_vector.end(), i.begin(), i.end());
	}
	return new_vector;
}

constexpr uint_t max_ascii = 248;
constexpr uint_t max_font_size = 1024;

namespace fan_gui {

	constexpr fan::color default_text_color(1);
	constexpr f32_t font_size(128);

	class text_renderer {
	public:
		text_renderer();

		~text_renderer();

		void render(const std::wstring& text, fan::vec2 position, const fan::color& color, f32_t scale, bool use_old = false);

	protected:

		void alloc_storage(const std::vector<std::wstring>& vector);
		void realloc_storage(const std::vector<std::wstring>& vector);

		void store_to_renderer(std::wstring& text, fan::vec2 position, const fan::color& color, f32_t scale, f32_t max_width = -1);
		void edit_storage(uint64_t i, const std::wstring& text, fan::vec2 position, const fan::color& color, f32_t scale);

		void upload_vertices();
		void upload_colors();
		void upload_characters();

		void upload_stored();
		void upload_stored(uint64_t i);

		void render_stored();
		void set_scale(uint64_t i, f32_t font_size, fan::vec2 position);

		fan::vec2 get_length(const std::wstring& text, f32_t scale);
		std::vector<fan::vec2> get_length(const std::vector<std::wstring>& texts, const std::vector<f32_t>& scales, bool half = false);

		void clear_storage();

		std::vector<std::vector<int>> m_characters;
		std::vector<std::vector<fan::color>> m_colors;
		std::vector<std::vector<fan::vec2>> m_vertices;
		std::vector<f32_t> m_scales;

		static std::array<f32_t, 248> widths;

		static suckless_font_t font;

		Shader m_shader;
		unsigned int m_vao, m_vertex_ssbo;
		unsigned int m_texture;
		unsigned int m_text_ssbo;
		unsigned int m_texture_id_ssbo;
		unsigned int m_colors_ssbo;

	};

	namespace font {

		namespace properties {
			constexpr fan::vec2 gap_scale(0.25, 0.25);
			constexpr f32_t space_width = 15;
			constexpr f32_t space_between_characters = 5;

			constexpr fan::vec2 get_gap_scale(const fan::vec2& size) {
				return size * gap_scale;
			}

			constexpr f32_t get_gap_scale_x(f32_t width) {
				return width * gap_scale.x;
			}

			constexpr f32_t get_gap_scale_y(f32_t height) {
				return height * gap_scale.y;
			}

			constexpr f32_t get_character_x_offset(f32_t width, f32_t scale) {
				return width * scale + space_between_characters;
			}

			constexpr f32_t get_space(f32_t scale) {
				return scale / (font_size / 2) * space_width;
			}
		}

		namespace basic_methods {
			class basic_text_button_vector : public text_renderer {
			public:

				basic_text_button_vector();

			protected:
				fan::vec2 edit_size(uint64_t i, const std::wstring& text, f32_t scale);

				std::vector<std::wstring> m_texts;
			};
		}


		class text_button_vector : public fan_gui::font::basic_methods::basic_text_button_vector, public fan_2d::square_vector {
		public:

			text_button_vector();

			text_button_vector(const std::wstring& text, const fan::vec2& position, const fan::color& box_color, f32_t font_scale, f32_t left_offset, f32_t max_width);

			text_button_vector(const std::wstring& text, const fan::vec2& position, const fan::color& color, f32_t scale);
			text_button_vector(const std::wstring& text, const fan::vec2& position, const fan::color& color, f32_t scale, const fan::vec2& box_size);

			void add(const std::wstring& text, const fan::vec2& position, const fan::color& color, f32_t scale);
			void add(const std::wstring& text, const fan::vec2& position, const fan::color& color, f32_t scale, const fan::vec2& box_size);

			void edit_string(uint64_t i, const std::wstring& text, f32_t scale);

			fan::vec2 get_string_length(const std::wstring& text, f32_t scale);

			f32_t get_scale(uint64_t i);

			void set_font_size(uint64_t i, f32_t scale);
			void set_position(uint64_t i, const fan::vec2& position);

			void set_press_callback(int key, const std::function<void()>& function);

			void draw();

			bool inside(std::uint64_t i);

		private:

			using fan_2d::square_vector::set_position;
			using fan_2d::square_vector::draw;
			using fan_2d::square_vector::set_size;
		};
	}
}

void begin_render(const fan::color& background_color);
void end_render();

#define window_loop2(color_m, code_m) \
	while(!glfwWindowShouldClose(window)) { \
		begin_render(color_m); \
		code_m \
		end_render(); \
	}

static void window_loop(const fan::color& color, std::function<void()> _function) {
	while (!glfwWindowShouldClose(window)) {
		begin_render(color);
		_function();
		end_render();
	}
}


#define sign_dr(_m) (si_t)(-(_m < 0) | (_m > 0))

constexpr fan::da_t<f32_t, 2> LineInterLine_fr(fan::da_t<f32_t, 2, 2> src, fan::da_t<f32_t, 2, 2> dst, const fan::da_t<f32_t, 2>& normal) {
	f32_t s1_x, s1_y, s2_x, s2_y;
	s1_x = src[1][0] - src[0][0]; s1_y = src[1][1] - src[0][1];
	s2_x = dst[1][0] - dst[0][0]; s2_y = dst[1][1] - dst[0][1];

	const f32_t s = (-s1_y * (src[0][0] - dst[0][0]) + s1_x * (src[0][1] - dst[0][1])) / (-s2_x * s1_y + s1_x * s2_y);
	const f32_t t = (s2_x * (src[0][1] - dst[0][1]) - s2_y * (src[0][0] - dst[0][0])) / (-s2_x * s1_y + s1_x * s2_y);

	if (s < 0 || s > 1 || t < 0 || t > 1)
		return FLT_MAX;

	si_t signy = sign_dr(normal.gfne());
	if (fan::dcom_fr(signy > 0, src[1][!!normal[1]], dst[0][!!normal[1]]))
		return FLT_MAX;

	fan::da_t<f32_t, 2> min = dst.min();
	fan::da_t<f32_t, 2> max = dst.max();
	for (ui_t i = 0; i < 2; i++) {
		if (!normal[i])
			continue;
		if (src[0][i ^ 1] == min[i ^ 1])
			return FLT_MAX;
		if (src[0][i ^ 1] == max[i ^ 1])
			return FLT_MAX;
	}

	return { src[0][0] + (t * s1_x), src[0][1] + (t * s1_y) };
}
constexpr fan::da_t<ui_t, 3> GetPointsTowardsVelocity3(fan::da_t<f32_t, 2> vel) {
	if (vel[0] >= 0)
		if (vel[1] >= 0)
			return { 2, 1, 3 };
		else
			return { 0, 3, 1 };
	else
		if (vel[1] >= 0)
			return { 0, 3, 2 };
		else
			return { 2, 1, 0 };
}

constexpr void calculate_velocity(const fan::da_t<f32_t, 2>& spos, const fan::da_t<f32_t, 2>& svel, const fan::da_t<f32_t, 2>& dpos, const fan::da_t<f32_t, 2>& dvel, const fan::da_t<f32_t, 2>& normal, f32_t sign, fan::da_t<f32_t, 2>& lvel, fan::da_t<f32_t, 2>& nvel) {
	fan::da_t<f32_t, 2, 2> sline = { spos, spos + svel };
	fan::da_t<f32_t, 2, 2> dline = { dpos, dpos + dvel };
	fan::da_t<f32_t, 2> inter = LineInterLine_fr(sline, dline, normal);
	if (inter == FLT_MAX)
		return;
	fan::da_t<f32_t, 2> tvel = (inter - spos) * sign;
	if (tvel.abs() >= lvel.abs())
		return;
	nvel = svel * sign - tvel;
	lvel = tvel;
	nvel[0] = normal[1] ? nvel[0] : 0;
	nvel[1] = normal[0] ? nvel[1] : 0;
}

constexpr fan::da_t<f32_t, 4, 2> Math_SquToQuad_fr(const fan::da_t<f32_t, 2, 2>& squ) {
	return fan::da_t<f32_t, 4, 2>{
		fan::da_t<f32_t, 2>(squ[0]),
			fan::da_t<f32_t, 2>(squ[1][0], squ[0][1]),
			fan::da_t<f32_t, 2>(squ[0][0], squ[1][1]),
			fan::da_t<f32_t, 2>(squ[1])
	};
}

constexpr auto get_cross(const fan::da_t<f32_t, 2>& a, const fan::da_t<f32_t, 3>& b) {
	return cross(fan::da_t<f32_t, 3>{ a[0], a[1], 0 }, b);
}

template <
	template <typename, std::size_t, std::size_t> typename inner_da_t,
	template <typename, std::size_t> typename outer_da_t, std::size_t n
>
constexpr fan::da_t<fan::da_t<f32_t, 2>, n> get_normals(const outer_da_t<inner_da_t<f32_t, 2, 2>, n>& lines) {
	fan::da_t<fan::da_t<f32_t, 2>, n> normals;
	for (int i = 0; i < n; i++) {
		normals[i] = get_cross(lines[i][1] - lines[i][0], fan::da_t<f32_t, 3>(0, 0, 1));
	}
	return normals;
}


inline uint8_t ProcessCollision_fl(fan::da_t<f32_t, 2, 2>& pos, fan::da_t<f32_t, 2>& vel, const std::vector<fan::da_t<f32_t, 2, 2>> walls) {
	fan::da_t<f32_t, 2> pvel = vel;

	if (!pvel[0] && !pvel[1])
		return 0;

	fan::da_t<f32_t, 4, 2> ocorn = Math_SquToQuad_fr(pos);
	fan::da_t<f32_t, 4, 2> ncorn = ocorn + pvel;

	fan::da_t<ui_t, 3> ptv3 = GetPointsTowardsVelocity3(pvel);
	fan::da_t<ui_t, 3> ntv3 = GetPointsTowardsVelocity3(-pvel);

	fan::da_t<ui_t, 4, 2> li = { fan::da_t<ui_t, 2>{0, 1}, fan::da_t<ui_t, 2>{1, 3}, fan::da_t<ui_t, 2>{3, 2}, fan::da_t<ui_t, 2>{2, 0} };

	const static auto normals = get_normals(
		fan::da_t<fan::da_t<f32_t, 2, 2>, 4>{
		fan::da_t<f32_t, 2, 2>{fan::da_t<f32_t, 2>{ 0, 0 }, fan::da_t<f32_t, 2>{ 1, 0 }},
			fan::da_t<f32_t, 2, 2>{fan::da_t<f32_t, 2>{ 1, 0 }, fan::da_t<f32_t, 2>{ 1, 1 }},
			fan::da_t<f32_t, 2, 2>{fan::da_t<f32_t, 2>{ 1, 1 }, fan::da_t<f32_t, 2>{ 0, 1 }},
			fan::da_t<f32_t, 2, 2>{fan::da_t<f32_t, 2>{ 0, 1 }, fan::da_t<f32_t, 2>{ 0, 0 }},
	});

	fan::da_t<f32_t, 2> lvel = pvel;
	fan::da_t<f32_t, 2> nvel = 0;
	for (ui_t iwall = 0; iwall < walls.size(); iwall++) {
		fan::da_t<f32_t, 4, 2> bcorn = Math_SquToQuad_fr(walls[iwall]);

		/* step -1 */
		for (ui_t i = 0; i < 4; i++) {
			for (ui_t iline = 0; iline < 4; iline++) {
				calculate_velocity(fan::da_t<f32_t, 2, 2>(ocorn[li[i][0]], ocorn[li[i][1]]).avg(), pvel, bcorn[li[iline][0]], bcorn[li[iline][1]] - bcorn[li[iline][0]], normals[iline], 1, lvel, nvel);
			}
		}

		/* step 0 and step 1*/
		for (ui_t i = 0; i < 3; i++) {
			for (ui_t iline = 0; iline < 4; iline++) {
				calculate_velocity(ocorn[ptv3[i]], ncorn[ptv3[i]] - ocorn[ptv3[i]], bcorn[li[iline][0]], bcorn[li[iline][1]] - bcorn[li[iline][0]], normals[iline], 1, lvel, nvel);
				calculate_velocity(bcorn[ntv3[i]], -pvel, ocorn[li[iline][0]], ocorn[li[iline][1]] - ocorn[li[iline][0]], normals[iline], -1, lvel, nvel);
			}
		}
	}

	pos += lvel;
	vel = nvel;

	return 1;
}

#define ProcessCollision_dl(pos_m, vel_m, walls_m) \
	while(ProcessCollision_fl(pos_m, vel_m, walls_m))

inline void rectangle_collision_2d(fan_2d::square& player, const fan_2d::square_vector& walls) {
	const fan::da_t<f32_t, 2> size = player.get_size();
	const fan::da_t<f32_t, 2> base = player.get_velocity();
	fan::da_t<f32_t, 2> velocity = base * delta_time;
	const fan::da_t<f32_t, 2> old_position = player.get_position() - velocity;
	fan::da_t<f32_t, 2, 2> my_corners(old_position, old_position + size);
	const auto wall_corners = walls.get_icorners();
	ProcessCollision_dl(my_corners, velocity, wall_corners);
	player.set_position(my_corners[0]);
}

constexpr bool rectangles_collide(const fan::vec2& a, const  fan::vec2& a_size, const fan::vec2& b, const fan::vec2& b_size) {
	bool x = a[0] + a_size[0] > b[0] &&
		a[0] < b[0] + b_size[0];
	bool y = a[1] + a_size[1] > b[1] &&
		a[1] < b[1] + b_size[1];
	return x && y;
}

static void vsync() {
	glfwSwapInterval(1);
}

//#endif
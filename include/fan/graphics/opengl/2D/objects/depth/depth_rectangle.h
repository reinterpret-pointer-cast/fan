#pragma once

#include _FAN_PATH(graphics/opengl/gl_core.h)
#include _FAN_PATH(graphics/opengl/gl_shader.h)
#include _FAN_PATH(graphics/shared_graphics.h)
#include _FAN_PATH(physics/collision/rectangle.h)

namespace fan_2d {
	namespace opengl {
		namespace depth {
			struct rectangle_t {

			rectangle_t() = default;

			struct properties_t {
				fan::color color;
				fan::vec2 position = 0;
				fan::vec2 size = 0;
				f32_t angle = 0;
				fan::vec2 rotation_point = 0;
				fan::vec3 rotation_vector = fan::vec3(0, 0, 1);
				f32_t render_depth = 0;
			};

			static constexpr uint32_t vertex_count = 6;

			static constexpr uint32_t offset_color = offsetof(properties_t, color);
			static constexpr uint32_t offset_position = offsetof(properties_t, position);
			static constexpr uint32_t offset_size = offsetof(properties_t, size);
			static constexpr uint32_t offset_angle = offsetof(properties_t, angle);
			static constexpr uint32_t offset_rotation_point = offsetof(properties_t, rotation_point);
			static constexpr uint32_t offset_rotation_vector = offsetof(properties_t, rotation_vector);
			static constexpr uint32_t offset_render_depth = offsetof(properties_t, render_depth);
			static constexpr uint32_t element_byte_size = offset_render_depth + sizeof(properties_t::render_depth);

			void open(fan::opengl::context_t* context) {

				m_shader.open(context);

				m_shader.set_vertex(
					context, 
					#include _FAN_PATH(graphics/glsl/opengl/2D/objects/depth/depth_rectangle.vs)
				);

				m_shader.set_fragment(
					context, 
					#include _FAN_PATH(graphics/glsl/opengl/2D/objects/depth/depth_rectangle.fs)
				);

				m_shader.compile(context);

				m_glsl_buffer.open(context);
				m_glsl_buffer.init(context, m_shader.id, element_byte_size);
				m_queue_helper.open();
				m_draw_node_reference = fan::uninitialized;
			}
			void close(fan::opengl::context_t* context) {

				m_glsl_buffer.close(context);
				m_queue_helper.close(context);
				m_shader.close(context);

				if (m_draw_node_reference == fan::uninitialized) {
					return;
				}

				context->disable_draw(m_draw_node_reference);
				m_draw_node_reference = fan::uninitialized;
			}

			void push_back(fan::opengl::context_t* context, rectangle_t::properties_t properties) {
				properties.render_depth += size(context) * 0.00000003;
				for (int i = 0; i < vertex_count; i++) {
					m_glsl_buffer.push_ram_instance(context, &properties, sizeof(properties));
				}
				m_queue_helper.edit(
					context,
					(this->size(context) - 1) * vertex_count * element_byte_size,
					(this->size(context)) * vertex_count * element_byte_size,
					&m_glsl_buffer
				);
			}

			void insert(fan::opengl::context_t* context, uint32_t i, rectangle_t::properties_t properties) {
				for (int j = 0; j < vertex_count; j++) {
					m_glsl_buffer.insert_ram_instance(context, i * vertex_count + j, &properties, sizeof(properties));
				}
				m_queue_helper.edit(
					context,
					i * vertex_count * element_byte_size,
					(this->size(context)) * vertex_count * element_byte_size,
					&m_glsl_buffer
				);
			}


			void erase(fan::opengl::context_t* context, uint32_t i) {
				m_glsl_buffer.erase_instance(context, i * vertex_count, 1, element_byte_size, vertex_count);

				m_queue_helper.edit(
					context,
					i * vertex_count * element_byte_size,
					m_glsl_buffer.m_buffer.size(),
					&m_glsl_buffer
				);
			}

			void erase(fan::opengl::context_t* context, uint32_t begin, uint32_t end) {

				m_glsl_buffer.erase_instance(context, begin * vertex_count, end - begin, element_byte_size, vertex_count);

				uint32_t to = m_glsl_buffer.m_buffer.size();

				if (to == 0) {
					// erase queue if there will be no objects left (special case)
					return;
				}

				m_queue_helper.edit(
					context,
					begin * vertex_count * element_byte_size,
					to,
					&m_glsl_buffer
				);
			}

			// erases everything
			void clear(fan::opengl::context_t* context) {
				m_glsl_buffer.clear_ram(context);
			}

			fan_2d::opengl::rectangle_corners_t get_corners(fan::opengl::context_t* context, uint32_t i) const {
				auto position = this->get_position(context, i);
				auto size = this->get_size(context, i);

				fan::vec2 mid = position;

				auto corners = fan_2d::opengl::get_rectangle_corners_no_rotation(position, size);

				f32_t angle = -this->get_angle(context, i);

				fan::vec2 top_left = fan_2d::opengl::get_transformed_point(corners[0] - mid, angle) + mid;
				fan::vec2 top_right = fan_2d::opengl::get_transformed_point(corners[1] - mid, angle) + mid;
				fan::vec2 bottom_left = fan_2d::opengl::get_transformed_point(corners[2] - mid, angle) + mid;
				fan::vec2 bottom_right = fan_2d::opengl::get_transformed_point(corners[3] - mid, angle) + mid;

				return { top_left, top_right, bottom_left, bottom_right };
			}

			const fan::color get_color(fan::opengl::context_t* context, uint32_t i) const {
				return *(fan::color*)m_glsl_buffer.get_instance(context, i * vertex_count, element_byte_size, offset_color);
			}
			void set_color(fan::opengl::context_t* context, uint32_t i, const fan::color& color) {
				for (int j = 0; j < vertex_count; j++) {
					m_glsl_buffer.edit_ram_instance(
						context, 
						i * vertex_count + j,
						&color,
						element_byte_size,
						offset_color,
						sizeof(rectangle_t::properties_t::color)
					);
				}

				m_queue_helper.edit(
					context,
					i * vertex_count * element_byte_size,
					(i + 1) * (vertex_count)*element_byte_size,
					&m_glsl_buffer
				);
			}

			fan::vec2 get_position(fan::opengl::context_t* context, uint32_t i) const {
				return *(fan::vec2*)m_glsl_buffer.get_instance(context, i * vertex_count, element_byte_size, offset_position);
			}
			void set_position(fan::opengl::context_t* context, uint32_t i, const fan::vec2& position) {
				for (int j = 0; j < vertex_count; j++) {
					m_glsl_buffer.edit_ram_instance(
						context, 
						i * vertex_count + j,
						&position,
						element_byte_size,
						offset_position,
						sizeof(rectangle_t::properties_t::position)
					);
				}
				m_queue_helper.edit(
					context,
					i * vertex_count * element_byte_size,
					(i + 1) * (vertex_count)*element_byte_size,
					&m_glsl_buffer
				);
			}

			fan::vec2 get_size(fan::opengl::context_t* context, uint32_t i) const {
				return *(fan::vec2*)m_glsl_buffer.get_instance(context, i * vertex_count, element_byte_size, offset_size);
			}
			void set_size(fan::opengl::context_t* context, uint32_t i, const fan::vec2& size) {
				for (int j = 0; j < vertex_count; j++) {
					m_glsl_buffer.edit_ram_instance(
						context, 
						i * vertex_count + j,
						&size,
						element_byte_size,
						offset_size,
						sizeof(rectangle_t::properties_t::size)
					);
				}
				m_queue_helper.edit(
					context,
					i * vertex_count * element_byte_size,
					(i + 1) * (vertex_count)*element_byte_size,
					&m_glsl_buffer
				);
			}

			f32_t get_angle(fan::opengl::context_t* context, uint32_t i) const {
				return *(f32_t*)m_glsl_buffer.get_instance(context, i * vertex_count, element_byte_size, offset_angle);
			}
			void set_angle(fan::opengl::context_t* context, uint32_t i, f32_t angle) {
				f32_t a = fmod(angle, fan::math::pi * 2);

				for (int j = 0; j < vertex_count; j++) {
					m_glsl_buffer.edit_ram_instance(
						context, 
						i * vertex_count + j,
						&a,
						element_byte_size,
						offset_angle,
						sizeof(rectangle_t::properties_t::angle)
					);
				}
				m_queue_helper.edit(
					context,
					i * vertex_count * element_byte_size,
					(i + 1) * (vertex_count)*element_byte_size,
					&m_glsl_buffer
				);
			}

			fan::vec2 get_rotation_point(fan::opengl::context_t* context, uint32_t i) const {
				return *(fan::vec2*)m_glsl_buffer.get_instance(context, i * vertex_count, element_byte_size, offset_rotation_point);
			}
			void set_rotation_point(fan::opengl::context_t* context, uint32_t i, const fan::vec2& rotation_point) {
				for (int j = 0; j < vertex_count; j++) {
					m_glsl_buffer.edit_ram_instance(
						context, 
						i * vertex_count + j,
						&rotation_point,
						element_byte_size,
						offset_rotation_point,
						sizeof(rectangle_t::properties_t::rotation_point)
					);
				}
				m_queue_helper.edit(
					context,
					i * vertex_count * element_byte_size,
					(i + 1) * (vertex_count)*element_byte_size,
					&m_glsl_buffer
				);
			}

			fan::vec3 get_rotation_vector(fan::opengl::context_t* context, uint32_t i) const {
				return *(fan::vec3*)m_glsl_buffer.get_instance(context, i * vertex_count, element_byte_size, offset_rotation_vector);
			}
			void set_rotation_vector(fan::opengl::context_t* context, uint32_t i, const fan::vec3& rotation_vector) {
				for (int j = 0; j < vertex_count; j++) {
					m_glsl_buffer.edit_ram_instance(
						context,
						i * vertex_count + j,
						&rotation_vector,
						element_byte_size,
						offset_rotation_vector,
						sizeof(rectangle_t::properties_t::rotation_vector)
					);
				}
				m_queue_helper.edit(
					context,
					i * vertex_count * element_byte_size,
					(i + 1) * (vertex_count)*element_byte_size,
					&m_glsl_buffer
				);
			}

			uint32_t size(fan::opengl::context_t* context) const {
				return m_glsl_buffer.m_buffer.size() / element_byte_size / vertex_count;
			}


			bool inside(fan::opengl::context_t* context, uint32_t i, const fan::vec2& position) const {

				auto corners = get_corners(context, i);

				return fan_2d::collision::rectangle::point_inside(
					corners[0],
					corners[1],
					corners[2],
					corners[3],
					position
				);
			}

			void enable_draw(fan::opengl::context_t* context) {
				m_draw_node_reference = context->enable_draw(this, [](fan::opengl::context_t* c, void* d) { ((decltype(this))d)->draw(c); });
			}
			void disable_draw(fan::opengl::context_t* context) {
			#ifdef fan_debug >= fan_debug_low
				if (m_draw_node_reference == fan::uninitialized) {
					fan::throw_error("trying to disable unenabled draw call");
				}
			#endif
				context->disable_draw(m_draw_node_reference);
			}

			// pushed to window draw queue
			void draw(fan::opengl::context_t* context) {

				m_shader.use(context);

				m_glsl_buffer.draw(
					context,
					0,
					this->size(context) * vertex_count
				);
			}

		private:

			fan::shader_t m_shader;
			fan::opengl::core::glsl_buffer_t m_glsl_buffer;
			fan::opengl::core::queue_helper_t m_queue_helper;
			uint32_t m_draw_node_reference;
		};
		}
	}
}
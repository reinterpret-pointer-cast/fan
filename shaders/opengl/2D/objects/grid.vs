#version 330

out vec2 instance_position;
out vec2 instance_size;
out vec4 instance_color;
out vec2 texture_coordinate;
out vec2 grid_size;

uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_size;
layout (location = 2) in vec2 in_grid_size;
layout (location = 3) in vec2 in_rotation_point;
layout (location = 4) in vec4 in_color;
layout (location = 5) in vec3 in_angle;

vec2 rectangle_vertices[] = vec2[](
	vec2(-1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(1.0, 1.0),

	vec2(1.0, 1.0),
	vec2(-1.0, 1.0),
	vec2(-1.0, -1.0)
);

vec2 tc[] = vec2[](
	vec2(0, 0), // top left
	vec2(1, 0), // top right
	vec2(1, 1), // bottom right
	vec2(1, 1), // bottom right
	vec2(0, 1), // bottom left
	vec2(0, 0) // top left
);

mat4 translate(mat4 m, vec3 v) {
	mat4 matrix = m;

	matrix[3][0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0];
	matrix[3][1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1];
	matrix[3][2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2];
	matrix[3][3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3];

	return matrix;
}

mat4 rotate(mat4 m, vec3 angles) {
	float cx = cos(angles.x);
	float sx = sin(angles.x);
	float cy = cos(angles.y);
	float sy = sin(angles.y);
	float cz = cos(angles.z);
	float sz = sin(angles.z);

	mat4 rotationX = mat4(1.0, 0.0, 0.0, 0.0,
						  0.0, cx, -sx, 0.0,
						  0.0, sx, cx, 0.0,
						  0.0, 0.0, 0.0, 1.0);

	mat4 rotationY = mat4(cy, 0.0, sy, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  -sy, 0.0, cy, 0.0,
						  0.0, 0.0, 0.0, 1.0);

	mat4 rotationZ = mat4(cz, -sz, 0.0, 0.0,
						  sz, cz, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0);

	mat4 matrix = rotationX * rotationY * rotationZ * m;
	return matrix;
}

void main() {
	uint id = uint(gl_VertexID % 6);

	vec2 ratio_size = in_size;

	vec2 rp = rectangle_vertices[id];

	mat4 m = mat4(1);
	m = translate(m, -vec3(in_rotation_point, 0));
	m = rotate(m, in_angle); 
	m = translate(m, vec3(in_rotation_point, 0));
	vec2 rotated = vec4(m * vec4(rp * in_size, 0, 1)).xy;

	gl_Position = projection * view * vec4((rotated + in_position.xy), in_position.z, 1);

	instance_position = in_position.xy;
	instance_size = in_size.xy;

	instance_color = in_color;
	texture_coordinate = tc[id];
	grid_size = in_grid_size / 2;
}

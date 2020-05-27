#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec4 inColor;
layout (location = 1) in vec2 in_texture_coordinate;

out vec2 texture_coordinate;
out vec4 color;

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 position = projection * view * vec4(aPos, 1.0);
   // color = inColor;
   texture_coordinate = in_texture_coordinate;
    gl_Position = position;
}
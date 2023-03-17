R"(
#version 330

layout (location = 0) out vec4 o_attachment0;

in vec4 instance_color;
in vec3 instance_position;
in float instance_radius;
in vec3 frag_position;
in vec2 texture_coordinate;

out vec4 color;

void main() {
  o_attachment0 = instance_color;

  vec3 lightDir = normalize(instance_position - frag_position);
  float distance = length(frag_position - instance_position);
  float radius = instance_radius;
  float smooth_edge = 0.01;
  float intensity = 1.0 - smoothstep(radius - smooth_edge, radius, distance);
  o_attachment0 *= intensity;
}
)"
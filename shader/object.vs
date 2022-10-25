#version 450
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 model;
uniform mat4 camera;

out VS_OUT {
   vec3 normal;
   vec3 pos;
   vec3 color;
} vs_out;

uniform float point_size;
uniform vec3 color;

void main() {
   vs_out.normal = normalize(mat3(transpose(inverse(model))) * aNormal);
   vs_out.pos = vec3(model * vec4(aPos, 1.0f));
   vs_out.color = color;
   gl_PointSize = point_size;
   gl_Position = camera * vec4(vs_out.pos, 1.0f);
}
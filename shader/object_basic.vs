#version 450
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 model;
uniform mat4 camera;

out VS_OUT {
   vec3 normal;
   vec3 pos;
} vs_out;

void main() {
   vs_out.normal = normalize(mat3(transpose(inverse(model))) * aNormal);
   vs_out.pos = vec3(model * vec4(aPos, 1.0f));
   // gl_Position = camera * model * vec4(aPos, 1.0f);
}
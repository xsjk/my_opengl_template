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

uniform vec3 color;

void main() {
   vs_out.normal = normalize(mat3(transpose(inverse(model))) * aNormal);
   vs_out.pos = vec3(model * vec4(aPos, 1.0f));
   switch (gl_VertexID % 3) {
      case 0: vs_out.color = vec3(1.0f, 0.0f, 0.0f); break;
      case 1: vs_out.color = vec3(0.0f, 1.0f, 0.0f); break;
      case 2: vs_out.color = vec3(0.0f, 0.0f, 1.0f); break;
   }
   gl_Position = camera * vec4(vs_out.pos, 1.0f);
}
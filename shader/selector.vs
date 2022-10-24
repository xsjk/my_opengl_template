#version 450
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 camera;
out vec3 color;
uniform vec3 ID;

void main() {
   color = ID;
   color[1] = 1;
   gl_Position = camera * model * vec4(aPos, 1.0f);
}
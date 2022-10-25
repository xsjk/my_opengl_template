#version 450
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 camera;
out vec3 color;
uniform vec3 ID;
void main() {
   color = ID;
   color[1] = gl_VertexID;
   color[2] = gl_InstanceID;
   gl_Position = camera * model * vec4(aPos, 1.0f);
   gl_PointSize = 30.0;
}
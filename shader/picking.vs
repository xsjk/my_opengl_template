#version 450
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;

uniform mat4 model;
uniform mat4 camera;
out vec3 color;
uniform vec3 ID;
void main() {
   color = ID;
   if(aUV==vec2(-1,-1)) {
      color[1] = gl_VertexID;
   } else {
         
      color[1] = aUV[0];
      color[2] = aUV[1];
      // if (aUV==vec2(0,0))
      //    color[1] = color[2] = 25;

   }
   gl_Position = camera * model * vec4(aPos, 1.0f);
   gl_PointSize = 50.0;
}
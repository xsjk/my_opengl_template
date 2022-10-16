#version 450
layout (location = 0) in vec3 aPos;

uniform mat4 camera;
uniform mat4 model;

struct Light {
    // for point light and spot light
    vec4 position;
    float constant;
    float linear;
    float quadratic; 
    
    // for directional light and spot light
    vec3 direction;

    // for spot light
    float cutOff;
    float outerCutOff;

    // for all lights
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
};

uniform Light light;


out VS_OUT {
   vec3 color;
   vec4 position;
} vs_out;

void main() {
   vs_out.color = light.color;
   
   vec3 FragPos = normalize(aPos);

   if (light.direction!=vec3(0.0)) {
      if (FragPos.y < cos(radians(light.outerCutOff+0))) {
         FragPos = vec3(0);
         vs_out.color = vec3(0);
      } else {
         // vs_out.color = light.color * FragPos.y;
         // color = vec3(0);
      }
   }
   
   
   gl_Position = camera * model * vec4(FragPos/10, 1.0);
}
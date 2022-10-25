#include <light.h>
#include <camera.h>

void Light::follow(const Camera& camera) {
  switch (getType()) {
    case POINT:
      position = vec4(camera.get_eye(), 1);
      break;
    case DIRECTIONAL:
      direction = normalize(-camera.get_eye());
      break;
    case SPOT:
      direction = inverse(mat3(View(camera).matrix())) * camera.get_front();
      position = vec4(camera.get_eye(),1);
      break;
  }
  
}

RenderData Light::display {
  [](GLfloat t, GLfloat u) { return vec3(sin(u)*cos(t), cos(u), sin(u)*sin(t)); },
  0, 2*PI, PI/12, 
  0, PI, PI/120
};

Light::Type Light::getType() const {
  if (position.w == 0) return DIRECTIONAL;
  else if (direction == vec3(0)) return POINT;
  else return SPOT;
}
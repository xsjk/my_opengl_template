#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <defines.h>
#include <render_data.h>

class Camera;

struct Light {
  // for point light and spot light
  vec4 position   = {0.0, 0.0, 0.0, 0.0};
  float constant  = 1.0;
  float linear    = 0.09;
  float quadratic = 0.032;

  // for directional light and spot light
  vec3 direction  = {0.0, 0.0, 0.0};

  // for spot light
  float cutOff    = 12.5f;
  float outerCutOff = 15.0f;

  // for all lights
  vec3 color      = {1.0, 1.0, 1.0};
  vec3 ambient    = {0.0, 0.0, 0.0};
  vec3 diffuse    = {0.5, 0.5, 0.5};
  vec3 specular   = {1.0, 1.0, 1.0};

  static RenderData display;

  void follow(const Camera&);

  bool displaying = true;

  enum Type {
    POINT,
    DIRECTIONAL,
    SPOT,
  };

  Type getType() const;

  inline float attenuation(float distance) const {
    return 1.0 / (constant + (linear + quadratic * distance) * distance);
  }
};



#endif
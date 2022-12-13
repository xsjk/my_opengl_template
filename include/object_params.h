#ifndef _OBJECT_PARAMS_
#define _OBJECT_PARAMS_

#include <transform.h>

class AbsoluteObjectParams {
  protected:
    void __update__() {}
    vec3 color;
    float opacity = 1;
    bool visibility = true;
    float point_size = 10;
    GLenum buffer_mode = GL_STATIC_DRAW;
  public:
    AbsoluteObjectParams(vec3 color=vec3{.8}) : color(color) {}
    SETTER_GETTER(color)
    SETTER_GETTER(visibility)
    SETTER_GETTER(buffer_mode) 
    SETTER_GETTER(opacity)
    SETTER_GETTER(point_size)
};

class ObjectParams : public AbsoluteObjectParams, public Affine {
  protected:
    void __update__() {
      Affine::__update__(); 
      AbsoluteObjectParams::__update__();
    }
  public:
    ObjectParams(vec3 color=vec3{.8}, Affine affine={})
     : AbsoluteObjectParams(color), Affine(affine) {}
};

#endif
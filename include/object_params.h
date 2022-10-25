#ifndef _OBJECT_PARAMS_
#define _OBJECT_PARAMS_

#include <transform.h>

class ObjectParams : public Affine {
  protected:
    void __update__() {}
    vec3 color;
    float opacity = 1;
    bool visibility = true;
    float point_size = 10;
    GLenum buffer_mode = GL_STATIC_DRAW;
  public:
    ObjectParams(vec3 color=vec3{.8}, Affine affine={})
     : Affine(affine), color(color) {}

    SETTER_GETTER(color, vec3)
    SETTER_GETTER(visibility, bool)
    SETTER_GETTER(buffer_mode,GLenum) 
    SETTER_GETTER(opacity,float)
    SETTER_GETTER(point_size,float)
};

#endif
#ifndef _OBJECT_PARAMS_
#define _OBJECT_PARAMS_

#include <transform.h>

class ObjectParams : public Affine {
  protected:
    void __update__() {}
    vec3 color;
    bool visibility = true;
    GLenum buffer_mode;
  public:
    ObjectParams(vec3 color=vec3{.8}, Affine affine={})
     : Affine(affine), color(color) {}

    SETTER_GETTER(color, vec3)
    SETTER_GETTER(visibility, bool)
    SETTER_GETTER(buffer_mode,GLenum)  
};

#endif
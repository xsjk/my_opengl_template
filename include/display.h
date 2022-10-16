#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <defines.h>
#include <camera.h>

class Display {
    void __update__();
    GLint x, y;
    GLsizei width, height;
  public:
    enum Type {
      DEFAULT,
      LIGHT_TRACKING,
    } mode = DEFAULT;

    SETTER_GETTER(x, GLint);
    SETTER_GETTER(y, GLint);
    SETTER_GETTER(width, GLsizei);
    SETTER_GETTER(height, GLsizei);

    Camera camera;
    void activate();
    Display(GLint x, GLint y, GLsizei width, GLsizei height);
};


#endif
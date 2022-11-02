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
      FOLLOW_CAMERA,
      LIGHT_TRACKING,
      TEMP_FIXED,
      FIXED
    } mode = FOLLOW_CAMERA;

    SETTER_GETTER(x, GLint);
    SETTER_GETTER(y, GLint);
    SETTER_GETTER(width, GLsizei);
    SETTER_GETTER(height, GLsizei);

    Camera camera;
    void activate();
    Display(GLint x, GLint y, GLsizei width, GLsizei height);
    
    bool contains(GLuint x, GLuint y) const;
    bool contains(glm::vec<2, unsigned> pos) const;

    vec2 screen_to_camera(const vec2& p) const;
    vec2 camera_to_screen(const vec2& p) const;
};


#endif
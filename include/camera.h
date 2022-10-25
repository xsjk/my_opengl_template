#ifndef _CAMERA_H
#define _CAMERA_H

#include <defines.h>

class Shader;
class Light;

#include <transform.h>

class Camera : public View, public Perspective {
  GLfloat movementSpeed, rotationSpeed;
  void __update__();
public:
  Camera(GLfloat zoom=45,GLfloat aspect=16.0/9.0, GLfloat movementSpeed=0.01, GLfloat rotationSpeed=0.1);
  void move(GLfloat x, GLfloat y, GLfloat z);
  void zoom(GLfloat offset);
  mat4 matrix() const;
  SETTER_GETTER(movementSpeed, GLfloat)
  SETTER_GETTER(rotationSpeed, GLfloat)
  void KeyboardPressingCallback(int key, int mod);
  void CursorMoveCallback(GLfloat xoffset, GLfloat yoffset);
  void ScrollCallback(GLfloat yoffset);

  void follow(const Light& light);

  vec3 camera_to_world(const vec3&) const;
  vec3 world_to_camera(const vec3&) const;

};



#endif
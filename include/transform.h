#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <defines.h>

class Shader;

class Affine {
protected:
  vec3 displacement, rotation, scale;
  mat4 mat;
  void __update__();
public:
  /**
   * @brief Construct an Affine matrix
   * 
   * @param displacement 
   * @param rotation 
   * @param scale 
   */
  Affine(vec3 displacement=vec3(0), vec3 rotation=vec3(0), vec3 scale=vec3(1));
  MAT_GETTER
  SETTER_GETTER(displacement, vec3)
  SETTER_GETTER(rotation, vec3)
  SETTER_GETTER(scale, vec3)
};


class View {
  GLfloat yaw, pitch;
  vec3 eye = {0,0,1}, front = {0,0,-1}, up = {0,1,0};
  mat4 mat;
  void __update__();
public:
  /**
   * @brief Construct a View matrix
   * 
   * @param yaw 
   * @param pitch 
   */
  View(GLfloat yaw=0, GLfloat pitch=0);
  void displace(vec3 v);
  void rotate(GLfloat yaw_offset, GLfloat pitch_offset);
  MAT_GETTER
  SETTER_GETTER(yaw, GLfloat)
  SETTER_GETTER(pitch, GLfloat)
  SETTER_GETTER(eye, vec3)
  SETTER_GETTER(front, vec3)
  SETTER_GETTER(up, vec3)
};


class Perspective {
  GLfloat fov, aspect, near, far;
  mat4 mat;
  void __update__();
public:
  /**
   * @brief Construct a new Perspective object
   * 
   * @param fov 
   * @param aspect 
   * @param near 
   * @param far 
   */
  Perspective(GLfloat fov, GLfloat aspect, GLfloat near=0.1, GLfloat far=100);
  MAT_GETTER
  SETTER_GETTER(fov, GLfloat)
  SETTER_GETTER(aspect, GLfloat)
  SETTER_GETTER(near, GLfloat)
  SETTER_GETTER(far, GLfloat)
};


#endif
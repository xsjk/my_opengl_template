#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <defines.h>

class Shader;

class Rotation {
  quat q;
  mat3 mat;
  inline void __update__() { mat = mat4_cast(q); }
public:
  Rotation(quat q={1,0,0,0}) : q(q) { __update__(); }
  Rotation(GLfloat angle, vec3 axis) : Rotation(angleAxis(angle, axis)) { __update__(); }
  Rotation(GLfloat yaw, GLfloat pitch, GLfloat roll) : Rotation(angleAxis(yaw, vec3(0, 1, 0)) * angleAxis(pitch, vec3(1, 0, 0)) * angleAxis(roll, vec3(0, 0, 1))) { __update__(); }
  MAT_GETTER
  SETTER_GETTER(q)
  [[nodiscard]] inline GLfloat get_yaw() const { return atan2(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x * q.x + q.y * q.y));}
  [[nodiscard]] inline GLfloat get_pitch() const { return asin(2 * (q.w * q.y - q.z * q.x));}
  [[nodiscard]] inline GLfloat get_roll() const { return atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z));}
  [[nodiscard]] inline vec3 get_euler_angles() const { return vec3(get_yaw(), get_pitch(), get_roll());}
  inline auto& set_euler_angles(GLfloat yaw, GLfloat pitch, GLfloat roll) {
    q = quat(cos(yaw / 2) * cos(pitch / 2) * cos(roll / 2) + sin(yaw / 2) * sin(pitch / 2) * sin(roll / 2),
             sin(yaw / 2) * cos(pitch / 2) * cos(roll / 2) - cos(yaw / 2) * sin(pitch / 2) * sin(roll / 2),
             cos(yaw / 2) * sin(pitch / 2) * cos(roll / 2) + sin(yaw / 2) * cos(pitch / 2) * sin(roll / 2),
             cos(yaw / 2) * cos(pitch / 2) * sin(roll / 2) - sin(yaw / 2) * sin(pitch / 2) * cos(roll / 2));
    __update__();
    return *this;
  }
  inline auto& set_euler_angles(vec3 angles) { return set_euler_angles(angles.x, angles.y, angles.z); }
  inline auto& rotate(GLfloat angle, vec3 axis) { return set_q(angleAxis(angle, axis) * q); }
  inline auto operator*(const Rotation& other) const { return Rotation(q * other.q); }
  inline auto operator*(const vec3& v) const { return vec3(mat * v); }
  inline operator mat4() const { return mat4(mat); }
  inline auto& operator*=(const Rotation& other) { return set_q(q * other.q); }
};


class Affine {
protected:
  vec3 displacement, scale;
  Rotation rotation;
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
  Affine(vec3 displacement=vec3(0), Rotation rotation={}, vec3 scale=vec3(1));
  MAT_GETTER
  SETTER_GETTER(displacement)
  SETTER_GETTER(rotation)
  SETTER_GETTER(scale)

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
  void set_rotation(const Rotation& rotation);
  MAT_GETTER
  SETTER_GETTER(yaw)
  SETTER_GETTER(pitch)
  SETTER_GETTER(eye)
  SETTER_GETTER(front)
  SETTER_GETTER(up)
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
  SETTER_GETTER(fov)
  SETTER_GETTER(aspect)
  SETTER_GETTER(near)
  SETTER_GETTER(far)
};


#endif
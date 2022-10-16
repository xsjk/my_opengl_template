#include <transform.h>
#include <shader.h>

// Model

Affine::Affine(vec3 displacement, vec3 rotation, vec3 scale) 
  : displacement(displacement), rotation(rotation), scale(scale) {
  __update__();
}

void Affine::__update__() {
  mat = glm::translate(mat4(1), displacement);
  mat = glm::rotate(mat, glm::radians(rotation.x), vec3(1, 0, 0));
  mat = glm::rotate(mat, glm::radians(rotation.y), vec3(0, 1, 0));
  mat = glm::rotate(mat, glm::radians(rotation.z), vec3(0, 0, 1));
  mat = glm::scale(mat, scale);
}


// View

View::View(GLfloat yaw, GLfloat pitch) : yaw(yaw), pitch(pitch) {
  __update__();
}

void View::__update__() {
  mat = glm::rotate(mat4(1), glm::radians(pitch), vec3(1, 0, 0))
      * glm::rotate(mat4(1), glm::radians(yaw), vec3(0, 1, 0))
      * glm::lookAt(eye, eye+front, up);
}

void View::displace(vec3 v) {
  mat3 m = glm::rotate(mat4(1), glm::radians(yaw), vec3(0, -1, 0));
  eye += m*v;
  __update__(); 
}

void View::rotate(GLfloat yaw_offset, GLfloat pitch_offset) {
  yaw += yaw_offset;
  pitch += pitch_offset;
  if (pitch > 89) pitch = 89;
  else if (pitch < -89) pitch = -89;
  __update__();
}

// Perspective

Perspective::Perspective(GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far) 
  : fov(fov), aspect(aspect), near(near), far(far) {
  __update__();
}

void Perspective::__update__() {
  mat = glm::perspective(glm::radians(fov), aspect, near, far);
}

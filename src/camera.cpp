#include <camera.h>

#include <transform.h>
#include <shader.h>
#include <scene.h>
#include <light.h>

// Camera

Camera::Camera(GLfloat zoom, GLfloat aspect, GLfloat movementSpeed, GLfloat rotationSpeed) 
  : View(), Perspective(zoom, aspect),
    movementSpeed(movementSpeed), rotationSpeed(rotationSpeed) {}


void Camera::move(GLfloat x, GLfloat y, GLfloat z) {
  displace(vec3(x,y,z)*movementSpeed);
}

void Camera::zoom(GLfloat offset){
  set_fov(glm::min(glm::max(get_fov()-offset,1.0f),45.0f));
}

mat4 Camera::matrix() const {
  return Perspective::matrix()*View::matrix();
}

void Camera::__update__() {}

void Camera::KeyboardPressingCallback(int key, int mod) {
  int v = mod & GLFW_MOD_CONTROL ? 10 : 1;
  switch(key){
    case GLFW_KEY_W:            move(0,0,-v);     break;
    case GLFW_KEY_S:            move(0,0,v);      break;
    case GLFW_KEY_A:            move(-v,0,0);     break;
    case GLFW_KEY_D:            move(v,0,0);      break;
    case GLFW_KEY_SPACE:        move(0,v,0);      break;
    case GLFW_KEY_LEFT_SHIFT:   move(0,-v,0);     break;
  }
  // std::cout << get_eye() << std::endl;
}

void Camera::CursorMoveCallback(GLfloat xoffset, GLfloat yoffset) {
  rotate(xoffset * rotationSpeed, yoffset * rotationSpeed);
}

void Camera::ScrollCallback(GLfloat yoffset) {
  zoom(yoffset);
}


void Camera::follow(const Light& light) {
  switch (light.getType()) {
    case Light::POINT:
      set_eye(vec3(light.position)+vec3(0,5,0));
      set_front(vec3(0,-1,-.1));
      break;
    case Light::SPOT:
      set_eye(vec3(light.position));
      set_front(light.direction);
      break;
    case Light::DIRECTIONAL:
      set_front(light.direction);
      set_eye(vec3(-normalize(get_front()))*5.0f);
      break;
  }
  
}
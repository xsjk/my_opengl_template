#include <display.h>

Display::Display(GLint x, GLint y, GLsizei width, GLsizei height)
      : x{x}, y{y}, width{width}, height{height}, camera{45,(float)width/height} {}

void Display::activate() {
  glViewport(x, y, width, height);
  glScissor(x, y, width, height);
}

void Display::__update__() {
  camera.set_aspect((float)width/height);
}

bool Display::contains(GLuint x_, GLuint y_) const { 
  return x_ >= x && x_ <= x+width && y_ >= y && y_ <= y+height; 
}

bool Display::contains(glm::vec<2, unsigned> pos) const { 
  return contains(pos.x, pos.y); 
}

vec2 Display::screen_to_camera(const vec2& p) const {
  return {2*(p.x-x)/width-1, 2*(p.y-y)/height-1};
}

vec2 Display::camera_to_screen(const vec2& p) const {
  return {(p.x+1)*width/2+x, (p.y+1)*height/2+y};
}
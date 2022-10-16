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
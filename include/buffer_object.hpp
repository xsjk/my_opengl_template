#ifndef _BUFFER_OBJECT_HPP_
#define _BUFFER_OBJECT_HPP_
#include <defines.h>

class Vertex;

template<typename T>
struct BufferObject {
  mutable GLuint ID = 0;
  GLenum buffer_mode;
  std::vector<T> data;
  BufferObject(GLenum buffer_mode) : buffer_mode(buffer_mode) {}
  inline void gen() const { 
    if (ID == 0) glGenBuffers(1, &ID); 
  }
  inline void bind() const { 
    glBindBuffer(buffer_mode, ID); 
  }
  inline void unbind() { 
    glBindBuffer(buffer_mode, 0); 
  }
  inline void set_data(GLenum usage=GL_STATIC_DRAW) const { 
    glBufferData(buffer_mode, data.size() * sizeof(T), data.data(), usage);
  }
  inline void init() const {
    gen(), update();
  }
  inline void update(GLenum usage=GL_STATIC_DRAW) const {
    bind(), set_data(usage);
  }
};

struct VertexBufferObject : BufferObject<Vertex> {
  VertexBufferObject() : BufferObject<Vertex>(GL_ARRAY_BUFFER) {}
  VertexBufferObject(const std::vector<Vertex>& vertices)
   : BufferObject<Vertex>(GL_ARRAY_BUFFER) {
    data = vertices;
  }
  void draw(GLenum draw_mode) const {
    glDrawArrays(draw_mode, 0, data.size());
  }
};

struct ElementBufferObject : BufferObject<GLuint> {
  ElementBufferObject() : BufferObject<GLuint>(GL_ELEMENT_ARRAY_BUFFER) {}
  ElementBufferObject(const std::vector<GLuint>& indices)
   : BufferObject<GLuint>(GL_ELEMENT_ARRAY_BUFFER) {
    data = indices;
  }
  void draw(GLenum draw_mode) const {
    glDrawElements(draw_mode, data.size(), GL_UNSIGNED_INT, 0);
  }
};


#endif
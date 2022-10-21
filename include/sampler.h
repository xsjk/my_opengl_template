#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#include <defines.h>

class Vertex;

class Sampler {
public:  
  GLuint resolution_m = 20;
  GLuint resolution_n = 20;

  virtual Vertex evaluate(GLfloat u, GLfloat v) const = 0;
};

#endif
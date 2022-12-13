#ifndef _VERTEX_H_
#define _VERTEX_H_
#include <defines.h>

struct Vertex {
  vec3 position;
  union {
    vec3 normal;    // used for meshes
    vec3 direction; // used for curves
  };
  union {
    vec2 uv = {-1, -1};
    vec2 texcoord;
  };
};


#endif
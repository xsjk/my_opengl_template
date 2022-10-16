#ifndef _RENDER_DATA_UPDATER_H_
#define _RENDER_DATA_UPDATER_H_

#include <defines.h>

class Vertex;
class RenderDataHandler;


/// @brief an updater directly modifies vertices and indices
struct RenderDataUpdater {
  bool enable_EBO;
  GLenum draw_mode;
  RenderDataUpdater(GLenum draw_mode, bool enable_EBO);
  virtual ~RenderDataUpdater();
  virtual void operator()(std::vector<Vertex>& vertices, std::vector<GLuint>& indices) const;
  RenderDataHandler operator()() const;
};


#endif
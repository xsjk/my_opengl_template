#include <render_data_updater.h>
#include <render_data.h>

RenderDataUpdater::RenderDataUpdater(GLenum draw_mode, bool enable_EBO) : draw_mode{draw_mode}, enable_EBO{enable_EBO} {}

RenderDataUpdater::~RenderDataUpdater() {}

void RenderDataUpdater::operator()(std::vector<Vertex>&, std::vector<GLuint>&) const {}

RenderDataHandler RenderDataUpdater::operator()() const {
  return RenderDataHandler{this}; 
}
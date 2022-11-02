#ifndef _PICKING_H_
#define _PICKING_H_

#include <defines.h>

struct Pixel { GLubyte red, green, blue; };

class PickingTexture {
  GLuint m_fbo = 0, m_pickingTexture = 0, m_depthTexture = 0;
public:
  ~PickingTexture();

  bool init(unsigned WindowWidth, unsigned WindowHeight);

  void activate();
  void deactivate();

  vec3 read(unsigned x, unsigned y);
};

#include <shader.h>

/// @brief this shader is only used for uploading the ID of each object to the GPU
class PickingShader : public Shader {
  static const char* vs_file, *fs_file;
  public:
    using Shader::Shader;
    void set(const Camera &) const;
    void init();
    void draw(const ObjectData &) const;
};


#include <utils.h>
#include <handler.hpp>


class Scene;

struct Picking {
  PickingTexture texture;
  PickingShader shader;
  unsigned current_id = 0, last_id = 0, dragging_id = 0;
  inline bool dragging() const { return dragging_obj; }
  vec2 current_uv = {-1,-1}, last_uv = {-1,-1}, dragging_uv = {-1,-1};
  ObjectData* current_obj = nullptr, *last_obj = nullptr, *dragging_obj = nullptr;
  void init(unsigned width, unsigned height);
  void update(const std::vector<Handler<Scene>>& scenes);
  vec3 get_current(unsigned x, unsigned y);
  vec3 get_current(glm::vec<2,unsigned> pos);
  void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};


#endif
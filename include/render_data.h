#ifndef _RENDER_DATA_H_
#define _RENDER_DATA_H_
#include <shader.h>
#include <vertex.h>

struct RenderData {
  mutable GLuint VBO = 0, EBO = 0;
  void draw_elements(GLenum mode) const;
  void draw_arrays(GLenum mode) const;
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
};


class Window;
struct RenderDataUpdater;
class RenderDataHandler {

  friend class Window;

  

protected:
  std::shared_ptr<RenderData> data{ std::make_shared<RenderData>() };
  std::shared_ptr<RenderDataUpdater> updater = nullptr;
  GLuint VAO, &VBO{ data->VBO }, &EBO{ data->EBO };
  std::vector<Vertex> &vertices{ data->vertices };
  std::vector<GLuint> &indices{ data->indices };
  GLenum draw_mode = GL_TRIANGLES;
  bool enable_EBO = true;

  
private:

  friend class RenderDataUpdater;
  /// @brief create an object with updater only once (static data)
  template<class T, typename U=std::enable_if_t<std::is_base_of_v<RenderDataUpdater,T>>>
  RenderDataHandler(const T* u) : draw_mode{u->draw_mode}, enable_EBO{u->enable_EBO} {(*u)(vertices, indices);}

public:

  inline bool is_static() const { return updater == nullptr; }

  /// @brief create an object binding to an updater (dynamic data)
  template<class T, typename U=std::enable_if_t<std::is_base_of_v<RenderDataUpdater,T>>>
  RenderDataHandler(const T& u) : updater(std::make_shared<T>(u)), draw_mode{u.draw_mode}, enable_EBO{u.enable_EBO} { u(vertices, indices);}

  template<class T, typename U=std::enable_if_t<std::is_base_of_v<RenderDataUpdater,T>>>
  RenderDataHandler(std::shared_ptr<T> u) : updater(u), draw_mode{u->draw_mode}, enable_EBO{u->enable_EBO} { (*u)(vertices, indices);}

  RenderDataHandler(
    const std::vector<Vertex>& vertices, 
    const std::optional<std::vector<GLuint>>& indices = std::nullopt, 
    GLenum mode = GL_TRIANGLES
  );

  /// @brief create mesh from parameter function
  /// @param f: the function used for generation of the mesh
  RenderDataHandler(
    std::function<vec3(GLfloat,GLfloat)> f, 
    GLfloat t_min=0, GLfloat t_max=1, GLfloat t_delta=0.05,
    GLfloat u_min=0, GLfloat u_max=1, GLfloat u_delta=0.05
  );

  /// @brief create curve from parameter function
  /// @param f: the function used to generate of the curve
  RenderDataHandler(
    std::function<vec3(GLfloat)> f, 
    GLfloat t_min=0, GLfloat t_max=1, GLfloat t_delta=0.05
  );
     

  /// @brief create Mesh from file
  /// @param path: the path of mesh file
  /// @param mode: the draw mode of the mesh
  /// @param enable_EBO: enable EBO or not
  RenderDataHandler(
    const std::string &path, 
    GLenum draw_mode=GL_TRIANGLES, 
    bool enable_EBO=true
  );

  void setup() const;

  void update(GLenum) const;


  /// @brief generate array buffer object
  void genAO();

  /// @brief generate vertex buffer object and element buffer object
  void genBO();

  /// @brief draw the mesh
  void draw() const;
  // void draw();

  void load(const std::string &path);

  bool operator==(const RenderDataHandler &rhs) const { return data==rhs.data; }
  operator bool() { return bool(data); }
};

#endif
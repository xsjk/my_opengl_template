#ifndef _RENDER_DATA_H_
#define _RENDER_DATA_H_
#include <shader.h>
#include <vertex.h>
#include <utils.h>

#include <buffer.hpp>
#include <handler.hpp>
#include <transform.h>

enum class MeshPrimitiveType {
  Cube,
  Sphere,
  Square,
  Other
};

struct VertexArrayObject {
  mutable GLuint ID = 0;
  inline void gen() const { 
    if (ID == 0) glGenVertexArrays(1, &ID); 
  }
  inline void bind() const { 
    glBindVertexArray(ID); 
  }
  inline void unbind() { 
    glBindVertexArray(0); 
  }
  inline void init() const {
    gen();
  }
};


struct BufferData {
  Handler<VertexBufferObject> VBO;
  Handler<ElementBufferObject> EBO;
};


class Window;
class ObjectData;
class ObjectDataUpdater;
class RenderData {

  friend class Window;
  friend class ObjectData;
  RenderData();

  bool enable_EBO = true;

  Handler<BufferData> data;

  GLuint VAO = 0;  
  GLenum draw_mode = GL_TRIANGLES;

  MeshPrimitiveType mesh_primitive_type = MeshPrimitiveType::Other;

  vec3 center = vec3(0);

public:

  inline auto & vertices() const { return data->VBO->data; };
  inline auto & indices() const { return data->EBO->data; };
  
  RenderData(GLenum draw_mode, bool enable_EBO);
  RenderData(const ObjectDataUpdater& );


  RenderData(
    const std::vector<Vertex>& vertices, 
    const std::optional<std::vector<GLuint>>& indices = std::nullopt, 
    GLenum mode = GL_TRIANGLES
  );

  RenderData(
    const Handler<VertexBufferObject>& VBO,
    std::optional<Handler<ElementBufferObject>> EBO = std::nullopt,
    GLenum mode = GL_TRIANGLES
  );

  /// @brief create mesh from parameter function
  /// @param f: the function used for generation of the mesh
  RenderData(
    std::function<vec3(GLfloat,GLfloat)> f, 
    GLfloat t_min=0, GLfloat t_max=1, GLfloat t_delta=0.05,
    GLfloat u_min=0, GLfloat u_max=1, GLfloat u_delta=0.05
  );

  RenderData(
    std::function<vec3(GLfloat,GLfloat)> f, 
    std::vector<GLfloat> t_list,
    std::vector<GLfloat> u_list
  );

  /// @brief create curve from parameter function
  /// @param f: the function used to generate of the curve
  RenderData(
    std::function<vec3(GLfloat)> f, 
    GLfloat t_min=0, GLfloat t_max=1, GLfloat t_delta=0.05
  );

  RenderData(
    std::function<vec3(GLfloat)> f, 
    std::vector<GLfloat> t_list
  );

  RenderData(
    MeshPrimitiveType type
  );
     

  /// @brief create Mesh from file
  /// @param path: the path of mesh file
  /// @param mode: the draw mode of the mesh
  /// @param enable_EBO: enable EBO or not
  RenderData(
    const std::string &path, 
    GLenum draw_mode=GL_TRIANGLES, 
    bool enable_EBO=true
  );

  void save(const std::string &path) const;

  /// @brief copy
  RenderData deepcopy() const;

  void setupVAO() const;

  void update(GLenum) const;

  /// @brief setup array buffer object
  void genVAO();

  /// @brief bind array buffer object
  void bindVAO() const;

  /// @brief setup vertex buffer object and element buffer object
  void setupBO();

  /// @brief draw the mesh
  void draw() const;
  // void draw();

  void bind() const;

  void load(const std::string &path);

  void apply(const Affine&);

  bool contains(const vec3&) const;

  bool operator==(const RenderData &rhs) const { return data==rhs.data; }
  operator bool() { return bool(data); }
};


#endif
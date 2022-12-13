#include <utils.h>
#include <object_data.h>
#include <object_data_updater.h>
#include <window.h>
#include <bezier.h>
#include <render_data.h>



void RenderData::bind() const {
  data->VBO->bind();
  if(enable_EBO)
    data->EBO->bind();
}

void RenderData::draw() const {

  if(!data) return;
  glBindVertexArray(VAO);
  if (enable_EBO)
    data->EBO->draw(draw_mode);
  else 
    data->VBO->draw(draw_mode);
}

void RenderData::setupVAO() const {
  if(!data) return;
  /// @warning this should be executed for each window once (though there is usually only one window)
  bindVAO();
  bind();
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(vec3));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (2*sizeof(vec3)));
}

void RenderData::update(GLenum usage) const {
  data->VBO->update(usage);
  if (enable_EBO)
    data->EBO->update(usage);
}

void RenderData::genVAO() { 
  if (VAO == 0) glGenVertexArrays(1, &VAO);
  bindVAO();
}

void RenderData::bindVAO() const {
  glBindVertexArray(VAO);
}

void RenderData::setupBO() { 
  data->VBO->init();
  if (enable_EBO) 
    data->EBO->init();
}

void RenderData::load(const std::string &path) {
  std::ifstream infile(path);
  std::vector<vec3> normals;
  GLuint index=0;
  std::string line;
  while (std::getline(infile, line)) {
    if(line.size()==0 || line[0]=='#') continue;
    std::istringstream iss(line);
    std::string type; iss >> type;
    if (type == "v") {
      vec3 v; iss >> v.x >> v.y >> v.z;
      vertices().push_back({v});
    } else if (type == "n") {
      vec3 v; iss >> v.x >> v.y >> v.z;
      if(normals.size()==0) normals.reserve(vertices().size());
      normals.push_back(v);
    } else if (type == "f") {
      GLuint vertex_idx,normal_idx;
      for(auto i=0; i<3; ++i){
        iss >> vertex_idx >> normal_idx;
        indices().push_back(vertex_idx);
        vertices()[vertex_idx].normal = normals[normal_idx];
      }
    }
  }
}


RenderData::RenderData() {}

RenderData::RenderData(GLenum draw_mode, bool enable_EBO)
  : draw_mode { draw_mode }, enable_EBO { enable_EBO } {}

RenderData::RenderData(const ObjectDataUpdater& u) 
  : RenderData{u.draw_mode,u.enable_EBO} {}

RenderData::RenderData(
  const std::vector<Vertex>& ver, 
  const std::optional<std::vector<GLuint>>& ind, 
  GLenum draw_mode
) : 
  draw_mode(draw_mode), 
  enable_EBO(ind.has_value())
{
  vertices() = ver;
  if (enable_EBO)
    indices() = ind.value();
}

RenderData::RenderData(
  const Handler<VertexBufferObject>& VBO,
  std::optional<Handler<ElementBufferObject>> EBO,
  GLenum draw_mode
) :
  enable_EBO(EBO.has_value()),
  draw_mode(draw_mode)
{
  data->VBO = VBO;
  if (enable_EBO)
    data->EBO = EBO.value();
}


RenderData::RenderData(
  std::function<vec3(float,float)> f,
  float t_min, float t_max, float t_step,
  float u_min, float u_max, float u_step
) : draw_mode{GL_TRIANGLES}, enable_EBO{true} {

  unsigned t_num = ((t_max - t_min) / t_step) + 1;
  unsigned u_num = ((u_max - u_min) / u_step) + 1;
  vertices().reserve(t_num*u_num);
  const float dt = t_step/100, du = u_step/100;
  for(unsigned i=0; i<t_num; ++i) {
    for(unsigned j=0; j<u_num; ++j){
      vec3 pos = f(t_min+i*t_step, u_min+j*u_step);
      vec3 normal = cross(
        (f(t_min+i*t_step+dt, u_min+j*u_step)-f(t_min+i*t_step, u_min+j*u_step))/dt,
        (f(t_min+i*t_step, u_min+j*u_step+du)-f(t_min+i*t_step, u_min+j*u_step))/du
      );
      vertices().push_back({pos,normal,vec2{i*t_step,j*u_step}});
    }
  }

  indices().reserve(2*(t_num-1)*(u_num-1));
  for(unsigned i=1; i<t_num; ++i)
    for(unsigned j=1; j<u_num; ++j) {
      GLuint index[4] = {
        (i-1)*u_num + (j-1),
        (i-1)*u_num + j,
        i*u_num + (j-1),
        i*u_num + j
      };
      indices().push_back(index[0]);
      indices().push_back(index[1]);
      indices().push_back(index[2]);
      indices().push_back(index[1]);
      indices().push_back(index[2]);
      indices().push_back(index[3]);
    }
}

RenderData::RenderData(
  std::function<vec3(float,float)> f,
  std::vector<GLfloat> t_list,
  std::vector<GLfloat> u_list
) : draw_mode{GL_TRIANGLES}, enable_EBO{true} {
  const unsigned t_num = t_list.size(), u_num = u_list.size();

  vertices().reserve(t_num*u_num);
  const float dt = 0.0001, du = 0.0001;
  for(auto t:t_list) {
    for(auto u:u_list){
      vec3 pos = f(t,u);
      vec3 normal = cross(f(t,u+du)-f(t,u), f(t+dt,u)-f(t,u));
      vertices().push_back({pos,normal,vec2{t,u}});
    }
  }
  indices().reserve(6*(t_num-1)*(u_num-1));
  for(unsigned i=1; i<t_num; ++i)
    for(unsigned j=1; j<u_num; ++j) {
      GLuint index[4] = {
        (i-1)*u_num + (j-1),
        (i-1)*u_num + j,
        i*u_num + (j-1),
        i*u_num + j
      };
      indices().push_back(index[0]);
      indices().push_back(index[1]);
      indices().push_back(index[2]);
      indices().push_back(index[1]);
      indices().push_back(index[2]);
      indices().push_back(index[3]);
    }
}

RenderData::RenderData(
  std::function<vec3(float)> f,
  float t_min, float t_max, float t_step
) : draw_mode{GL_LINE_STRIP}, enable_EBO{false} {

  unsigned t_num = ((t_max - t_min) / t_step) + 1;

  vertices().reserve(t_num);
  const float dt = t_step/100;
  for(unsigned i=0; i<t_num; ++i) {
      vec3 pos = f(t_min+i*t_step);
      vec3 dir = (f(t_min+i*t_step+dt)-f(t_min+i*t_step))/dt;
      vertices().push_back({pos,dir,vec2{i*t_step,0}});
  }
}

RenderData::RenderData(
  std::function<vec3(float)> f,
  std::vector<GLfloat> t_list
) : draw_mode{GL_LINE_STRIP}, enable_EBO{false} {
  const GLfloat dt = 0.001;
  vertices().reserve(t_list.size());
  for(auto t: t_list) {
      vec3 pos = f(t);
      vec3 dir = (f(t+dt)-f(t))/dt;
      vertices().push_back({pos,dir});
  }
}


RenderData::RenderData(const std::string &path, GLenum mode, bool enable_EBO)
 : draw_mode{mode}, enable_EBO{enable_EBO} {
  load(getPath(path));
}

RenderData::RenderData(MeshPrimitiveType type)
 : mesh_primitive_type{type} {
  switch (type) {
    case MeshPrimitiveType::Cube:
      enable_EBO = false;
      draw_mode = GL_QUADS;
      vertices() = {
        {vec3(-0.5, -0.5, -0.5), vec3( 0.0,  0.0, -1.0), vec2(0.0, 0.0)}, // forward
        {vec3( 0.5, -0.5, -0.5), vec3( 0.0,  0.0, -1.0), vec2(1.0, 0.0)},
        {vec3( 0.5,  0.5, -0.5), vec3( 0.0,  0.0, -1.0), vec2(1.0, 1.0)},
        {vec3(-0.5,  0.5, -0.5), vec3( 0.0,  0.0, -1.0), vec2(0.0, 1.0)},

        {vec3(-0.5, -0.5,  0.5), vec3( 0.0,  0.0,  1.0), vec2(0.0, 0.0)}, // back
        {vec3( 0.5, -0.5,  0.5), vec3( 0.0,  0.0,  1.0), vec2(1.0, 0.0)},
        {vec3( 0.5,  0.5,  0.5), vec3( 0.0,  0.0,  1.0), vec2(1.0, 1.0)},
        {vec3(-0.5,  0.5,  0.5), vec3( 0.0,  0.0,  1.0), vec2(0.0, 1.0)},

        {vec3(-0.5,  0.5,  0.5), vec3(-1.0,  0.0,  0.0), vec2(1.0, 0.0)}, // left
        {vec3(-0.5,  0.5, -0.5), vec3(-1.0,  0.0,  0.0), vec2(1.0, 1.0)},
        {vec3(-0.5, -0.5, -0.5), vec3(-1.0,  0.0,  0.0), vec2(0.0, 1.0)},
        {vec3(-0.5, -0.5,  0.5), vec3(-1.0,  0.0,  0.0), vec2(0.0, 0.0)},

        {vec3( 0.5,  0.5,  0.5), vec3( 1.0,  0.0,  0.0), vec2(1.0, 0.0)}, // right
        {vec3( 0.5,  0.5, -0.5), vec3( 1.0,  0.0,  0.0), vec2(1.0, 1.0)},
        {vec3( 0.5, -0.5, -0.5), vec3( 1.0,  0.0,  0.0), vec2(0.0, 1.0)},
        {vec3( 0.5, -0.5,  0.5), vec3( 1.0,  0.0,  0.0), vec2(0.0, 0.0)},

        {vec3(-0.5, -0.5, -0.5), vec3( 0.0, -1.0,  0.0), vec2(0.0, 1.0)}, // down
        {vec3( 0.5, -0.5, -0.5), vec3( 0.0, -1.0,  0.0), vec2(1.0, 1.0)},
        {vec3( 0.5, -0.5,  0.5), vec3( 0.0, -1.0,  0.0), vec2(1.0, 0.0)},
        {vec3(-0.5, -0.5,  0.5), vec3( 0.0, -1.0,  0.0), vec2(0.0, 0.0)},

        {vec3(-0.5,  0.5, -0.5), vec3( 0.0,  1.0,  0.0), vec2(0.0, 1.0)}, // up
        {vec3( 0.5,  0.5, -0.5), vec3( 0.0,  1.0,  0.0), vec2(1.0, 1.0)},
        {vec3( 0.5,  0.5,  0.5), vec3( 0.0,  1.0,  0.0), vec2(1.0, 0.0)},
        {vec3(-0.5,  0.5,  0.5), vec3( 0.0,  1.0,  0.0), vec2(0.0, 0.0)},
      };
    break;
    case MeshPrimitiveType::Sphere: {
      enable_EBO = true;
      draw_mode = GL_TRIANGLES;
      static constexpr unsigned primitive_sphere_resolution = 16;
      // compute vertices and indices for +x part
      auto & vertices = this->vertices();
      auto & indices = this->indices();
      const float deg_to_rad = PI / float(180);
      for (int i = 0; i < primitive_sphere_resolution; ++i) {
        float phi = deg_to_rad * (float(45) - float(90) * float(i) / (primitive_sphere_resolution - 1)); // [45°, -45°]
        vec3 d_phi(-float(std::sin(phi)), float(std::cos(phi)), 0);
        auto k1 = unsigned(i * primitive_sphere_resolution);
        auto k2 = k1 + unsigned(primitive_sphere_resolution);
        float t = float(i) / (primitive_sphere_resolution - 1);
        for (int j = 0; j < primitive_sphere_resolution; ++j, ++k1, ++k2) {
          // vertex
          float theta = deg_to_rad * (float(-45) + float(90) * float(j) / (primitive_sphere_resolution - 1)); // [-45°, 45°]
          vec3 d_theta(-float(std::sin(theta)), 0, -float(std::cos(theta)));
          vec3 n = glm::normalize(glm::cross(d_theta, d_phi));
          float s = (float)j / (primitive_sphere_resolution - 1);
          vertices.emplace_back(Vertex{n * float(0.5), n, vec2(s, t)});

          // index
          if (i < (primitive_sphere_resolution - 1) && j < (primitive_sphere_resolution - 1)) {
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
          }
        }
      }
      size_t num_vertices_per_part = vertices.size();
      size_t num_indices_per_part = indices.size() / 3;
      // -x
      auto index_start = vertices.size();
      for (size_t i = 0; i < num_vertices_per_part; ++i) {
        vec3 v = vertices[i].position;
        vec3 n = vertices[i].normal;
        vertices.emplace_back(Vertex{vec3(-v.x, v.y, -v.z), vec3(-n.x, n.y, -n.z), vertices[i].uv});
      }
      for (size_t i = 0; i < num_indices_per_part; ++i) {
        indices.push_back(indices[i*3+0] + index_start);
        indices.push_back(indices[i*3+1] + index_start);
        indices.push_back(indices[i*3+2] + index_start);
      }
      // +y
      index_start = vertices.size();
      for (size_t i = 0; i < num_vertices_per_part; ++i) {
        vec3 v = vertices[i].position;
        vec3 n = vertices[i].normal;
        vertices.emplace_back(Vertex{vec3(-v.z, v.x, -v.y), vec3(-n.z, n.x, -n.y), vertices[i].uv});
      }
      for (size_t i = 0; i < num_indices_per_part; ++i) {
        indices.push_back(indices[i*3+0] + index_start);
        indices.push_back(indices[i*3+1] + index_start);
        indices.push_back(indices[i*3+2] + index_start);
      }
      // -y
      index_start = vertices.size();
      for (size_t i = 0; i < num_vertices_per_part; ++i) {
        vec3 v = vertices[i].position;
        vec3 n = vertices[i].normal;
        vertices.emplace_back(Vertex{vec3(-v.z, -v.x, v.y), vec3(-n.z, -n.x, n.y), vertices[i].uv});
      }
      for (size_t i = 0; i < num_indices_per_part; ++i) {
        indices.push_back(indices[i*3+0] + index_start);
        indices.push_back(indices[i*3+1] + index_start);
        indices.push_back(indices[i*3+2] + index_start);
      }
      // +z
      index_start = vertices.size();
      for (size_t i = 0; i < num_vertices_per_part; ++i) {
        vec3 v = vertices[i].position;
        vec3 n = vertices[i].normal;
        vertices.emplace_back(Vertex{vec3(-v.z, v.y, v.x), vec3(-n.z, n.y, n.x), vertices[i].uv});
      }
      for (size_t i = 0; i < num_indices_per_part; ++i) {
        indices.push_back(indices[i*3+0] + index_start);
        indices.push_back(indices[i*3+1] + index_start);
        indices.push_back(indices[i*3+2] + index_start);
      }
      // -z
      index_start = vertices.size();
      for (size_t i = 0; i < num_vertices_per_part; ++i) {
        vec3 v = vertices[i].position;
        vec3 n = vertices[i].normal;
        vertices.emplace_back(Vertex{vec3(v.z, v.y, -v.x), vec3(n.z, n.y, -n.x), vertices[i].uv});
      }
      for (size_t i = 0; i < num_indices_per_part; ++i) {
        indices.push_back(indices[i*3+0] + index_start);
        indices.push_back(indices[i*3+1] + index_start);
        indices.push_back(indices[i*3+2] + index_start);
      }
    }
    break;
    case MeshPrimitiveType::Square: {
      enable_EBO = false;
      draw_mode = GL_QUADS;
      vertices() = {
        {vec3(-.5,-.5, 0), vec3(0, 0, -1), vec2(0, 0)},
        {vec3( .5,-.5, 0), vec3(0, 0, -1), vec2(1, 0)},
        {vec3( .5, .5, 0), vec3(0, 0, -1), vec2(1, 1)},
        {vec3(-.5, .5, 0), vec3(0, 0, -1), vec2(1, 1)},
      };
    }
  }

}


RenderData RenderData::deepcopy() const {
  if (enable_EBO)
    return RenderData{vertices(),indices(),draw_mode};
  else
    return RenderData{vertices(),std::nullopt,draw_mode};
}


void RenderData::save(const std::string &path) const {
  std::cerr << path ;
  std::ofstream file(path, std::ios::out);
  if (!file.is_open())
    throw std::runtime_error("Failed to open file: " + path);
  file << "# OBJ file\n";
  for (auto &v: vertices())
    file << "v " << v.position.x << " " << v.position.y << " " << v.position.z << '\n';
  for (auto &v: vertices())
    file << "n " << v.normal.x << " " << v.normal.y << " " << v.normal.z << '\n';
  if (enable_EBO) {
    for (unsigned i=0; i<indices().size();)
      file << "f " << indices()[i++] 
            << ' ' << indices()[i++] 
            << ' ' << indices()[i++] << '\n';
  } else {
    file << "f ";
    for (unsigned i=0; i<vertices().size(); )
      file << i++ << '\n';
    file << "\n";
  }
}

void RenderData::apply(const Affine& t) {
  std::cout << "apply: " << t.get_rotation().get_q() << " to " << vertices().size() << " vertices\n";
  for (auto &v: vertices()) {
    v.position *= t.get_scale();
    v.position = t.get_rotation() * v.position;
    v.position += t.get_displacement();
  }
  center += t.get_displacement();
}

bool RenderData::contains(const vec3& pos) const {
  switch(mesh_primitive_type) {
    case MeshPrimitiveType::Cube:
      return true;
    case MeshPrimitiveType::Sphere:
      return true;
    case MeshPrimitiveType::Square:
      return true;
    case MeshPrimitiveType::Other:
      throw std::runtime_error("RenderData::contains() not implemented for MeshPrimitiveType::Other");
  }
}
  
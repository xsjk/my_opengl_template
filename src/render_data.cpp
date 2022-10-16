#include <utils.h>
#include <object_data.h>
#include <window.h>
#include <bezier.h>

void RenderData::draw_elements(GLenum mode) const {
  glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, nullptr);
}

void RenderData::draw_arrays(GLenum mode) const {
  glDrawArrays(mode, 0, vertices.size());
}

void RenderDataHandler::draw() const {
  if(updater){
    (*updater)(vertices,indices);
    update(GL_STREAM_DRAW);
  }
  if(!data) return;
  glBindVertexArray(VAO);
  if (enable_EBO)
    data->draw_elements(draw_mode);
  else 
    data->draw_arrays(draw_mode);
}

void RenderDataHandler::setup() const {
  if(!data) return;
  /// @warning this should be executed for each window once (though there is usually only one window)
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(vec3));
}

void RenderDataHandler::update(GLenum usage) const {
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), usage);
  if (enable_EBO) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), usage);
  }
}

void RenderDataHandler::genAO() { 
  glGenVertexArrays(1, &VAO); 
  glBindVertexArray(VAO);
}

void RenderDataHandler::genBO() { 
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}


void RenderDataHandler::load(const std::string &path) {
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
      vertices.push_back({v});
    } else if (type == "n") {
      vec3 v; iss >> v.x >> v.y >> v.z;
      if(normals.size()==0) normals.reserve(vertices.size());
      normals.push_back(v);
    } else if (type == "f") {
      GLuint vertex_idx,normal_idx;
      for(auto i=0; i<3; ++i){
        iss >> vertex_idx >> normal_idx;
        indices.push_back(vertex_idx);
        vertices[vertex_idx].normal = normals[normal_idx];
      }
    }
  }
}


RenderDataHandler::RenderDataHandler(
  const std::vector<Vertex>& ver, 
  const std::optional<std::vector<GLuint>>& ind, 
  GLenum draw_mode
) : 
  draw_mode(draw_mode), 
  enable_EBO(ind.has_value())
{
  vertices = ver;
  if (enable_EBO)
    indices = ind.value();
}


RenderDataHandler::RenderDataHandler(
  std::function<vec3(float,float)> f,
  float t_min, float t_max, float t_step,
  float u_min, float u_max, float u_step
) : draw_mode{GL_TRIANGLES}, enable_EBO{true} {

  unsigned t_num = ((t_max - t_min) / t_step) + 1;
  unsigned u_num = ((u_max - u_min) / u_step) + 1;
  vertices.reserve(t_num*u_num);
  float dt = t_step/100, du = u_step/100;
  for(unsigned i=0; i<t_num; ++i) {
    for(unsigned j=0; j<u_num; ++j){
      vec3 pos = f(t_min+i*t_step, u_min+j*u_step);
      vec3 normal = cross(
        (f(t_min+i*t_step+dt, u_min+j*u_step)-f(t_min+i*t_step, u_min+j*u_step))/dt,
        (f(t_min+i*t_step, u_min+j*u_step+du)-f(t_min+i*t_step, u_min+j*u_step))/du
      );
      vertices.push_back({pos,normal});
    }
  }

  indices.reserve(2*(t_num-1)*(u_num-1));
  for(unsigned i=1; i<t_num; ++i)
    for(unsigned j=1; j<u_num; ++j) {
      GLuint index[4] = {
        (i-1)*u_num + (j-1),
        (i-1)*u_num + j,
        i*u_num + (j-1),
        i*u_num + j
      };
      indices.push_back(index[0]);
      indices.push_back(index[1]);
      indices.push_back(index[2]);
      indices.push_back(index[1]);
      indices.push_back(index[2]);
      indices.push_back(index[3]);
    }
}

RenderDataHandler::RenderDataHandler(
  std::function<vec3(float)> f,
  float t_min, float t_max, float t_step
) : draw_mode{GL_LINE_STRIP}, enable_EBO{false} {

  unsigned t_num = ((t_max - t_min) / t_step) + 1;

  vertices.reserve(t_num);
  float dt = t_step/100;
  for(unsigned i=0; i<t_num; ++i) {
      vec3 pos = f(t_min+i*t_step);
      vec3 dir = (f(t_min+i*t_step+dt)-f(t_min+i*t_step))/dt;
      vertices.push_back({pos,dir});
  }
}



RenderDataHandler::RenderDataHandler(const std::string &path, GLenum mode, bool enable_EBO)
 : draw_mode{mode}, enable_EBO{enable_EBO} {
  load(getPath(path));
}


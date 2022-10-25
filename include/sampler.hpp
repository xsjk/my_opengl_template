#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#include <defines.h>
#include <object_data.h>

class Vertex;


template<class S>
class Sampler {

  inline unsigned index(unsigned i, unsigned j) {
    return i*(resolution[1]+1)+j;
  }

  const S& surface;

public:  
  glm::vec<2,GLuint> resolution;

  std::vector<Vertex> vertices;

  std::vector<GLuint> mesh_indices;

  std::vector<GLuint> line_indices;

  Sampler(const S& surface, glm::vec<2,GLuint> reslution={20,20});

};




template<class S>
class AdaptiveSampler {
  
  const S& surface;

  double dS, order;

  struct Line {
    unsigned i,j;
    inline operator unsigned() const { return i^j; }
    inline Line(unsigned i,unsigned j) {
      if (i<j)  this->i = i, this->j = j;
      else      this->i = j, this->j = i;
    }
  };

public:
  std::vector<Vertex> vertices;

  std::vector<GLuint> mesh_indices;

  std::vector<GLuint> line_indices;

  AdaptiveSampler(const S& surface, double dS=0.02, double order=2);

};





template<class S>
Sampler<S>::Sampler(const S& surface, glm::vec<2,GLuint> resolution)
 : surface{surface}, resolution{resolution} {
  const auto m = resolution[0], n = resolution[1];
  vertices.reserve((m+1)*(n+1));
  for (auto i=0; i<=m; ++i)
    for (auto j=0; j<=n; ++j)
      vertices.push_back(surface.evaluate(float(i)/m, float(j)/n));
  mesh_indices.reserve(m*n*6);
  line_indices.reserve(2*(m*n*2 + n + m));
  for (auto i=0; i<m; ++i) {
    for (auto j=0; j<n; ++j) {
      const unsigned idx[4] = {index(i,j), index(i,j+1), index(i+1,j), index(i+1,j+1)};
      mesh_indices.push_back(idx[0]);
      mesh_indices.push_back(idx[1]);
      mesh_indices.push_back(idx[2]);
      mesh_indices.push_back(idx[1]);
      mesh_indices.push_back(idx[3]);
      mesh_indices.push_back(idx[2]);
      line_indices.push_back(idx[0]);
      line_indices.push_back(idx[1]);
      line_indices.push_back(idx[0]);
      line_indices.push_back(idx[2]);
    }
    line_indices.push_back(index(i,n));
    line_indices.push_back(index(i+1,n));
  }
  for (auto j=0; j<n; ++j) {
    line_indices.push_back(index(m,j));
    line_indices.push_back(index(m,j+1));
  }
}



#include <delaunator.h>
#include <simpson.h>

template<class S>
AdaptiveSampler<S>::AdaptiveSampler(const S& surface, double dS, double order) 
  : surface{surface}, dS{dS}, order{order} {
    std::function<double(double,double)> f = 
      [&surface,order] (double u, double v) { 
        return surface.cur_measure(u,v,order); 
      };

    auto sample = Simpson::fast_partition(f, dS, 0, 1, 0, 1);
    std::vector<double> uv_coord;
    uv_coord.reserve(sample.size()*2);
    std::cout << sample.size() << std::endl;
    vertices.clear();
    vertices.reserve(sample.size());
    for (auto [u,v]: sample) {
      vertices.push_back(surface.evaluate(u, v));
      uv_coord.push_back(u);
      uv_coord.push_back(v);
    }
    delaunator::Delaunator d{uv_coord};
    mesh_indices = std::move(d.triangles);

    // get line indices from mesh_indices;
    std::unordered_set<Line,std::hash<unsigned>> lines;
    for (auto i=0; i<mesh_indices.size(); i+=3) {
      lines.emplace(mesh_indices[i], mesh_indices[i+1]);
      lines.emplace(mesh_indices[i+1], mesh_indices[i+2]);
      lines.emplace(mesh_indices[i+2], mesh_indices[i]);
    }
    line_indices.reserve(lines.size()*2);
    for (auto [i,j]: lines) {
      line_indices.push_back(i);
      line_indices.push_back(j);
    }

  }



#endif
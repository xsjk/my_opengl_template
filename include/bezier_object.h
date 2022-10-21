#ifndef _BEZIER_OBJECT_H_
#define _BEZIER_OBJECT_H_

#include <defines.h>
#include <object.h>
#include <bezier.h>
#include <object_data.h>
#include <object_data_updater.h>


template<class T>
class PointUpdater : public ObjectDataUpdaterPlus {
  std::shared_ptr<T> surface;
public:
  PointUpdater(std::shared_ptr<T> surface) : ObjectDataUpdaterPlus{GL_POINTS, false}, surface{surface} {}
  virtual void update(ObjectData& data) const override {
    data.vertices.resize((surface->resolution_m+1)*(surface->resolution_n+1));
    for (GLuint i=0; i<=surface->resolution_m; ++i)
      for (GLuint j=0; j<=surface->resolution_n; ++j) 
        data.vertices[i*(1+surface->resolution_n)+j] = surface->evaluate(float(i)/surface->resolution_m, float(j)/surface->resolution_n);
  }
};

template<class T>
class GridUpdater : public ObjectDataUpdaterPlus {
  std::shared_ptr<T> surface;
public:
  GridUpdater(std::shared_ptr<T> surface) : ObjectDataUpdaterPlus{GL_LINES, true}, surface{surface} {}
  virtual void update(ObjectData& data) const override {
    data.vertices.resize((surface->resolution_m+1)*(surface->resolution_n+1));
    for (GLuint i=0; i<=surface->resolution_m; ++i)
      for (GLuint j=0; j<=surface->resolution_n; ++j) 
        data.vertices[i*(1+surface->resolution_n)+j] = surface->evaluate(float(i)/surface->resolution_m, float(j)/surface->resolution_n);

    data.indices.resize(surface->resolution_m*2*(surface->resolution_n+1) + surface->resolution_n*2*(surface->resolution_m+1));
    GLuint index_offset = 0;
    for (GLuint i=0; i<=surface->resolution_m; ++i) {
      for (GLuint j=0; j<surface->resolution_n; ++j) {
        data.indices[index_offset++] = i*(surface->resolution_n+1)+j;
        data.indices[index_offset++] = i*(surface->resolution_n+1)+j+1;
      }
    }
    for (GLuint j=0; j<=surface->resolution_n; ++j) {
      for (GLuint i=0; i<surface->resolution_m; ++i) {
        data.indices[index_offset++] = i*(surface->resolution_n+1)+j;
        data.indices[index_offset++] = (i+1)*(surface->resolution_n+1)+j;
      }
    }
  }
};

template<class T>
struct FaceUpdater : public ObjectDataUpdaterPlus {
  std::shared_ptr<T> surface;
public:
  FaceUpdater(std::shared_ptr<T> surface) : ObjectDataUpdaterPlus{GL_TRIANGLES, true}, surface{surface} {}
  virtual void update(ObjectData& data) const override {
    data.vertices.resize((surface->resolution_m+1)*(surface->resolution_n+1));
    for (GLuint i=0; i<=surface->resolution_m; ++i)
      for (GLuint j=0; j<=surface->resolution_n; ++j)
        data.vertices[i*(1+surface->resolution_n)+j] = surface->evaluate(float(i)/surface->resolution_m, float(j)/surface->resolution_n);

    data.indices.resize(surface->resolution_m*surface->resolution_n*6);
    GLuint index_offset = 0;
    for (GLuint i=0; i<surface->resolution_m; ++i) {
      for (GLuint j=0; j<surface->resolution_n; ++j) {
        GLuint index[4] = {
              i*(surface->resolution_n+1)+j,
              i*(surface->resolution_n+1)+j+1,
          (i+1)*(surface->resolution_n+1)+j,
          (i+1)*(surface->resolution_n+1)+j+1,
        };
        data.indices[index_offset++] = index[0];
        data.indices[index_offset++] = index[1];
        data.indices[index_offset++] = index[2];
        data.indices[index_offset++] = index[1];
        data.indices[index_offset++] = index[3];
        data.indices[index_offset++] = index[2];
      }
    }
  }
};

// template<T>



class BezierSurfaceObject {

public:


  std::shared_ptr<BezierSurface> surface; // data this show be kept well

  struct {
    std::shared_ptr<PointUpdater<BezierSurface>> point;
    std::shared_ptr<GridUpdater<BezierSurface>> grid;
    std::shared_ptr<FaceUpdater<BezierSurface>> face;
  } updater = {
    std::make_shared<PointUpdater<BezierSurface>>(surface),
    std::make_shared<GridUpdater<BezierSurface>>(surface),
    std::make_shared<FaceUpdater<BezierSurface>>(surface),
  };


  public:
    /// @brief create bezier surface from a single surface
    /// @param surface 
    BezierSurfaceObject(const BezierSurface& surface);



};



#endif
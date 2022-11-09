#ifndef _SPLINE_SURFACE_OBJECT_H_
#define _SPLINE_SURFACE_OBJECT_H_

#include <defines.h>
#include <object_data.h>

#include <sampler.hpp>
#include <window.h>
#include <scene.h>
#include <group.h>

template<class S>
class SurfaceObject : public Group {
  
  struct HighLightedObj : public ObjectData {
    SurfaceObject<S>& parent;
    HighLightedObj(SurfaceObject<S>& s);
    virtual void mouseover(vec2 uv) override;
    virtual void mouseout(vec2 uv) override;
  };

  struct ControlPointsObj : public ObjectData {
    SurfaceObject<S>& parent;
    ControlPointsObj(SurfaceObject<S>& s);
    virtual void ondrag(unsigned x, unsigned y, int) override;
    virtual void mouseover(int) override;
    virtual void mouseout(int) override;
    virtual void onclick(int,int,int) override;
  };

  Triangulation triangulation_mode = Triangulation::adaptive;

  void __update__() {
    resample();
  }

  void all_need_update();

  const S origin;

  bool selected;

  bool changed = false;

public:

  SETTER_GETTER(triangulation_mode)



  // buffer objects
  Handler<VertexBufferObject> control_points_VBO, surface_VBO;
  Handler<ElementBufferObject> control_points_edge_EBO, control_points_face_EBO, surface_edge_EBO, surface_face_EBO;


  



  ///@note just store vertices in each ObjectData, since it's too complicated to modify the capsulation


  Handler<S> surface; // data this show be kept well

  
  Handler<ControlPointsObj> control_points{*this};
  
  Handler<ObjectData> control_points_edge{
    RenderData{control_points_VBO, control_points_edge_EBO, GL_LINES}
  };
  Handler<ObjectData> control_points_face{
    RenderData{control_points_VBO, control_points_face_EBO, GL_TRIANGLES}
  };

  Handler<ObjectData> surface_points{
    RenderData{surface_VBO, std::nullopt, GL_POINTS}
  };
  Handler<ObjectData> surface_edge{
    RenderData{surface_VBO, surface_edge_EBO, GL_LINES}
  };
  Handler<HighLightedObj> surface_face{*this};

  /// @brief display mode
  /// @details 
  ///   - 1: show mesh points
  ///   - 2: show mesh grid,
  ///   - 4: show mesh faces,
  ///   - 8: show control points
  ///   - 16: show control grid,
  ///   - 32: show control faces

  enum {
    SURFACE_POINTS = 1,
    SURFACE_EDGE = 2,
    SURFACE_FACE = 4,
    CONTROL_POINTS = 8,
    CONTROL_EDGE = 16,
    CONTROL_FACE = 32
  };
  int display_mode;


  virtual void init(Scene& scene) override;

  virtual void KeyboardCallback(int key, int scancode, int action, int mods) override;


  
  /// @brief create bezier surface from a single surface
  /// @param surface 
  SurfaceObject<S>(const S& surface);

  void init(bool VBOonly = false);

  void set_control_point(unsigned i, unsigned j, vec3 point);

  void set_display_mode(int);

  void resample();

  void show(Handler<ObjectData>);

  void hide(Handler<ObjectData>);

};

template<class S>
SurfaceObject<S>::HighLightedObj::HighLightedObj(SurfaceObject<S>& s) : 
  ObjectData(RenderData{s.surface_VBO, s.surface_face_EBO, GL_TRIANGLES}), parent{s} {}

template<class S>
void SurfaceObject<S>::HighLightedObj::mouseover(vec2 uv) {
  parent.selected = true;
  if (parent.parent)
    set_color(0,1,0);
}

template<class S>
void SurfaceObject<S>::HighLightedObj::mouseout(vec2 uv) {
  parent.selected = false;
  if (parent.parent)
    set_color(0.8);
}


template<class S>
SurfaceObject<S>::ControlPointsObj::ControlPointsObj(SurfaceObject<S>& s)
  : ObjectData(RenderData{s.control_points_VBO, std::nullopt, GL_POINTS}), parent(s) {}  


template<class S>
void SurfaceObject<S>::ControlPointsObj::ondrag(unsigned x, unsigned y, int pointID) {
  auto scene = this->scene;
  if (scene) {
    auto& display = scene->display;
    auto old_world_coordinate = parent.control_points_VBO->data[pointID].position;
    // auto old_world_coordinate = parent.surface->evaluate(uv[0], uv[1]).position;
    auto old_camera_coordinate = display.camera.world_to_camera(old_world_coordinate);
    auto new_screen_coordinate = vec3(x, y, old_camera_coordinate.z);
    auto new_camera_coordinate = display.screen_to_camera(new_screen_coordinate);
    auto new_world_coordinate = display.camera.camera_to_world({new_camera_coordinate, old_camera_coordinate.z});
    unsigned n = parent.surface->control_points.front().size();
    parent.set_control_point(pointID / n, pointID % n, new_world_coordinate);
  } else {
    std::cout << "no scene" << std::endl;
  }
}

template<class S>
void SurfaceObject<S>::ControlPointsObj::mouseover(int pointID) {
  set_point_size(20.0);
  set_color(1,0,0);
}

template<class S>
void SurfaceObject<S>::ControlPointsObj::mouseout(int pointID) {
  set_color(0.8);
  set_point_size(10.0);
}

template<class S>
void SurfaceObject<S>::ControlPointsObj::onclick(int,int,int) {
  set_color(0);
}

template<class S>
SurfaceObject<S>::SurfaceObject(
  const S& surface
) : surface { surface }, origin{ surface } {  }


template<class S>
void SurfaceObject<S>::KeyboardCallback(int key, int scancode, int action, int mods) {
  // if(selected)
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_1:
        if (display_mode & SURFACE_POINTS) {
          hide(surface_points);
          display_mode &= ~SURFACE_POINTS;
        } else {
          show(surface_points);
          display_mode |= SURFACE_POINTS;
        }
        break;
      case GLFW_KEY_2:
        if (display_mode & SURFACE_EDGE) {
          hide(surface_edge);
          display_mode &= ~SURFACE_EDGE;
        } else {
          show(surface_edge);
          display_mode |= SURFACE_EDGE;
        }
        break;

      case GLFW_KEY_3:
        if (display_mode & SURFACE_FACE) {
          hide(surface_face);
          display_mode &= ~SURFACE_FACE;
        } else {
          show(surface_face);
          display_mode |= SURFACE_FACE;
        }
        break;

      case GLFW_KEY_4:
        if (display_mode & CONTROL_POINTS) {
          hide(control_points);
          display_mode &= ~CONTROL_POINTS;
        } else {
          show(control_points);
          display_mode |= CONTROL_POINTS;
        }
        break;

      case GLFW_KEY_5:
        if (display_mode & CONTROL_EDGE) {
          hide(control_points_edge);
          display_mode &= ~CONTROL_EDGE;
        } else {
          show(control_points_edge);
          display_mode |= CONTROL_EDGE;
        }
        break;

      case GLFW_KEY_6:
        if (display_mode & CONTROL_FACE) {
          hide(control_points_face);
          display_mode &= ~CONTROL_FACE;
        } else {
          show(control_points_face);
          display_mode |= CONTROL_FACE;
        }
        break;

      case GLFW_KEY_T:
        set_triangulation_mode( get_triangulation_mode() == Triangulation::adaptive ? Triangulation::uniform : Triangulation::adaptive );
        all_need_update();
        break;

      case GLFW_KEY_R:
        if(changed) {
          std::copy(origin.control_points.begin(),origin.control_points.end(),surface->control_points.begin());
          control_points_VBO->data.clear();
          init(true);
        }
        break;
      default:
        break;
    }
  }

}

template<class S>
void SurfaceObject<S>::init(bool VBOonly) {
  changed = false;

  unsigned m=surface->control_points.size(), n=surface->control_points.front().size();

  // create vertex list of control points
  control_points_VBO->data.reserve(m*n);
  for (auto& row: surface->control_points) 
    for (auto& point: row) {
      control_points_VBO->data.push_back({point,vec3(0)});
    }
  
  if(!VBOonly) {
    // create index list of control points lines
    control_points_edge_EBO->data.reserve(2*(2*(m-1)*(n-1)+(m-1)+(n-1)));
    for (unsigned i=0; i<m-1; i++) {
      for (unsigned j=0; j<n-1; j++) {
        control_points_edge_EBO->data.push_back(i*n+j);
        control_points_edge_EBO->data.push_back(i*n+j+1);
        control_points_edge_EBO->data.push_back(i*n+j);
        control_points_edge_EBO->data.push_back((i+1)*n+j);
      }
      control_points_edge_EBO->data.push_back(i*n+n-1);
      control_points_edge_EBO->data.push_back((i+1)*n+n-1);
    }
    for (unsigned j=0; j<n-1; j++) {
      control_points_edge_EBO->data.push_back((n-1)*n+j);
      control_points_edge_EBO->data.push_back((n-1)*n+j+1);
    }

    // create index list of control points faces
    control_points_face_EBO->data.reserve(2*(m-1)*(n-1));
    for (unsigned i=0; i<m-1; i++) {
      for (unsigned j=0; j<n-1; j++) {
        control_points_face_EBO->data.push_back(i*n+j);
        control_points_face_EBO->data.push_back(i*n+j+1);
        control_points_face_EBO->data.push_back((i+1)*n+j);
        control_points_face_EBO->data.push_back(i*n+j+1);
        control_points_face_EBO->data.push_back((i+1)*n+j+1);
        control_points_face_EBO->data.push_back((i+1)*n+j);
      }
    } 
  }
  // create vertex list of surface points
  resample();

  set_display_mode(display_mode);
  // set_display_mode(SURFACE_EDGE|CONTROL_POINTS|CONTROL_EDGE);
  // set_display_mode(SURFACE_FACE|CONTROL_POINTS|CONTROL_EDGE);


}

template<class S>
void SurfaceObject<S>::init(Scene& scene) {
  init();
  scene.add(control_points);
  scene.add(control_points_edge);
  scene.add(control_points_face);
  scene.add(surface_points);
  scene.add(surface_edge);
  scene.add(surface_face);
}

template<class S>
void SurfaceObject<S>::all_need_update() {
  control_points->set_need_update(true);
  control_points_edge->set_need_update(true);
  control_points_face->set_need_update(true);
  surface_edge->set_need_update(true);
  surface_points->set_need_update(true);
  surface_face->set_need_update(true);
}

template<class S>
void SurfaceObject<S>::set_control_point(unsigned i, unsigned j, vec3 point) {
  changed = true;
  all_need_update();

  unsigned n = surface->control_points.front().size();
  surface->control_points.at(i).at(j) = point;
  control_points_VBO->data.at(i*n+j) = {point,vec3(0)};
  resample(); //maybe asyncio
}

template<class S>
void SurfaceObject<S>::set_display_mode(int m) {
  display_mode = m;
  if (m & CONTROL_POINTS) {
    show(control_points);
  } else {
    hide(control_points);
  }
  if (m & CONTROL_EDGE) {
    show(control_points_edge);
  } else {
    hide(control_points_edge);
  }
  if (m & CONTROL_FACE) {
    show(control_points_face);
  } else {
    hide(control_points_face);
  }
  if (m & SURFACE_EDGE) {
    show(surface_edge);
  } else {
    hide(surface_edge);
  }
  if (m & SURFACE_POINTS) {
    show(surface_points);
  } else {
    hide(surface_points);
  }
  if (m & SURFACE_FACE) {
    show(surface_face);
  } else {
    hide(surface_face);
  }
}


template<class S>
void SurfaceObject<S>::resample() {
  switch (triangulation_mode) {
    case Triangulation::adaptive: {
      AdaptiveSampler<S> s{*surface};
      surface_VBO->data = std::move(s.vertices);
      surface_edge_EBO->data = std::move(s.line_indices);
      surface_face_EBO->data = std::move(s.mesh_indices);
    } break;
    case Triangulation::uniform: {
      Sampler<S> s{*surface};
      surface_VBO->data = std::move(s.vertices);
      surface_edge_EBO->data = std::move(s.line_indices);
      surface_face_EBO->data = std::move(s.mesh_indices);
    } break;
  }
}


template<class S>
void SurfaceObject<S>::hide(Handler<ObjectData> obj) {
  obj->set_buffer_mode(GL_STATIC_DRAW);
  obj->set_visibility(false);
}

template<class S>
void SurfaceObject<S>::show(Handler<ObjectData> obj) {
  obj->set_buffer_mode(GL_DYNAMIC_DRAW);
  obj->set_need_update(true);
  obj->set_visibility(true);
}




#endif
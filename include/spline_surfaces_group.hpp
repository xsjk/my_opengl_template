#ifndef _SPLINE_SURFACE_GROUP_HPP_
#define _SPLINE_SURFACE_GROUP_HPP_



#include <spline_surface_object.hpp>


template<class S>
class SurfaceGroup : public Group {

  virtual void init(Scene& scene) override;

  // virtual void KeyboardCallback(int key, int scancode, int action, int mods) override;

public:

  std::vector<Handler<SurfaceObject<S>>> surfaces;

  SurfaceGroup(const std::vector<S>& surfaces_data);

};


template<class S>
SurfaceGroup<S>::SurfaceGroup(const std::vector<S>& surfaces_data) {
  surfaces.reserve(surfaces_data.size());
  for(const auto& surface_data: surfaces_data)
    surfaces.emplace_back(surface_data);
}


template<class S>
void SurfaceGroup<S>::init(Scene& scene) {
  for (auto& surface: surfaces) {
    surface->set_display_mode(SurfaceObject<S>::SURFACE_FACE);
    surface->surface_face->set_color(0.8);
    surface->control_points_face->set_color(0.8);
    surface->set_triangulation_mode(Triangulation::uniform);
    scene.add(surface);
  }
}


#endif
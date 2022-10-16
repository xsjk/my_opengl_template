#include <utils.h>
#include <bezier_object.h>

#include <bezier.h>

BezierSurfaces::BezierSurfaces(
  const std::string& path
) : BezierSurfaces { BezierSurface::read(path) } {}

BezierSurfaces::BezierSurfaces(
  const BezierSurface& surface_generator
) : BezierSurfaces { std::vector<BezierSurface>{{surface_generator}} } {}

BezierSurfaces::BezierSurfaces(
  const std::vector<BezierSurface>& surface_generators
) : surface_generators { surface_generators } {
  
}

BezierSurface& BezierSurfaces::operator[](GLuint i) {
  return surface_generators[i];
}


void BezierSurfaces::operator()(ObjectData& data) const {
  // data.clear();
  // for (const auto& surface_generator : surface_generators) {
  //   surface_generator(data.vertices, data.indices);
  // }

}
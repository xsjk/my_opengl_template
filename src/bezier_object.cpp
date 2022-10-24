#include <utils.h>
#include <bezier_object.h>
#include <scene.h>
#include <bezier.h>

BezierSurfaceObject::BezierSurfaceObject(
  const BezierSurface& surface
) : surface { std::make_shared<BezierSurface>(surface) } {
  
}
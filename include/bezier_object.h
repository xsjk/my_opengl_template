#ifndef _BEZIER_OBJECT_H_
#define _BEZIER_OBJECT_H_

#include <defines.h>
#include <object.h>
#include <bezier.h>
#include <object_data.h>
#include <object_data_updater.h>


/// alternative control points by clicking
class ControlPoints : public ObjectData {

  std::vector <vec3> points;

public:

  ControlPoints() : ObjectData{RenderDataHandler{GL_POINT,false}} {}



};

class BezierSurfaceObject {

  ControlPoints control_points;

public:

  ///@note just store vertices in each ObjectData, since it's too complicated to modify the capsulation


  std::shared_ptr<BezierSurface> surface; // data this show be kept well

  /// @brief display mode
  /// @details 
  ///   - 1: show mesh points
  ///   - 2: show mesh grid,
  ///   - 4: show mesh faces,
  ///   - 8: show control points
  ///   - 16: show control grid,
  ///   - 32: show control faces
  unsigned mode = 0;
  
  inline bool if_show_mesh_points() const { return mode & 1; }
  inline bool if_show_mesh_grid() const { return mode & 2; }
  inline bool if_show_mesh_faces() const { return mode & 4; }
  inline bool if_show_control_points() const { return mode & 8; }
  inline bool if_show_control_grid() const { return mode & 16; }
  inline bool if_show_control_faces() const { return mode & 32; }


  public:
    /// @brief create bezier surface from a single surface
    /// @param surface 
    BezierSurfaceObject(const BezierSurface& surface);



};



#endif
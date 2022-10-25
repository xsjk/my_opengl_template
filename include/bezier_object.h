#ifndef _BEZIER_OBJECT_H_
#define _BEZIER_OBJECT_H_

#include <defines.h>
#include <object.h>
#include <bezier.h>
#include <object_data.h>
#include <object_data_updater.h>


class BezierSurfaceObject {

public:

  // buffer objects
  Handler<VertexBufferObject> control_points_VBO, surface_VBO;
  Handler<ElementBufferObject> control_points_edge_EBO, control_points_face_EBO, 
                              surface_points_EBO, surface_edge_EBO, surface_face_EBO;


  /// derive from ObjectData to rewrite mouse callback
  class ControlPointsObj : public ObjectData {
    BezierSurfaceObject& parent;
    bool is_drag;
  public:
    ControlPointsObj(BezierSurfaceObject& s);
    // virtual void onclick(int button, int mod, int pointID) override;
    // virtual void onrelease(int button, int mod, int pointID) override;
    virtual void ondrag(unsigned x, unsigned y, int pointID) override;
    virtual void mouseover(int) override;
    virtual void onclick(int,int,int) override;
  };



  ///@note just store vertices in each ObjectData, since it's too complicated to modify the capsulation


  Handler<BezierSurface> surface; // data this show be kept well

  
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
  Handler<ObjectData> surface_face{
    RenderData{surface_VBO, surface_face_EBO, GL_TRIANGLES}
  };



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

  void set_control_point(unsigned i, unsigned j, vec3 point);



  void resample();

  void show(Handler<ObjectData>);

  void hide(Handler<ObjectData>);

};



#endif
#include <utils.h>
#include <bezier_object.h>
#include <scene.h>
#include <bezier.h>
#include <sampler.hpp>
#include <window.h>

BezierSurfaceObject::ControlPointsObj::ControlPointsObj(BezierSurfaceObject& s)
  : ObjectData(RenderData{s.control_points_VBO, std::nullopt, GL_POINTS}), parent(s) {}  


void BezierSurfaceObject::ControlPointsObj::ondrag(unsigned x, unsigned y, int pointID) {
  auto scene = Window::focused_window->focused_scene;
  if (scene) {
    auto& display = scene->display;
    auto old_world_coordinate = parent.control_points_VBO->data[pointID].position;
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

void BezierSurfaceObject::ControlPointsObj::mouseover(int) {
  set_color(1,0,0);
}

void BezierSurfaceObject::ControlPointsObj::onclick(int,int,int) {
  set_color(0);
}

BezierSurfaceObject::BezierSurfaceObject(
  const BezierSurface& surface
) : surface { surface } {
  unsigned m=surface.control_points.size(), n=surface.control_points.front().size();

  // create vertex list of control points
  std::cout << std::boolalpha << control_points_VBO->data.empty() << std::endl;
  control_points_VBO->data.reserve(m*n);
  for (auto& row: surface.control_points) 
    for (auto& point: row) {
      control_points_VBO->data.push_back({point,vec3(0)});
    }

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

  // create vertex list of surface points
  resample();



  show(control_points);
  show(control_points_edge);
  hide(control_points_face);
  hide(surface_edge);
  hide(surface_points);
  show(surface_face);

}

void BezierSurfaceObject::set_control_point(unsigned i, unsigned j, vec3 point) {

  control_points->set_need_update(true);
  surface_face->set_need_update(true);


  unsigned n = surface->control_points.front().size();
  surface->control_points[i][j] = point;
  control_points_VBO->data[i*n+j] = {point,vec3(0)};
  resample(); //maybe asyncio
}



void BezierSurfaceObject::resample() {
  Sampler<BezierSurface> s{*surface};
  surface_VBO->data = std::move(s.vertices);
  surface_edge_EBO->data = std::move(s.line_indices);
  surface_face_EBO->data = std::move(s.mesh_indices);
}


void BezierSurfaceObject::hide(Handler<ObjectData> obj) {
  obj->set_buffer_mode(GL_STATIC_DRAW);
  obj->set_visibility(false);
}

void BezierSurfaceObject::show(Handler<ObjectData> obj) {
  obj->set_buffer_mode(GL_DYNAMIC_DRAW);
  // obj->set_buffer_mode(GL_STREAM_DRAW);
  obj->set_need_update(true);
  obj->set_visibility(true);
}
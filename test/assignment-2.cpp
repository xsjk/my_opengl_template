#include <utils.h>
#include <shader.h>
#include <camera.h>
#include <light.h>
#include <object_data.h>
#include <transform.h>
#include <scene.h>
#include <window.h>
#include <bezier.h>
#include <spline_surface_object.hpp>
#include <spline_surfaces_group.hpp>
#include <b-spline.h>
#include <axis.h>



int main() {



  const unsigned width = 2000, height = 1200;

  // create a window
  Window window{width, height, "tea"};

  // create a scene
  Handler<Scene> scene{0,0,width,height}, scene2 = scene.copy();
  // Handler<Scene> scene2{width/2,0,width/2,height/2};
  // Handler<Scene> scene2{width/2,0,width/2,height};
  // Handler<Scene> scene3{width/2,height/2,width/2,height/2};

  window.add(scene);
  // window.add(scene2);
  // window.add(scene3);

  scene2->display.set_width(width/3);
  scene2->display.set_height(height/3);
  scene2->display.camera.set_eye(5,4,4);
  scene2->display.camera.set_front(-1,-0.4,-1);
  scene2->display.mode = Display::FIXED;
  scene2->set_bg_color(0);
  window.add(scene2);
  



  ObjectShader 
    basic_shader{
      "shader/object_basic.vs",
      "shader/object_basic.fs",
      "shader/object_basic.gs"
    },
    edge_shader{
      "shader/object_basic.vs",
      "shader/object_basic.fs",
      "shader/edge.gs"
    },
    points_shader{
      "shader/object_basic.vs",
      "shader/object_basic.fs",
      "shader/points.gs"
    },
    normal_shader{
      "shader/object_basic.vs",
      "shader/object_basic.fs",
      "shader/normal.gs"
    },
    breathe_shader{
      "shader/object_basic.vs",
      "shader/object_basic.fs",
      "shader/breathe.gs"
    },
    explosion_shader{
      "shader/object_basic.vs",
      "shader/object_basic.fs",
      "shader/explosion.gs"
    },
    fragmentize_shader{
      "shader/object_basic.vs",
      "shader/object_basic.fs",
      "shader/fragmentize.gs"
    },
    colorful_shader{
      "shader/colorful.vs",
      "shader/object.fs",
    };


  auto 
    teaspoon = BezierSurface::read("assets/teaspoon.bzs"),
    teacup = BezierSurface::read("assets/teacup.bzs"),
    tea = BezierSurface::read("assets/tea.bzs");
  
  scene->add(SurfaceGroup{tea});

  // for (auto& s: tea){
  //   BsplineSurface s1 {s.control_points, {2,2}};
  //   // BsplineSurface s1 {s};
  //   scene->add(s1());
  // }

  {
    vec3 pos(0,1,0);
    for (int i=0;i<3;++i) {
      vec3 d{0};
      d[i] = 1;
      Arrow a{pos,pos+d};
      a.set_color(d);
      scene->add(a);
    }
    {
      Sphere s{pos, 0.1};
      scene->add(s);
    }
  }

  // add surface 
  {
    auto b4 = BezierSurface::read("assets/mybezier.bzs")[0];
    BsplineSurface b5 {b4.control_points,{2,2}};
    Handler<SurfaceObject<BsplineSurface>> s5 {b5};
    s5->set_display_mode(4);
    s5->set_triangulation_mode(Triangulation::uniform);
    s5->surface_face->set_color(0.8);
    scene->add(s5);
  }

  // load meshes
  RenderData 
    bunny("assets/bunny.obj"), 
    plane("assets/plane.obj"),
    sphere("assets/sphere.obj");
  



  // add lights

  {// spot light 1
    Light l {
      .position = {-3, 0.2, 2, 1},
      .quadratic = 0.1,
      .direction = {0, -1, -5},
      .color = {.5, 1, 0},
      .diffuse = vec3{.9},
    };
    scene->add(l);
  }
  
  {// point light 1
    Light l {
      .position = {0, 5, 2, 1},
      .ambient = vec3{.03},
    };
    scene->add(l);
  }


  // start mainloop
  Window::mainloop();

  return 0;
}
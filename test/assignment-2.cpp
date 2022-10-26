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



int main() {



  const unsigned width = 1600, height = 900;

  // create a window
  Window window{width, height, "Assignment 2"};

  // // create a scene
  // Scene scene{0,0,width/2,height};

  // // add a scene
  // scene->display.mode = Display::FOLLOW_CAMERA;
  // // scene->display.camera.set_eye(5, 2, 5);
  // // scene->display.camera.set_yaw(-45);
  // // scene->display.camera.set_pitch(15);
  // window.add(scene);

  // // add a scene
  // scene->display.mode = Display::LIGHT_TRACKING;
  // scene->display.set_width(width/2);
  // scene->display.set_height(height/2);
  // scene->display.set_x(width/2);
  // scene->set_bg_color(0);
  // window.add(scene);

  // // add a scene
  // scene->display.mode = Display::FIXED;
  // scene->display.set_width(width/2);
  // scene->display.set_height(height/2);
  // scene->display.set_x(width/2);
  // scene->display.set_y(height/2);
  // scene->display.camera.set_eye(6, 3, 5);
  // scene->display.camera.set_yaw(-45);
  // scene->display.camera.set_pitch(15);
  // window.add(scene);


  // create a scene
  Handler<Scene> scene{0,0,width/2,height};
  Handler<Scene> scene2{width/2,0,width/2,height/2};
  Handler<Scene> scene3{width/2,height/2,width/2,height/2};

  window.add(scene);
  window.add(scene2);
  window.add(scene3);






  // scene->display.set_width(width);
  // scene->display.set_height(height);
  // scene->display.set_x(0);
  // scene->display.set_y(0);
  // scene->display.mode = Display::FOLLOW_CAMERA;
  // window2.add(scene);



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

  // scene->add(tea[62]());
  // scene->add(tea[62]());


  
  for (auto &surface : tea) {
    // auto h = surface();
    // Handler<ObjectHighlight> highlight{h};
    // Handler<ObjectData> obj = highlight;
    // obj->mouseover();


    // scene->add(highlight);
    // scene->add(surface);
    // scene->add(surface());
    // scene->add(surface(), normal_shader);
    // scene->add(surface(), edge_shader);
    // scene->add(surface(), points_shader);
  }

  BezierCurve 
    b1 {{
          {0,0,0},
          {1,0,0},
          {1,1,0},
          {0,1,0},
          {0,0,0},
      }},
    b2 {{
          {1,1,0},
          {2,2,1},
          {1,1,2},
          {2,2,3},
          {1,1,4},
      }};

  BsplineSurface b6{{
    {{-0.5,-0.5,-2},{0.5,-0.5,-1},{1.5,-0.5,1},{2.5,-0.5,2}},
    {{-0.5,0.5,-2},{0.5,0.5,-1},{1.5,0.5,1},{2.5,0.5,2}},
    {{-0.5,1.5,-4},{0.5,1.5,-3},{1.5,1.5,-1},{2.5,1.5,2}},
    {{-0.5,2.5,-4},{0.5,2.5,-3},{1.5,2.5,-1},{2.5,2.5,2}}
  },{2,2}};



  auto b3 = BezierSurface::read("assets/mybezier.bzs")[0];
  BsplineSurface b5 {b3.control_points,{3,4}};

  // load meshes
  RenderData 
    bunny("assets/bunny.obj"), 
    plane("assets/plane.obj"),
    sphere("assets/sphere.obj"),
    curve1(b1),
    curve2(b2),
    surface1(b3)
    ;
  

  // scene->add(b1);
  // scene2->add(b5(), edge_shader);
  // scene->add(b7(), edge_shader);
  // scene->add(b3);
  // scene->add(b3(), edge_shader);
  // scene->add(b3(), normal_shader);

  // Handler<ObjectHighlight> iii {ObjectData{RenderData{
  //   VertexBufferObject{ ObjectData{sphere}.vertices() },
  //   std::nullopt, GL_POINTS 
  // }}};

  // scene->add(iii);

  // SurfaceObject ooo(b5);
  // std::cout << (ooo.control_points->get_ID()) << ' '
  //           << (ooo.control_points_edge->get_ID()) << ' '
  //           << (ooo.control_points_face->get_ID()) <<  ' '
  //           << (ooo.surface_points->get_ID()) << ' '
  //           << (ooo.surface_edge->get_ID()) << ' '
  //           << (ooo.surface_face->get_ID()) << ' '
  //           << 
  // std::endl;
  // std::cout << (ooo.control_points_VBO->data.size()) << ' '
  //           << (ooo.control_points_edge_EBO->data.size()) << ' '
  //           << (ooo.control_points_face_EBO->data.size()) << ' '
  //           << 
  // std::endl;
  // std::cout << (ooo.control_points->vertices().size()) << ' '
  //           << (ooo.control_points_edge->indices().size()) << ' '
  //           << (ooo.control_points_face->indices().size()) <<  ' '
  //           <<
  // std::endl;

  // ooo.surface_face->set_color(0.8);
  // std::cout << ooo.control_points_edge->get_color() << std::endl;
  // scene->add(ooo);

  // Handler<SurfaceObject<BsplineSurface>> bbb {b5};
  // bbb->set_display_mode(SurfaceObject<BsplineSurface>::SURFACE_EDGE|SurfaceObject<BsplineSurface>::CONTROL_POINTS);
  // bbb->set_triangulation_mode(Triangulation::uniform);
  // scene->add(bbb);


  // SurfaceObject oooo{teacup[0]};





  //  {
  //   Color color = {1.0f, 0.9f, 0.8f};
  //   ObjectData object{bunny, color};
  //   object.set_displacement(0, -0.18, 0);
  //   object.set_scale(5);
  //   scene->add(object);
  // }


 
    scene->add(SurfaceGroup{teaspoon});

//  {
//     Color color = {1.0f, 0.9f, 0.8f};
//     ObjectData object{bunny, color};
//     object.set_displacement(0, -0.18, 0);
//     object.set_scale(5);
//     // scene2->add(object);
//   }

  
 {
    Color color = {1.0f, 0.9f, 0.8f};
    ObjectData object{bunny, color};
    object.set_displacement(0, -0.18, 0);
    object.set_scale(5);
    scene3->add(object);
  }

  BsplineCurve b4 {{
    {0,0,0},
    {1,0,0},
    {1,1,0},
    {0,1,0},
    {0,0,0},
  }};

  // scene->add(b4);
  // for (float t=0; t<=1; t+=0.1)
  //   std::cout << b4.evaluate(t).position << std::endl;


  // add objects
  // {
  //   Color color = {1,0,0};
  //   ObjectData object{curve1, color};
  //   object.set_scale(0.5);
  //   scene->add(object);
  // }
  // {
  //   Color color = {1,0,0};
  //   ObjectData object{curve2, color};
  //   object.set_scale(0.5);
  //   scene->add(object);
  // }
  // {
  //   Color color = {0,1,0};
  //   ObjectData object{surface1, color};
  //   object.set_scale(0.5);
  //   scene->add(object);
  // }
  

  
  // BezierSurfaceObject surface2(b3);
  // scene->add(surface2);


  // surface2.updater.face->set_visibility(false);
  // surface2.surface->


  // {
  //   Color color = {.9,.3,.9};
  //   ObjectData object{plane, color};
  //   object.set_displacement(0,10,0);
  //   object.set_rotation(90.0,0.0,0.0);
  //   object.set_scale(-10);
  //   scene->add(object);
  // }
  // {
  //   Color color = {1,1,1};
  //   ObjectData object{sphere, color};
  //   object.set_displacement(-1,0.5,0);
  //   object.set_scale(0.1);
  //   scene2->add(object);
  // }


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
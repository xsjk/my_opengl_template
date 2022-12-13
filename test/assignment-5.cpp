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
#include <cloth.h>
#include <world.h>
#include <group_wrapper.hpp>


int main() {

  constexpr unsigned width = 1920, height = 1080;


  // create a window
  Window window{width, height, "hw5"};

  // create a scene
  Handler<Scene> scene{0,0,width,height};
  window.add(scene);

  scene->camera.displace(vec3{0, -1.5, -6});
  scene->camera.set_yaw(180);
  scene->set_bg_color(0.1);

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

  // load meshes
  RenderData 
    bunny("assets/bunny.obj"), 
    plane("assets/plane.obj"),
    cube(MeshPrimitiveType::Cube),
    sphere(MeshPrimitiveType::Sphere),
    square(MeshPrimitiveType::Square);

  cube.apply({
    vec3{-3.5, -1.8, 0.3},
    quat{1,0,0,0},
    vec3{1, 1, 1}
  });

  sphere.apply({
    vec3{3.5, -1.8, 0.3},
    quat{1,0,0,0},
    vec3{1, 1, 1}
  });

  square.apply({
    vec3{0,-2.3,0},
    Rotation{0.0,PI/2,0.0},
    vec3{10, 10, 1}
  });
  

  // physics world
  {
    Handler<World> world;

    constexpr float cloth_weight = 5;
    constexpr uvec2 mass_dim = { 70, 70 };
    constexpr float dx = 0.03;
    constexpr float stiffness = 15;
    constexpr float damping_ratio = 0.001;
    std::vector<ivec2> fixed_masses { { 0, -1 }, { -1, -1 } };

    Handler<RectCloth> cloth{
      cloth_weight,
      mass_dim,
      dx,
      stiffness,
      damping_ratio,
    };

    cloth->apply(Affine{ vec3{0, 0, 0}, Rotation{0, PI/3, 0},});
    cloth->set_color(0, .75, 1);
    cloth->set_fixed_masses(fixed_masses);

    ObjectData cube_obj { cube }, sphere_obj { sphere }, bunny_obj { bunny }, square_obj { square };
    cube_obj.set_color(.75, 1, 0);
    sphere_obj.set_color(1, .75, 0);
    square_obj.set_color(.3, .3, .75);

    world->add(cloth);
    world->add(cube_obj);
    world->add(sphere_obj);
    // world->add(bunny_obj);
    world->add(square_obj);

    scene->add(world);
  }

  // add lights

  {// spot light 1
    Light l {
      .position = {-3, 0.2, -2, 1},
      .constant = 0,
      .linear = 0.01,
      .quadratic = 0.05,
      .direction = {0, -1, 5},
      .color = {1, .5, 0},
      .diffuse = vec3{.9},
    };
    scene->add(l);
  }
  
  {// point light 1
    Light l {
      .position = { 0, 3, -10, 1},
      .constant = 1.0,
      .linear = 0.0,
      .quadratic = 0.0,
      .color = vec3{1},
      .ambient = vec3{.1},
    };
    scene->add(l);
  }


  // start mainloop
  Window::mainloop();

  return 0;
}
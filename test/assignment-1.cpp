#include <utils.h>
#include <render_data.h>
#include <shader.h>
#include <camera.h>
#include <light.h>
#include <object_data.h>
#include <transform.h>
#include <scene.h>
#include <window.h>
#include <bezier.h>

int main() {

  // create a window
  Window window{1920, 1080, "Scene"};

  // create a scene
  Scene scene{0,0,1920,1080};

  // add a scene
  scene.display.mode = Display::FOLLOW_CAMERA;
  scene.display.camera.set_eye(0, .5, 2);
  window.add(scene);

  // add a scene
  scene.display.mode = Display::LIGHT_TRACKING;
  scene.display.set_width(1920/3);
  scene.display.set_height(1080/3);
  window.add(scene);

  // load meshes
  RenderData bunny("assets/bunny.obj"), plane("assets/plane.obj");

  // create shaders
  ObjectShader 
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
    };

  

  // add objects
  {
    Color color = {1.0f, 0.9f, 0.8f};
    ObjectData object{bunny, color};
    object.set_displacement(0, -0.18, 0);
    object.set_scale(5);
    scene.add(object, breathe_shader);
  }
  {
    Color color = {0.2f, 0.5f, 0.8f};
    ObjectData object{bunny, color};
    object.set_displacement(3, -0.18, 0);
    object.set_scale(5);
    scene.add(object, explosion_shader);
  }
  {
    Color color = {1.0f, 0.9f, 0.8f};
    ObjectData object{bunny, color};
    object.set_displacement(-3, -0.18, 0);
    object.set_scale(5);
    scene.add(object, fragmentize_shader);
  }
  {
    Color color = {.9,.3,.9};
    ObjectData o{plane, color};
    o.set_displacement(0,10,0);
    o.set_rotation(90.0,0.0,0.0);
    o.set_scale(-10);
    scene.add(o);
  }
  
  // add lights
  
  {// spot light 1
    Light l {
      .position = {-3, 0.2, 2, 1},
      .quadratic = 0.1,
      .direction = {0, -1, -5},
      .color = {.5, 1, 0},
      .diffuse = vec3{.9},
    };
    scene.add(l);
  }
  
  {// spot light 2
    Light l {
      .position = {0, 0.2, 2, 1},
      .quadratic = 0.1,
      .direction = {0, -1, -5},
      .color = {1, 0, 1},
      .diffuse = vec3{.9},
    };
    scene.add(l);
  }

  {// spot light 3
    Light l {
      .position = {3, 0.2, 2, 1},
      .quadratic = 0.1,
      .direction = {0, -1, -5},
      .color = {0, 1, 1},
      .diffuse = vec3{.9},
    };
    scene.add(l);
  }
  {// directional light
    Light l {
      .direction = {0,-1,2},
      .color = {0.5,0.2,0}
      
    };
    scene.add(l);
  }
  
  {// point light 1
    Light l {
      .position = {0, 1, 3, 1},
      .ambient = vec3{.03},
    };
    scene.add(l);
  }


  // start mainloop
  Window::mainloop();

  return 0;
}
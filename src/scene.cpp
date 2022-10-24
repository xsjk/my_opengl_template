#include <camera.h>
#include <light.h>
#include <object_data.h>
#include <shader.h>
#include <scene.h>
#include <window.h>
#include <object.h>
#include <bezier_object.h>

ObjectShader Scene::defaultShader;

LightShader Scene::lightShader;

void Scene::clear() {
  glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::render(PickingShader* pickingShader) {

  display.activate();

  clear();

  // draw objects (High level)
  for (auto& [name,object]: objects) {
    object->draw();
  }

  // draw lights (Low level)
  if (pickingShader) {
    ///@warning pickingTexture is garanteed to be activated
    pickingShader->use();
    pickingShader->set(display.camera);
    for (auto& [shader,objects]: data->renderData)
      for(auto& object:objects)
        pickingShader->draw(object);
  } else {
    // draw objects in renderData
    for (auto& [shader,objects]: data->renderData) {
      shader.use();
      shader.set(display.camera);
      // print(display.camera.matrix());  
      // std::cout << & display.camera << std::endl;
      int i = 0;
      for(auto& light:lights) 
        shader.set(light,i++);
      for(auto& object:objects)
        shader.draw(object);
    }
  }
  
  // draw lights
  lightShader.use();
  lightShader.set(display.camera);
  for(auto& light:lights)
    if(light.displaying || display.mode == Display::LIGHT_TRACKING)
      lightShader.draw(light);

  
}

void Scene::update() {

  switch (display.mode) {
    case Display::FOLLOW_CAMERA:
      if(lightCarried>=0)
        lights[lightCarried].follow(display.camera);
      break;
    case Display::LIGHT_TRACKING:
      display.camera.follow(lights[lightCurrent]);
      break;
  }

  // update objects (High level)
  for (auto& [name,object]: objects) {
    object->update();
  }
}


void Scene::add(const Light &light) {
  lights.push_back(light);
}

void Scene::add(const ObjectData& object) {
  add(object, defaultShader);
}

void Scene::add(const ObjectData &object, const ObjectShader& shader) {
  renderData[shader].push_back(object);
}

void Scene::add(const ObjectShader& objectShader) {
  renderData.insert({objectShader,std::vector<ObjectData>{}});
}

void Scene::add(const std::string& name, ObjectBase& object) {
  objects.insert({name,&object});
}

// void Scene::add(ObjectDataUpdater& o)  { 
//   add(ObjectData{o}); 
// }


// void Scene::add(BezierSurfaceObject& object) {
//   add(ObjectData{object.updater.point});
//   add(ObjectData{object.updater.grid});
//   add(ObjectData{object.updater.face});

// }


void Scene::graspLight() {
  lightCarried = lightCurrent;
  lights[lightCarried].displaying = false;
}

void Scene::dropLight() {
  if (lightCarried >= 0)
    lights[lightCarried].displaying = true;
  lightCarried = -1;
}

bool Scene::isLightCarried() const {
  return lightCarried >= 0;
}


Scene::Scene(GLint x, GLint y, GLsizei width, GLsizei height)
  : data{std::make_shared<SceneData>()},
    display{x, y, width, height}
{
  static bool first = true;
  if (first) {
    defaultShader.init("shader/object.vs", "shader/object.fs");
    lightShader.init("shader/light.vs","shader/light.fs","shader/light.gs");
  }
  first = false;
  add(defaultShader);
}

void __update__() {}


void Scene::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
  switch (display.mode) {
    case Display::FOLLOW_CAMERA:
      switch (action) {
        case GLFW_PRESS:
          switch (key) {
            case GLFW_KEY_F1: // grasp or drop light
              isLightCarried() ? dropLight() : graspLight();
              break;   
            case GLFW_KEY_TAB: // switch light
              lightCurrent = (lightCurrent + 1) % lights.size();
              break;
            case GLFW_KEY_F2: // switch mode
              break;

            case GLFW_KEY_F3: // print current camera pos
              std::cout << display.camera.get_eye() << std::endl;

          }
          break;
        case GLFW_PRESSING:
          display.camera.KeyboardPressingCallback(key, mode);
          switch (key) {
            case GLFW_KEY_EQUAL: // increase light cone
              if (lights[lightCurrent].outerCutOff < 89.0f) {
                lights[lightCurrent].outerCutOff += 1.0f;
                lights[lightCurrent].cutOff += 1.0f;
              }
              break;
            case GLFW_KEY_MINUS: // decrease light cone
              if (lights[lightCurrent].outerCutOff > 1.0f) {
                lights[lightCurrent].outerCutOff -= 1.0f;
                lights[lightCurrent].cutOff -= 1.0f;
              }
              break;
          } 
          break;
      }
    break;
    case Display::FIXED:
      break;

  }
}

void Scene::CursorMoveCallback(GLFWwindow* window, double dx, double dy) {
  switch(display.mode){
    case Display::FOLLOW_CAMERA:
      display.camera.CursorMoveCallback(dx, dy);
      break;
  }
}

void Scene::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  switch(display.mode){
    case Display::FOLLOW_CAMERA:
      display.camera.ScrollCallback(yoffset);
      break;
  }
  
}

SceneData& Scene::getData() {
  return *data;
}


bool Scene::operator==(const Scene &other) const {
  return data == other.data;
}


void Scene::__update__() {}
#include <camera.h>
#include <light.h>
#include <object_data.h>
#include <shader.h>
#include <scene.h>
#include <window.h>


ObjectShader Scene::defaultShader;

LightShader Scene::lightShader;

void Scene::clear() {
  glClearColor(bg_color.r, bg_color.g, bg_color.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::render() {

  display.activate();

  clear();

  // draw
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
  

  lightShader.use();
  lightShader.set(display.camera);
  for(auto& light:lights)
    if(light.displaying || display.mode == Display::LIGHT_TRACKING)
      lightShader.draw(light);

}

void Scene::update() {
  if (display.mode == Display::DEFAULT)
    if(lightCarried>=0)
      lights[lightCarried].follow(display.camera);
  if (display.mode == Display::LIGHT_TRACKING)
    display.camera.follow(lights[lightCurrent]);
  render();
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
  if (display.mode == Display::DEFAULT)
  switch (action) {
    case GLFW_PRESS:
      switch (key) {
        case GLFW_KEY_F1: // grasp or drop light
          isLightCarried() ? dropLight() : graspLight();
          break;   
        case GLFW_KEY_TAB: // switch light
          lightCurrent = (lightCurrent + 1) % lights.size();
          break;
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
  
}

void Scene::CursorMoveCallback(GLFWwindow* window, double dx, double dy) {
  if (display.mode == Display::DEFAULT)
  display.camera.CursorMoveCallback(dx, dy);
}

void Scene::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  if (display.mode == Display::DEFAULT)
  display.camera.ScrollCallback(yoffset);
}

SceneData& Scene::getData() {
  return *data;
}


bool Scene::operator==(const Scene &other) const {
  return data == other.data;
}
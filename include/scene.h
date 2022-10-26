#ifndef _SCENE_H_
#define _SCENE_H_

#include <defines.h>

#include <utils.h>  // WindowGuard

class Light;
class Group;
class ObjectData;
class ObjectBase;
class ObjectDataUpdater;

template<class S>
class SurfaceObject;

#include <shader.h>
#include <display.h>


struct SceneData {

  std::vector<Handler<Light>> lights;
  std::unordered_map<ObjectShader, std::vector<Handler<ObjectData>>, std::hash<GLuint>> renderData;
  std::unordered_map<std::string, ObjectBase*> objects;

  GLint lightCarried = -1;
  GLuint lightCurrent = 0;

};


#include <object_data.h>
#include <picking.h>

class Controller;

class Scene {
  Handler<SceneData> data;
  std::shared_ptr<Controller> controller;

  vec3 bg_color = {0.1f, 0.2f, 0.3f};
  
  float start_time = glfwGetTime();

  float t;

  void __update__();

public:
  Scene(GLint x, GLint y, GLsizei width, GLsizei height);

  Display display;
  Camera& camera = display.camera;
  
  GLint & lightCarried = data->lightCarried;
  GLuint & lightCurrent = data->lightCurrent;
  

  std::vector<Handler<Light>>& lights = data->lights;
  std::unordered_map<ObjectShader, std::vector<Handler<ObjectData>>, std::hash<GLuint>>& renderData = data->renderData;
  std::unordered_map<std::string, ObjectBase*> objects = data->objects;

  std::vector<Handler<Group>> groups;

  // std::vector<Handler<SurfaceObject<
  

  /// @brief clear scene with bg_color
  void clear();
  void clear(const vec3& color);

  /// @brief render the scene
  void render(PickingShader* p = nullptr);

  /// @brief update the scene
  void update();

  /// @brief add light to the scene
  /// @param Light: the light to be added
  void add(Handler<Light>);

  /// @brief add object to the scene
  /// @param ObjectData: the object to be added
  void add(Handler<ObjectData>);

  /// @brief add group of objects to the scene
  /// @param Group: the group of objects to be added
  void add(Handler<Group>);

  template<class G, class = std::enable_if_t<std::is_base_of_v<Group, G>>>
  void add(const G& g) { add(Handler<G>{g}); }

  // void add(ObjectDataUpdater&);
  // template<class T, typename U = std::enable_if_t<std::is_base_of_v<ObjectDataUpdater, T>>>
  // void Scene::add(T&& o)  { 
  //   add(ObjectData{ std::forward<T>(o) });
  // }

  /// @brief add object to the scene
  /// @param ObjectData: the object to be added 
  /// @param ObjectShader: the shader used for this object
  void add(Handler<ObjectData>, const ObjectShader&);

  /// @brief add object shader to the scene
  /// @param ObjectShader: the shader to be added
  void add(const ObjectShader&);

  /// @brief add object to the scene
  /// @param Object: the object to be added
  void add(const std::string& name, ObjectBase&);

  /// @brief carry a light with the camera
  /// @param Light: the light to be carried
  void carry(Light &);

  /// @brief carry a light with the camera
  void graspLight();

  /// @brief drop the carried light
  void dropLight();

  ///@brief whether a light is carried
  ///@return flag
  bool isLightCarried() const;
  
  
  void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
  void CursorMoveCallback(GLFWwindow* window, double xpos, double ypos);
  void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  // void 

  SceneData& getData();

  bool operator==(const Scene &other) const;

  SETTER_GETTER(bg_color, vec3)

  


  static ObjectShader defaultShader;
  static LightShader lightShader;

};




#endif
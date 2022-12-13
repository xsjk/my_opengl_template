#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <defines.h>
#include <scene.h>
#include <shader.h>
#include <picking.h>

class RenderData;
class BufferData;
class Scene;
class Window {
  friend class Picking;
  friend class Scene;

  void update_cursor();

  void updateObjectID();

  glm::vec<2,unsigned> cursor_pos;

  unsigned cursor_mode = GLFW_CURSOR_DISABLED;
  inline void __update__() const {
    glfwSetInputMode(windowHandle, GLFW_CURSOR, cursor_mode);
  }

public:

  SETTER_GETTER(cursor_mode);

  const int width, height;
  
  // unsigned cursor_mode = GLFW_CURSOR_NORMAL;
  
  /**
   * @brief Construct a Window
   * 
   * @param width 
   * @param height 
   * @param title 
   */
  Window(const int width, const int height, const std::string &title);

  ~Window();

  /**
   * @brief Add a scene to the window
   * 
   * @param scene 
   */
  void add(Handler<Scene> scene);
  void add(Scene scene);

  /// @brief generate VAOs, VBOs and EBOs for all meshes in the window
  void setup();

  /// @brief generate VAOs, VBOs and EBOs for the mesh
  /// @param mesh: the mesh whose VAOs, VBOs and EBOs are to be generated
  void setup(RenderData&);
  void setup(std::vector<RenderData>&);

  /// @brief update the window (render all scenes and process events)
  void update();


  /// @brief check if the window is alive
  bool alive();

  /// @brief check if all windows are alive
  static bool all_alive();
  
  /// @brief start the main loop of all windows
  /// @param bool: whether to setup all windows before starting the main loop
  static void mainloop(bool setup = true);
  
  std::vector<Handler<Scene>> scenes;
  unsigned focused_scene_ID;

  /// @brief the PickingTexture for the window
  Picking picking;
  Scene* focused_scene = nullptr;
  static Window* focused_window;

  

private:

  GLFWwindow *windowHandle; 

  bool focused = false;

  /// @brief check the key pressed and call KeyboardPressingCallback of the scenes
  void checkKeyPressing();

  /// @brief the modifier key flag for SHIFT, CTRL, ALT, SUPER, CAPS_LOCK, NUM_LOCK
  int modifierKeys = 0;


  /// @brief set the window as the current context
  void makeCurrent();

  /// @brief bind all callbacks to the given window
  /// @param window: the window to bind callbacks
  static void bindCallbacks(GLFWwindow*);

  static struct Guard { 
    Guard(); ~Guard(); 
    static void error_callback(int, const char*);
    /**
     * @brief create a window handle
     * 
     * @param width
     * @param height 
     * @param title 
     * 
     * @return GLFWwindow*
     */
    static GLFWwindow * createWindowHandle(const int, const int, const std::string &);
  } guard;

  // use static so that it can be accessed by the static callback functions and then call the member
  static std::unordered_map<GLFWwindow*,Window*> windows;

  // since VAOs cannot be shared by different windows we need to store them in the window
  std::unordered_map<Handler<BufferData>, GLuint> VAOs;

  // since VBOs and EBOs can be shared by all windows we don't need to record them in the window

};



#endif
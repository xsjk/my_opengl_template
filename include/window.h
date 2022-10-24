#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <defines.h>
#include <scene.h>
#include <shader.h>
#include <picking.h>

class RenderDataHandler;
class RenderData;

class Window {

  void update_cursor_pos();

  void updateObjectID();

  glm::vec<2,unsigned> cursor_pos;

public:

  const int width, height;

  unsigned input_mode = GLFW_CURSOR_DISABLED;
  // unsigned input_mode = GLFW_CURSOR_NORMAL;
  
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
  void add(Scene &scene);

  /// @brief generate VAOs, VBOs and EBOs for all meshes in the window
  void setup();

  /// @brief generate VAOs, VBOs and EBOs for the mesh
  /// @param mesh: the mesh whose VAOs, VBOs and EBOs are to be generated
  void setup(RenderDataHandler&);
  void setup(std::vector<RenderDataHandler>&);

  /// @brief update the window (render all scenes and process events)
  void update();


  /// @brief check if the window is alive
  bool alive();

  /// @brief check if all windows are alive
  static bool all_alive();
  
  /// @brief start the main loop of all windows
  /// @param bool: whether to setup all windows before starting the main loop
  static void mainloop(bool setup = true);
  
  std::vector<Scene> scenes;

  /// @brief the PickingTexture for the window
  PickingTexture pickingTexture;
  PickingShader pickingShader;
  unsigned current_object_ID = 0;

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

  // use member since VAOs cannot be shared by different windows
  std::unordered_map<std::shared_ptr<RenderData>, GLuint> VAOs;

  // use static since VBOs and EBOs can be shared by all windows 
  static std::unordered_map<std::shared_ptr<RenderData>, std::pair<GLuint, GLuint>> BOs;

};



#endif
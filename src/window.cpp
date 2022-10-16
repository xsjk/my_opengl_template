#include <window.h>
#include <scene.h>
#include <render_data.h>
#include <object_data.h>
#include <light.h>

std::unordered_map<GLFWwindow*,Window*> Window::windows;
Window::Guard Window::guard;
std::unordered_map<std::shared_ptr<RenderData>, std::pair<GLuint, GLuint>> Window::BOs;

Window::Window(const int width, const int height, const std::string &title) {
  windowHandle = guard.createWindowHandle(width, height, title);
  windows.emplace(windowHandle,this);
  bindCallbacks(windowHandle);
  glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEPTH_TEST); 
  glEnable(GL_SCISSOR_TEST); 
  glEnable(GL_BLEND);  
  glEnable(GL_PROGRAM_POINT_SIZE);  

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  


}

Window::~Window() {
  glfwDestroyWindow(windowHandle);
}

void Window::setup(RenderDataHandler& handle) {
  if(!handle) return;
  GLuint &VAO = handle.VAO, &VBO = handle.VBO, &EBO = handle.EBO;
  if (VAOs.find(handle.data) == VAOs.end()) {
    glGenVertexArrays(1, &VAO); 
    VAOs.emplace(handle.data, VAO); // record the VAO for this mesh in this window
  } else {
    VAO = VAOs.at(handle.data); // get the VAO for this mesh in this window
  }
  glBindVertexArray(VAO);
  if (BOs.find(handle.data) == BOs.end()) {
    handle.genBO();
    BOs.emplace(handle.data, std::make_pair(VBO, EBO)); // record the VBO and EBO for this mesh for all windows
  } else {
    auto [vbo, ebo] = BOs.at(handle.data);
    VBO = vbo,  EBO = ebo;
  }
  handle.setup();
}

void Window::setup(std::vector<RenderDataHandler>& handles) {
  for(auto &handle : handles)
    setup(handle);
}

void Window::setup() {
  makeCurrent();
  for (auto &scene : scenes) {
    for (auto&[_, objects] : scene.renderData)
      for (auto &object : objects)
        setup(*object.data);
    for (auto &light : scene.lights)
      setup(light.display);
  }
}

void Window::update() {
  makeCurrent();
  checkKeyPressing();
  for(auto& scene: scenes)
    scene.update();
  glfwSwapBuffers(windowHandle);
  glfwPollEvents();
}

bool Window::all_alive() {
  for (auto &[window, windowPtr] : windows)
    if (!windowPtr->alive())
      return false;
  return true;
}

void Window::mainloop(bool setup) {
  if (setup)
    for (auto [_, window] : windows)
      window->setup();
  while(all_alive())
    for(auto& [_, window]: windows)
      window->update();
}

void Window::checkKeyPressing() {
  for(int k: {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_EQUAL, GLFW_KEY_MINUS})
    if(glfwGetKey(windowHandle, k) == GLFW_PRESS)
      for(auto& scene: scenes)
        scene.KeyboardCallback(windowHandle, k, glfwGetKeyScancode(k), GLFW_PRESSING, modifierKeys);
}

void Window::add(Scene& scene) {
  /// @todo generate VAOs, VBOs and EBOs is necessary
  scenes.push_back(scene);
}

void Window::makeCurrent() {
  glfwMakeContextCurrent(windowHandle);
}


bool Window::alive() {
  return !glfwWindowShouldClose(windowHandle);
}



void Window::Guard::error_callback(int error, const char* description) {
  std::cerr << "GLFW Error: " << description << std::endl;
}

Window::Guard::Guard() {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) LOG_ERR("Failed to init glfw");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}

Window::Guard::~Guard() { 
  glfwTerminate(); 
}

GLFWwindow* Window::Guard::createWindowHandle(const int width, const int height, const std::string &title) {
  static GLFWwindow* window = nullptr;
  bool first = window == nullptr;
  window = glfwCreateWindow(width, height, title.c_str(), nullptr, window);
  glfwMakeContextCurrent(window);
  if (!window) LOG_ERR("Failed to create GLFW window");
  if (first) {
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) LOG_ERR("Failed to initialize GLAD");
  }
  return window;
}


void Window::bindCallbacks(GLFWwindow* window) {
  glfwSetKeyCallback(window,[](GLFWwindow* w, int k, int s, int a, int m) {
    int& mod = Window::windows.at(w)->modifierKeys;
    switch (a) {
      case GLFW_PRESS:
        switch (k) {
          case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(w, true);
            break;
          case GLFW_KEY_LEFT_SHIFT:
          case GLFW_KEY_RIGHT_SHIFT:
            mod |= GLFW_MOD_SHIFT;
            break;
          case GLFW_KEY_LEFT_CONTROL:
          case GLFW_KEY_RIGHT_CONTROL:
            mod |= GLFW_MOD_CONTROL;
            break;
          case GLFW_KEY_LEFT_ALT:
          case GLFW_KEY_RIGHT_ALT:
            mod |= GLFW_MOD_ALT;
            break;
          case GLFW_KEY_LEFT_SUPER:
          case GLFW_KEY_RIGHT_SUPER:
            mod |= GLFW_MOD_SUPER;
            break;
          case GLFW_KEY_CAPS_LOCK:
            mod |= GLFW_MOD_CAPS_LOCK;
            break;
          case GLFW_KEY_NUM_LOCK:
            mod |= GLFW_MOD_NUM_LOCK;
            break;
        }
        break;
      case GLFW_RELEASE:
        switch (k) {
          case GLFW_KEY_LEFT_SHIFT:
          case GLFW_KEY_RIGHT_SHIFT:
            mod &= !GLFW_MOD_SHIFT;
            break;
          case GLFW_KEY_LEFT_CONTROL:
          case GLFW_KEY_RIGHT_CONTROL:
            mod &= !GLFW_MOD_CONTROL;
            break;
          case GLFW_KEY_LEFT_ALT:
          case GLFW_KEY_RIGHT_ALT:
            mod &= !GLFW_MOD_ALT;
            break;
          case GLFW_KEY_LEFT_SUPER:
          case GLFW_KEY_RIGHT_SUPER:
            mod &= !GLFW_MOD_SUPER;
            break;
          case GLFW_KEY_CAPS_LOCK:
            mod &= !GLFW_MOD_CAPS_LOCK;
            break;
          case GLFW_KEY_NUM_LOCK:
            mod &= !GLFW_MOD_NUM_LOCK;
            break;   
        }
    }
    for(auto& scene:Window::windows.at(w)->scenes) 
      scene.KeyboardCallback(w,k,s,a,m);
  });
  glfwSetCursorPosCallback(window,[](GLFWwindow* w, double x, double y) {
    static double x_ = x, y_ = y;
    if(Window::windows.at(w)->focused)     
      for(auto& scene:Window::windows.at(w)->scenes) 
        scene.CursorMoveCallback(w,x-x_,y-y_);
    x_ = x; y_ = y;
  });
  glfwSetScrollCallback(window,[](GLFWwindow* w, double x, double y) {
    for(auto& scene:Window::windows.at(w)->scenes) 
      scene.ScrollCallback(w,x,y);
  });
  glfwSetWindowFocusCallback(window,[](GLFWwindow* w, int f) {
    Window::windows.at(w)->focused = f;
  });
  // glfwSetFramebufferSizeCallback(window,[](GLFWwindow* w, int x, int y) {
  // });

}


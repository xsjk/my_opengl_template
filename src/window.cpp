#include <window.h>
#include <scene.h>
#include <render_data.h>
#include <object_data.h>
#include <light.h>
#include <picking.h>

std::unordered_map<GLFWwindow*,Window*> Window::windows;
Window::Guard Window::guard;
Window* Window::focused_window = nullptr;

Window::Window(const int width, const int height, const std::string &title)
  : width{width}, height{height}
{
  windowHandle = guard.createWindowHandle(width, height, title);
  windows.emplace(windowHandle,this);
  bindCallbacks(windowHandle);
  
  __update__();

  glEnable(GL_DEPTH_TEST); 
  glEnable(GL_SCISSOR_TEST); 
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_BLEND);  
  glEnable(GL_PROGRAM_POINT_SIZE);  

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

  
  picking.init(width, height);

  focused_window = this;
  focused = true;

  update_cursor();
}

Window::~Window() {
  glfwDestroyWindow(windowHandle);
}

void Window::setup(RenderData& handle) {
  if(!handle) return;
  GLuint &VAO = handle.VAO;
  if (VAOs.find(handle.data) == VAOs.end()) {
    glGenVertexArrays(1, &VAO); 
    VAOs.emplace(handle.data, VAO); // record the VAO for this mesh in this window
  } else {
    VAO = VAOs.at(handle.data); // get the VAO for this mesh in this window
  }
  handle.bindVAO();
  handle.setupBO();
  handle.setupVAO();
}

void Window::setup(std::vector<RenderData>& handles) {
  for(auto &handle : handles)
    setup(handle);
}

void Window::setup() {
  makeCurrent();
  for (auto &scene : scenes) {
    for (auto&[_, objects] : scene->renderData)
      for (auto &object : objects)
        setup(object->data);
    for (auto &light : scene->lights)
      setup(light->display);
  }
}

void Window::update() {
  makeCurrent();
  checkKeyPressing();
  update_cursor();
  updateObjectID();
  for(auto& scene: scenes) {
    scene->update();
    scene->render();
  }
  picking.update(scenes);
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
        scene->KeyboardCallback(windowHandle, k, glfwGetKeyScancode(k), GLFW_PRESSING, modifierKeys);
}

void Window::add(Handler<Scene> scene) {
  /// @todo generate VAOs, VBOs and EBOs is necessary
  scenes.emplace_back(scene);
}

void Window::add(Scene scene) {
  /// @todo generate VAOs, VBOs and EBOs is necessary
  scenes.emplace_back(std::move(scene));
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

void Window::update_cursor() {
  double x, y;
  switch (cursor_mode) {
    case GLFW_CURSOR_DISABLED:
      x = width/2;
      y = height/2;
      break;
    case GLFW_CURSOR_NORMAL:
    case GLFW_CURSOR_HIDDEN:
      glfwGetCursorPos(windowHandle, &x, &y);
      y = height - y;
      break;
  }
  cursor_pos = {round(x), round(y)};
}

void Window::updateObjectID() {
  picking.get_current(cursor_pos);
  
  if(picking.last_id != picking.current_id){
    try { 
      if(picking.current_obj)
          picking.current_obj->mouseover(picking.current_uv);
    } catch(std::out_of_range& e) { }
    try { 
      if(picking.last_obj)
        picking.last_obj->mouseout(picking_object_uv); 
    } catch(std::out_of_range& e) { }
  } 

  // ondrag
  if(picking.dragging_obj){
    picking.dragging_obj->ondrag(cursor_pos[0], cursor_pos[1], picking.dragging_uv );

  }
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
      scene->KeyboardCallback(w,k,s,a,m);
  });
  glfwSetCursorPosCallback(window,[](GLFWwindow* w, double x, double y) {
    auto &window = *Window::windows.at(w);

    // cursormove
    static double x_ = x, y_ = y;
    if(window.focused) {  
      Scene* focused_scene = nullptr;
      for(auto& scene:window.scenes) {
        scene->CursorMoveCallback(w,x-x_,y-y_);
        if (scene->display.contains(window.cursor_pos))
          focused_scene = scene.get();
      }
      window.focused_scene = focused_scene;
    }
    x_ = x; y_ = y;

    auto& pos = window.cursor_pos;

    // mouse move
    if (window.picking.current_obj) {
      /// @todo remake

      window.picking.current_obj->mousemove(
        window.cursor_pos[0], 
        window.cursor_pos[1],
        window.picking.current_uv
      );
    }

    pos = {unsigned(x), unsigned(window.height - y)};
    // pos = {round(x), round(y)};

  });
  glfwSetMouseButtonCallback(window,[](GLFWwindow* w, int b, int a, int m) {
    Window& window = *Window::windows.at(w);
    window.picking.get_current(window.cursor_pos);
    switch(b) {
      case GLFW_MOUSE_BUTTON_LEFT:
        switch (a) {
          case GLFW_PRESS:
            window.picking.MouseButtonCallback(w,b,a,m);
            if (window.picking.current_obj)
              window.picking.current_obj->onclick(b,m,window.picking.current_uv);
            break;
          case GLFW_RELEASE:
            if (window.picking.current_obj)
              window.picking.current_obj->onrelease(b,m,window.picking.current_uv);
            window.picking.MouseButtonCallback(w,b,a,m);
            break;
        }
        break;
    }
    
  });
  glfwSetScrollCallback(window,[](GLFWwindow* w, double x, double y) {
    for(auto& scene:Window::windows.at(w)->scenes) 
      scene->ScrollCallback(w,x,y);
  });
  glfwSetWindowFocusCallback(window,[](GLFWwindow* w, int f) {
    Window::focused_window = Window::windows.at(w);
    Window::focused_window->focused = f;
  });
  // glfwSetFramebufferSizeCallback(window,[](GLFWwindow* w, int x, int y) {
  // });

}


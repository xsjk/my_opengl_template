#ifndef _DEFINES_H_
#define _DEFINES_H_
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cassert>
#include <vector>
#include <list>
#include <memory>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <optional>
#include <type_traits>


using glm::cross;
using glm::dot;
using glm::inverse;
using glm::mat3;
using glm::mat4;
using glm::pi;
using glm::radians;
using glm::rotate;
using glm::scale;
using glm::translate;
using glm::transpose;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using Color = vec3;

#define GLFW_PRESSING 3

const float PI = pi<float>();

#define LOG_INFO(msg)                             \
  std::cerr << "[INFO] "                          \
            << "function(" << __FUNCTION__ << ")" \
            << " line " << __LINE__ << ": " << (msg) << std::endl

#define LOG_ERR(msg)                                                 \
  do {                                                               \
    std::cerr << "[ERROR] "                                          \
              << "function(" << __FUNCTION__ << ")"                  \
              << " line " << __LINE__ << ": " << (msg) << std::endl; \
    std::exit(1);                                                    \
  } while (0)

#define UNIMPLEMENTED                            \
  do {                                           \
    std::cerr << "not implemented" << std::endl; \
    assert(false);                               \
  } while (0)

#define SETTER(name, type)                  \
  template<typename ...T>                   \
  void set_##name(T&&... args){             \
    name = type(std::forward<T>(args)...);  \
    __update__();\
  }
#define INCREASER(name)         \
  template<typename T>          \
  void increase_##name(T arg){  \
    name += arg;                \
    __update__();                   \
  }
#define GETTER(name, type) \
  type get_##name() const { return name; }

#define SETTER_GETTER(name, type) \
  SETTER(name, type)              \
  GETTER(name, type)

#define MAT_GETTER \
  mat4 matrix() const { return mat; } \
  operator mat4() const { return mat; }

#endif // _DEFINES_H_
cmake_minimum_required(VERSION 3.16)
project(OpenGL LANGUAGES C CXX)

option(USE_FILESYSTEM "use std::filesystem" off)



# glfw
option(GLFW_BUILD_TESTS off)
option(GLFW_BUILD_EXAMPLES off)
option(GLFW_BUILD_DOCS off)
add_subdirectory(libs/glfw)
include_directories(libs/glfw/include)
list(APPEND LIBRARIES glfw ${glfw_LIBRARIES})

# glad
set(GLAD_INCLUDE libs/glad/include)
include_directories(${GLAD_INCLUDE})

# glm
add_subdirectory(libs/glm)
list(APPEND LIBRARIES glm::glm)

# add filesystem
if (USE_FILESYSTEM)
  list(APPEND DEFINES -DUSE_FILESYSTEM)
  if (NOT MSVC)
    list(APPEND LIBRARIES stdc++fs)
  endif()
endif()

# glad
add_library(GLAD src/glad.c)
list(APPEND LIBRARIES GLAD)


# set source files and add executable
file(GLOB SRC_FILES src/*.cpp)
file(GLOB UTIL_FILES utils/*.hpp)


# # delaunator
# add_library(Utils ${UTIL_FILES})
# set_target_properties(Utils PROPERTIES LINKER_LANGUAGE CXX)
# list(APPEND LIBRARIES Utils)
# include_directories(utils)


# add library
add_library(Capsule STATIC ${SRC_FILES})
target_include_directories(Capsule PUBLIC include)
target_include_directories(Capsule PUBLIC libs)
target_include_directories(Capsule PUBLIC utils)
target_compile_definitions(Capsule PUBLIC ${DEFINES})
target_link_libraries(Capsule PUBLIC ${LIBRARIES})
target_compile_features(Capsule PUBLIC cxx_std_17)

# add executable
add_executable(assignment-1 test/assignment-1.cpp)
target_link_libraries(assignment-1 PRIVATE Capsule)
target_include_directories(assignment-1 PUBLIC include)

add_executable(assignment-2 test/assignment-2.cpp)
target_link_libraries(assignment-2 PRIVATE Capsule)
target_include_directories(assignment-2 PUBLIC include)



add_executable(test test/test.cpp)
target_link_libraries(test PRIVATE Capsule)


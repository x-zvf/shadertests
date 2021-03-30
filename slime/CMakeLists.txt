cmake_minimum_required(VERSION 3.11)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -ggdb3)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -ggdb3)
project(SHADERTEST)


find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
add_subdirectory(glad/)

file(GLOB SHADERS
  "*.glsl"
)

file(COPY ${SHADERS} DESTINATION ${CMAKE_BINARY_DIR})


add_executable(slime slime.cpp)
set(LLIBS ${GLFW_LIBRARIES} OpenGL::GL glad m)

message("LINKLIBS::::> '${LLIBS}' ")
target_link_libraries(slime ${LLIBS})

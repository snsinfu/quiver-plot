cmake_minimum_required(VERSION 3.8)

project(quiver CXX)

set(CMAKE_CXX_STANDARD 20)

set(PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}"     CACHE PATH "project root")
set(BLEND2D_DIR "${PROJECT_DIR}/vendor/blend2d" CACHE PATH "blend2d")

set(BLEND2D_STATIC TRUE)
include("${BLEND2D_DIR}/CMakeLists.txt")

add_executable(quiver src/main.cpp)
target_link_libraries(quiver Blend2D::Blend2D)

cmake_minimum_required(VERSION 3.8)

project(quiver CXX)

set(CMAKE_CXX_STANDARD 17)

set(PROJECT_DIR  "${CMAKE_CURRENT_LIST_DIR}"        CACHE PATH "project root")
set(BLEND2D_DIR  "${PROJECT_DIR}/vendor/blend2d"    CACHE PATH "blend2d")
set(JSONCONS_DIR "${PROJECT_DIR}/vendor/jsoncons"   CACHE PATH "jsoncons")
set(GETOPT_DIR   "${PROJECT_DIR}/vendor/cxx-getopt" CACHE PATH "getopt")

set(JSONCONS_INCLUDE_DIR "${JSONCONS_DIR}/include")
set(GETOPT_INCLUDE_DIR   "${GETOPT_DIR}")

set(BLEND2D_STATIC TRUE)
include("${BLEND2D_DIR}/CMakeLists.txt")

add_executable(
    quiver
    src/main.cpp
    src/spec.cpp
    src/plot.cpp
)
target_include_directories(quiver
    PRIVATE
    ${JSONCONS_INCLUDE_DIR}
    ${GETOPT_INCLUDE_DIR}
)
target_link_libraries(quiver Blend2D::Blend2D)

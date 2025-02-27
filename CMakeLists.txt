cmake_minimum_required(VERSION 3.20)
project(mesh-compiler CXX)

include_directories(ext/include)

add_executable(mesh-compiler)
add_subdirectory(src)

target_link_libraries(mesh-compiler assimp)
add_definitions(-DNDEBUG)
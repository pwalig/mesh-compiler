file(GLOB src_sources CONFIGURE_DEPENDS *.cpp)
target_sources(mesh-compiler PRIVATE ${src_sources})

add_subdirectory(assimp-integration)
add_subdirectory(core)
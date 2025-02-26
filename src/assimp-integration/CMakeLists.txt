file(GLOB assimp_sources CONFIGURE_DEPENDS *.cpp)
target_sources(mesh-compiler PRIVATE ${assimp_sources})
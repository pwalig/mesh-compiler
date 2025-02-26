file(GLOB types_sources CONFIGURE_DEPENDS *.cpp)
target_sources(mesh-compiler PRIVATE ${types_sources})
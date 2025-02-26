file(GLOB exceptions_sources CONFIGURE_DEPENDS *.cpp)
target_sources(mesh-compiler PRIVATE ${exceptions_sources})
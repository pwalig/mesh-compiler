file(GLOB fields_sources CONFIGURE_DEPENDS *.cpp)
target_sources(mesh-compiler PRIVATE ${fields_sources})
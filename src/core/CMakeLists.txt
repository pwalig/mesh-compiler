file(GLOB core_sources CONFIGURE_DEPENDS *.cpp)
target_sources(mesh-compiler PRIVATE ${core_sources})

add_subdirectory(exceptions)
add_subdirectory(fields)
add_subdirectory(types)
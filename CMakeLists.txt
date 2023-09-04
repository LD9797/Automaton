cmake_minimum_required(VERSION 3.10)
project(Automaton C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# Generate startResource.c using glib-compile-resources
add_custom_command(
        OUTPUT startResource.c
        COMMAND glib-compile-resources --target=${CMAKE_CURRENT_BINARY_DIR}/startResource.c --generate-source resource.xml
        DEPENDS resource.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating startResource.c from resource.xml"
)

add_executable(Automaton
        start.c
        ${CMAKE_CURRENT_BINARY_DIR}/startResource.c # Add generated resource file to executable
)

# Suppress deprecation warnings
target_compile_options(Automaton PRIVATE -Wno-deprecated-declarations -Wno-format-security)

# Math library and dynamic exports
target_link_libraries(Automaton ${GTK3_LIBRARIES} m -export-dynamic)

cmake_minimum_required(VERSION 3.26)
project(Automaton C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

include_directories(/usr/include/gtk-3.0)

file(GLOB SOURCES "Source/*.c" "Headers/*.h")
add_executable(Automaton
        ${SOURCES}
        )

target_link_libraries(Automaton ${GTK3_LIBRARIES} -lm -export-dynamic)
cmake_minimum_required(VERSION 3.26)
project(Automaton C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

include_directories(/usr/include/gtk-3.0)

add_executable(Automaton
        Source/output.c
        Headers/output.h
        Source/dfadriver.c
        Headers/dfadriver.h)

target_link_libraries(Automaton ${GTK3_LIBRARIES} -lm -export-dynamic)
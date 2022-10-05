cmake_minimum_required(VERSION 2.8)

project(countdown LANGUAGES C)

set (VERSION 1.0)
add_definitions(-DVERSION="${VERSION}")

include(CheckIncludeFiles)
check_include_files("getopt.h" HAVE_GETOPT_H)
if (HAVE_GETOPT_H)
    add_definitions(-DHAVE_GETOPT_H)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)
include_directories(${CURSES_INCLUDE_DIR})


aux_source_directory(${CMAKE_SOURCE_DIR}/src SRCS)  

add_executable(countdown main.c ${SRCS})
target_link_libraries(countdown ${CURSES_LIBRARIES})
message(${CURSES_LIBRARIES})

install(TARGETS countdown RUNTIME DESTINATION /usr/bin)

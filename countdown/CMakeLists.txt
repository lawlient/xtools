cmake_minimum_required(VERSION 2.8)

project(countdown LANGUAGES C)

set (VERSION 1.0)
add_definitions(-DVERSION="${VERSION}")

include(CheckIncludeFiles)
check_include_files("getopt.h" HAVE_GETOPT_H)
if (HAVE_GETOPT_H)
    add_definitions(-DHAVE_GETOPT_H)
endif()

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)
include_directories(${CURSES_INCLUDE_DIR})
add_definitions(-DHAVE_NCURSES_H)

add_executable(countdown countdown.c)
target_link_libraries(countdown ${CURSES_LIBRARIES})

install(TARGETS countdown RUNTIME DESTINATION /usr/bin)

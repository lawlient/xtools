cmake_minimum_required(VERSION 3.0)

project(diary)

add_definitions("-Wall -g")

set(biname diary)

include_directories(${CMAKE_CURRENT_SOURCE_DIR/src})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src srcs)

# curses library
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(${biname} ${srcs})
target_link_libraries(${biname} ${CURSES_LIBRARIES})

set(CMAKE_INSTALL_PREFIX "/usr/")
install(TARGETS ${biname} RUNTIME DESTINATION)




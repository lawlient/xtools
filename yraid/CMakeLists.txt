cmake_minimum_required(VERSION 3.0)

project(diary)

add_definitions("-Wall -g")

set(biname yraid)

include_directories(${CMAKE_CURRENT_SOURCE_DIR/src})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src srcs)

# curses library

set(CURSES_NEED_WIDE true) # enable unicode

SET(CURSES_LIBRARY_NAME "curses")
SET(NCURSES_LIBRARY_NAME "ncurses")

IF(CURSES_NEED_WIDE)
    SET(CURSES_LIBRARY_NAME "cursesw")
    SET(NCURSES_LIBRARY_NAME "ncursesw")
ENDIF(CURSES_NEED_WIDE)
find_package(Curses)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(${biname} ${srcs})
target_link_libraries(${biname} ${CURSES_LIBRARIES})

set(CMAKE_INSTALL_PREFIX "/usr/local/")
install(TARGETS ${biname} RUNTIME DESTINATION bin)




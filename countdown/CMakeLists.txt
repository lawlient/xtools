cmake_minimum_required(VERSION 3.11)

project(countdown LANGUAGES C)

set (VERSION 1.0)
add_definitions(-DVERSION="${VERSION}")

include(CheckIncludeFiles)
check_include_files("getopt.h" HAVE_GETOPT_H)
if (HAVE_GETOPT_H)
    add_definitions(-DHAVE_GETOPT_H)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/timeparser)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)
include_directories(${CURSES_INCLUDE_DIR})

aux_source_directory(${CMAKE_SOURCE_DIR}/src SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/timeparser SRCS)

find_package(BISON)
SET(BisonOutput ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
IF(BISON_FOUND)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${BisonOutput}
      COMMAND ${BISON_EXECUTABLE}
              --defines=${CMAKE_CURRENT_BINARY_DIR}/parser.h
              --output=${BisonOutput}
              ${CMAKE_SOURCE_DIR}/timeparser/parser.y
      COMMENT "Generating parser.c"
    )
ENDIF()

FIND_PACKAGE(FLEX REQUIRED)
SET(FlexOutput ${CMAKE_CURRENT_BINARY_DIR}/scanner.c)
IF(FLEX_FOUND)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${FlexOutput}
      COMMAND ${FLEX_EXECUTABLE}
              --outfile=${FlexOutput}
              ${CMAKE_SOURCE_DIR}/timeparser/time.l
      COMMENT "Generating scanner.c"
    )
ENDIF()


ADD_LIBRARY(parser ${BisonOutput} ${FlexOutput})
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

add_executable(countdown main.c ${SRCS})
target_link_libraries(countdown parser ${CURSES_LIBRARIES})
message(${CURSES_LIBRARIES})

install(TARGETS countdown RUNTIME DESTINATION /usr/bin)

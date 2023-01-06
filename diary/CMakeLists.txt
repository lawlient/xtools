cmake_minimum_required(VERSION 3.0)

project(diary)

add_definitions("-Wall -g")

set(biname diary)

include_directories(${CMAKE_CURRENT_SOURCE_DIR/src})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src srcs)

add_executable(${biname} ${srcs})

set(CMAKE_INSTALL_PREFIX "/usr/")
install(TARGETS ${biname} RUNTIME DESTINATION)




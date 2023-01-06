cmake_minimum_required(VERSION 3.0)

project(diary)

set(biname diary)

include_directories(${CMAKE_CURRENT_SOURCE_DIR/src})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src srcs)

add_executable(${biname} ${srcs})

set(CMAKE_INSTALL_PREFIX "/usr/bin/")
install(TARGETS ${biname} RUNTIME DESTINATION output/)




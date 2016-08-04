# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(gftest)

# list(APPEND CMAKE_CXX_FLAGS "-std=c++11")
list(APPEND CMAKE_CXX_FLAGS "-std=c++11  -g -O0 -ggdb ")

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} )
find_package( ROOT 5.34 REQUIRED )
include_directories( ${ROOT_INCLUDE_DIRS} )

find_package(Boost REQUIRED program_options)
include_directories(${Boost_INCLUDE_DIRS})

if(NOT DEFINED ENV{GENFIT_INSTALL})
  error( "GENFIT_INSTALL not defined" )
endif()
link_directories( $ENV{GENFIT_INSTALL}/lib )
include_directories( $ENV{GENFIT_INSTALL}/include )

find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

include_directories( ${PROJECT_SOURCE_DIR}/TestEm18/include )
file(GLOB SRCS ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/TestEm18/src/*.cc)

add_executable(gftest gftest.cxx ${SRCS} )
target_link_libraries(gftest ${Geant4_LIBRARIES} ${ROOT_LIBRARIES}  ${Boost_LIBRARIES} genfit2 Geom EG rt ) # rt not needed here if not liking with ProcessWrapper 

add_executable(gfscan gfscan.cxx ${SRCS} )
target_link_libraries(gfscan ${Geant4_LIBRARIES} ${ROOT_LIBRARIES}  ${Boost_LIBRARIES} genfit2 Geom EG rt )



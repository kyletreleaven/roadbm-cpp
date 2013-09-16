cmake_minimum_required(VERSION 2.8)

# defines the project name
project (raodbm-cpp)
set( ${CMAKE_PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${CMAKE_PROJECT_NAME}_VERSION_MINOR 1 )
set( ${CMAKE_PROJECT_NAME}_VERSION_BUGFIX 0 )

# adds the project-specific cmake module directory cmake/Modules to the cmake
# search path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# finds pkg-config
find_package(PkgConfig REQUIRED)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
add_subdirectory( test )

# configure the doxygen configuration
# configure_file( 
#    "${PROJECT_SOURCE_DIR}/doxy.config.in"
#    "${PROJECT_BINARY_DIR}/doxy.config" 
#    )

# use Jan Woetzel's doxygen doc target
# include("TargetDoc" OPTIONAL)

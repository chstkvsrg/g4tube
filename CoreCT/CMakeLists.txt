# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(corect)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


# gengetopt
INCLUDE(cmake/FindGengetopt.cmake)

WRAP_GGO(corect_GGO_C corect.ggo)
#ADD_EXECUTABLE(corect ${corect_GGO_C})

find_package (SQLite3 REQUIRED)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all HINTS "/home/xeon/Geant4MT/Installation/lib")
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${SQLITE3_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/targets/CorePhantom)
include_directories(${PROJECT_SOURCE_DIR}/include/targets/CoreVoxel)
include_directories(${PROJECT_SOURCE_DIR}/include/distributed)
include_directories(${PROJECT_SOURCE_DIR}/include/distributed/channels)


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/targets/CorePhantom/*.cc ${PROJECT_SOURCE_DIR}/src/targets/CoreVoxel/*.cc ${PROJECT_SOURCE_DIR}/src/distributed/*.cc ${PROJECT_SOURCE_DIR}/src/distributed/channels/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${PROJECT_SOURCE_DIR}/include/targets/CorePhantom/*.hh ${PROJECT_SOURCE_DIR}/include/targets/CoreVoxel/*.hh ${PROJECT_SOURCE_DIR}/include/distributed/*.hh ${PROJECT_SOURCE_DIR}/include/distributed/channels/*.hh)


#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(corect  corect.cc ${sources} ${corect_GGO_C} ${headers})
target_link_libraries(corect ${Geant4_LIBRARIES})
target_link_libraries (corect ${SQLITE3_LIBRARIES})
#message("${Geant4_LIBRARIES}")

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(corect_SCRIPTS
  init.mac
  run.mac
  runct.mac
  visGL.mac
  visQt.mac
  )

foreach(_script ${corect_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()


# Create output directory

add_custom_target( make-output-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/OUTPUT" # Create output directory
)

#----------------------------------------------------------------------------
# Create input directory
add_custom_target( make-input-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/INPUT")
	
configure_file(${PROJECT_SOURCE_DIR}/INPUT/Voxels.ml2g4  ${PROJECT_BINARY_DIR}/INPUT/Voxels.ml2g4 COPYONLY)

configure_file(${PROJECT_SOURCE_DIR}/INPUT/Densities.ml2g4  ${PROJECT_BINARY_DIR}/INPUT/Densities.ml2g4 COPYONLY)

configure_file(${PROJECT_SOURCE_DIR}/INPUT/Indexes.ml2g4 ${PROJECT_BINARY_DIR}/INPUT/Indexes.ml2g4 COPYONLY)

configure_file(${PROJECT_SOURCE_DIR}/INPUT/Spectrum.txt ${PROJECT_BINARY_DIR}/INPUT/Spectrum.txt COPYONLY)


#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(B1 DEPENDS exampleB1)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS corect DESTINATION bin)



cmake_minimum_required(VERSION 2.8)

#Project declaration
project(stylisation)

set(STYLISATION_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(STYLISATION_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH ${STYLISATION_ROOT}/bin/${CMAKE_BUILD_TYPE})

if("${STYLISATION_ROOT}" STREQUAL "${STYLISATION_CONFIG_DIR}")
  message(FATAL_ERROR
          "You need to run cmake from another directory: "
          "Build within the source tree is not supported\n"
          "Hint: Run 'rm -rf CMakeCache.txt CMakeFiles'\n"
          "Re-run CMake from another directory (e.g build/)")
endif()

#use openCV lib
find_package(
  OpenCV
  REQUIRED
)


link_directories(${OpenCV_LIBRARY_DIRS})

include_directories(
  ${STYLISATION_ROOT}/include,
  ${OpenCV_INCLUDE_DIRS}
)

#maybe for each artistical effect create specific var to have specific
file(
  GLOB_RECURSE
  #all path will be in ${SOURCE} var
  SOURCE

  ${STYLISATION_ROOT}/src/*

  ${STYLISATION_ROOT}/include/*
)

add_executable(
  stylisation
  ${SOURCE}
)

target_link_libraries(
  stylisation
  ${OPENCV_LIBRARIES}
)

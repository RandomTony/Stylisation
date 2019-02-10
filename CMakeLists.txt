cmake_minimum_required(VERSION 2.8)

#Project declaration
project(Stylisation)

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

# set Release build by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# add GCC optimisation
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#use openCV lib
find_package(
  OpenCV
  REQUIRED
)

#use OPENMP for parallelisation
find_package(
	OpenMP
)

if (OPENMP_FOUND)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
	message(STATUS
					"No OpenMP library found, compiling without parallelism support")
endif()

link_directories(${OpenCV_LIBRARY_DIRS})

include_directories(
	${OpenCV_INCLUDE_DIRS}
  ${STYLISATION_ROOT}/include
  # ${STYLISATION_ROOT}/lib
)

#maybe for each artistical effect create specific var to have specific
file(
  GLOB_RECURSE
  #all path will be in ${SOURCE} var
  SOURCE

	${STYLISATION_ROOT}/include/*.h
  ${STYLISATION_ROOT}/src/*.cpp
  # ${STYLISATION_ROOT}/lib/*.h
)

add_executable(
  stylisation
	test.cpp
  ${SOURCE}
)

target_link_libraries(
  stylisation
  ${OpenCV_LIBS}
)

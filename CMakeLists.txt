cmake_minimum_required(VERSION 3.11)

option(USE_OPENMP "Enable multi-threading" OFF)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

#set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/.. )

project (dynamic VERSION 0.1.0)

include(FetchContent)
include(CTest)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# hopscotch_map
ExternalProject_Add(hopscotch_map
  GIT_REPOSITORY "https://github.com/Tessil/hopscotch-map.git"
  BUILD_IN_SOURCE TRUE
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} # TODO ADD static build flag
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND ""
  CONFIGURE_COMMAND "")
ExternalProject_Get_property(hopscotch_map INSTALL_DIR)
set(hopscotch_map_INCLUDE "${INSTALL_DIR}/src/hopscotch_map/include/")

include_directories(${PROJECT_SOURCE_DIR}/include)
#include_directories(${PROJECT_SOURCE_DIR}/include/dynamic)
#include_directories(${PROJECT_SOURCE_DIR}/include/dynamic/internal)
#include_directories(${PROJECT_SOURCE_DIR}/include/dynamic/algorithms)
#include_directories(${PROJECT_SOURCE_DIR}/include/dynamic/algorithms/cw-bwt)
include_directories(${INSTALL_DIR}/src/hopscotch_map/include)

message("Building in ${CMAKE_BUILD_TYPE} mode")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.1
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate( googletest)
  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

enable_testing()

add_subdirectory ("tests")

if(XXSDS_DYN_MULTI_THREADED)
  find_package(OpenMP REQUIRED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif(XXSDS_DYN_MULTI_THREADED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ggdb -Ofast -fstrict-aliasing -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g -ggdb -Ofast -fstrict-aliasing -march=native")

add_executable(debug debug.cpp)
add_executable(deb deb.cpp)
add_executable(rle_lz77_v1 rle_lz77_v1.cpp)
add_executable(rle_lz77_v2 rle_lz77_v2.cpp)
add_executable(h0_lz77 h0_lz77.cpp)
add_executable(rle_bwt rle_bwt.cpp)
add_executable(cw-bwt cw-bwt.cpp)

add_executable(benchmark benchmark.cpp)
add_executable(add_bench add_bench.cpp)

add_executable(wm_string wm_string.cpp)

add_dependencies(debug hopscotch_map)
add_dependencies(rle_lz77_v1 hopscotch_map)
add_dependencies(rle_lz77_v2 hopscotch_map)
add_dependencies(h0_lz77 hopscotch_map)
add_dependencies(rle_bwt hopscotch_map)
add_dependencies(cw-bwt hopscotch_map)
add_dependencies(benchmark hopscotch_map)
add_dependencies(wm_string hopscotch_map)


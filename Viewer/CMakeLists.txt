
cmake_minimum_required(VERSION 2.8.2)
project(gvsViewer)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/..)
#message(${CMAKE_SOURCE_DIR})

set(GSL_DIR      $ENV{GSL_INSTALL_PATH}  CACHE FILEPATH "GSL_DIR")
set(GSL_LIB_DIR  $ENV{GSL_LIB_PATH}      CACHE FILEPATH "GSL_LIB_DIR")
#set(M4D_DIR      $ENV{HOME}/Projekte/libMotion4D CACHE FILEPATH "Motion4D_LIB")
set(M4D_DIR      ../../libMotion4D CACHE FILEPATH "Motion4D_LIB")

set(TIFF_AVAILABLE OFF CACHE BOOL "have tiff library available")
set(TIFF_INC_DIR  $ENV{TIFF_PATH}         CACHE FILEPATH "TIFF_INC_DIR")
set(TIFF_LIB_DIR  $ENV{TIFF_LIB_PATH}     CACHE FILEPATH "TIFF_LIB_DIR")

set(PNG_AVAILABLE OFF CACHE BOOL "have png library available")
set(PNG_INC_DIR  $ENV{PNG_PATH}         CACHE FILEPATH "PNG_INC_DIR")
set(PNG_LIB_DIR  $ENV{PNG_LIB_PATH}     CACHE FILEPATH "PNG_LIB_DIR")


set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath ${GSL_LIB_DIR}")

set(GVS_DIR  ${CMAKE_SOURCE_DIR}/..)
set(GL3W_DIR gl3w)
set(GLM_DIR  glm)
set(TBAR_DIR AntTweakBar)

include_directories(${GVS_DIR} ${GSL_DIR}/include)
include_directories(${M4D_DIR}/src ${GL3W_DIR} ${GLM_DIR} ${TBAR_DIR}/include)

link_directories(${CMAKE_SOURCE_DIR}/../lib)
link_directories(${CMAKE_SOURCE_DIR}/${TBAR_DIR}/lib ${GSL_LIB_DIR})

add_definitions(-DUSE_INTERFACE -DSTANDALONE=0 -DUSE_MATH -DHAVE_LIBPPM)
if (TIFF_AVAILABLE)
    include_directories(${TIFF_INC_DIR}/include)
	add_definitions( -DHAVE_LIBTIFF )
endif()
if (PNG_AVAILABLE)
    include_directories(${PNG_INC_DIR}/include)
	add_definitions( -DHAVE_LIBPNG )
endif()


set(BITS 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(BITS 64)
endif()

set(DAR "")
if(CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "DEBUG")
    set(DAR "d")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -DDEBUG -D_DEBUG -ggdb")
    #add_definitions(-pedantic)
endif()

add_executable(gvsViewer${BITS}${DAR} main.cpp GLShader.cpp Utils.cpp ${GL3W_DIR}/gl3w.c)
target_link_libraries(gvsViewer${BITS}${DAR} gvs${BITS}${DAR} glut GL dl AntTweakBar X11 gsl gslcblas)
if (TIFF_AVAILABLE)
    target_link_libraries(gvsViewer${BITS}${DAR} tiff)
endif()
if (PNG_AVAILABLE)
    target_link_libraries(gvsViewer${BITS}${DAR} png)
endif()


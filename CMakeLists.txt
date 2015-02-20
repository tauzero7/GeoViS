
cmake_minimum_required(VERSION 2.8.1)
project(GeoViSM4D)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR})

# set standard paths to the Motion4D source folder, the 
# GnuScientificLibrary folder, the MPI folder, and the 
# tiff image library folder
#set(M4D_ROOT_DIR  $ENV{HOME}/Projekte/libMotion4D  CACHE FILEPATH "M4D_ROOT_DIR")
set(M4D_ROOT_DIR  ../libMotion4D  CACHE FILEPATH "M4D_ROOT_DIR")
set(GSL_DIR       $ENV{GSL_INSTALL_PATH}  CACHE FILEPATH "GNU Scientific Library root path")
set(GSL_LIB_DIR   $ENV{GSL_LIB_PATH}      CACHE FILEPATH "GNU Scientific Library library path")

set(MPI_AVAILABLE OFF CACHE BOOL "have mpi library available")
set(MPI_DIR       $ENV{MPI_HOME}          CACHE FILEPATH "MPI root path")
set(MPI_LIB_DIR   $ENV{MPI_HOME}/lib64    CACHE FILEPATH "MPI library path")

set(TIFF_AVAILABLE OFF CACHE BOOL "have tiff library available")
set(TIFF_INC_DIR  $ENV{TIFF_PATH}         CACHE FILEPATH "TIFF include directory")
set(TIFF_LIB_DIR  $ENV{TIFF_LIB_PATH}     CACHE FILEPATH "TIFF library directory")

set(PNG_AVAILABLE OFF CACHE BOOL "have png library available")
set(PNG_INC_DIR  $ENV{PNG_PATH}         CACHE FILEPATH "PNG include directory")
set(PNG_LIB_DIR  $ENV{PNG_LIB_PATH}     CACHE FILEPATH "PNG library directory")


# The Motion4D src folder has four sub-folders
set(M4D_EXTRA_DIR  ${M4D_ROOT_DIR}/src/extra)
set(M4D_MATH_DIR   ${M4D_ROOT_DIR}/src/math)
set(M4D_METRIC_DIR ${M4D_ROOT_DIR}/src/metric)
set(M4D_MOTION_DIR ${M4D_ROOT_DIR}/src/motion)


# The GeoViS classes are separated by their type.
set(GVS_CAM_DIR    Cam)
set(GVS_DEV_DIR    Dev)
set(GVS_IMG_DIR    Img)
set(GVS_LIGHT_DIR  Light)
set(GVS_OBJ_DIR    Obj)
set(GVS_PARSER_DIR Parser)
set(GVS_RAY_DIR    Ray)
set(GVS_SHADER_DIR Shader)
set(GVS_TEX_DIR    Texture)
set(GVS_UTILS_DIR  Utils)
set(GVS_MPI_DIR    MpiUtils)

# add include folders
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${M4D_ROOT_DIR})
include_directories(${M4D_ROOT_DIR}/src)
include_directories(${GSL_DIR}/include)
include_directories(${GVS_CAM_DIR} ${GVS_DEV_DIR})
include_directories(${MPI_DIR}/include)

# Gather source files of the Motion4D library
file( GLOB m4d_source_files ${M4D_EXTRA_DIR}/*.cpp ${M4D_MATH_DIR}/*.cpp ${M4D_METRIC_DIR}/*.cpp ${M4D_MOTION_DIR}/*.cpp )
list( REMOVE_ITEM m4d_source_files ${CMAKE_SOURCE_DIR}/${M4D_METRIC_DIR}/m4dMetricAlcubierreAccel.cpp )

# Gather source files of GeoViS
file( GLOB gvs_source_files ${GVS_CAM_DIR}/*.cpp ${GVS_DEV_DIR}/*.cpp
                            ${GVS_IMG_DIR}/*.cpp ${GVS_LIGHT_DIR}/*.cpp 
                            ${GVS_OBJ_DIR}/*.cpp  
                            ${GVS_OBJ_DIR}/Comp/*.cpp  
                            ${GVS_OBJ_DIR}/MeshObj/*.cpp
                            ${GVS_OBJ_DIR}/PlanarObj/*.cpp  
                            ${GVS_OBJ_DIR}/SolidObj/*.cpp  
                            ${GVS_OBJ_DIR}/STMotion/*.cpp 
                            ${GVS_PARSER_DIR}/*.cpp ${GVS_PARSER_DIR}/*.c 
                            ${GVS_RAY_DIR}/*.cpp ${GVS_SHADER_DIR}/*.cpp
                            ${GVS_SHADER_DIR}/Surface/*.cpp
                            ${GVS_TEX_DIR}/*.cpp ${GVS_UTILS_DIR}/*.cpp)

# currently the following items are not in use
list( REMOVE_ITEM gvs_source_files ${CMAKE_SOURCE_DIR}/${GVS_TEX_DIR}/GvsChequeredTimeT2D.cpp )
list( REMOVE_ITEM gvs_source_files ${CMAKE_SOURCE_DIR}/${GVS_OBJ_DIR}/STMotion/GvsStMotionWorldline.cpp )

#message( STATUS ${gvs_source_files})

# These definitions are inevitable for tinyscheme!
add_definitions(-DUSE_INTERFACE -DSTANDALONE=0 -DUSE_MATH -DHAVE_LIBPPM)

# Depending on the image formats available
add_definitions( -DHAVE_PPM )
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
if(CMAKE_BUILD_TYPE MATCHES "debug" OR CMAKE_BUILD_TYPE MATCHES "DEBUG")
    set(DAR "d")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -DDEBUG -D_DEBUG -ggdb -DGVS_VERBOSE")
    add_definitions(-pedantic)
endif()


# ------------------------------
# build GeoViS as shared library
# ------------------------------
add_library(gvs${BITS}${DAR} SHARED ${m4d_source_files} ${gvs_source_files})


# ------------------------------
# build gvsRender
# ------------------------------
link_directories(${GSL_LIB_DIR} ${TIFF_LIB_DIR})
add_executable(gvsRender${BITS}${DAR} geovis.cpp)
target_link_libraries(gvsRender${BITS}${DAR} gvs${BITS}${DAR}  gsl gslcblas dl)
if (TIFF_AVAILABLE)
    target_link_libraries(gvsRender${BITS}${DAR} tiff)
endif()
if (PNG_AVAILABLE)
    target_link_libraries(gvsRender${BITS}${DAR} png)
endif()


# ------------------------------
# build gvsRenderPar
# ------------------------------
if (MPI_AVAILABLE)
    file( GLOB mpi_source_files ${GVS_MPI_DIR}/*.cpp)
    include_directories(${MPI_DIR}/include)
    link_directories(${MPI_LIB_DIR})
    add_executable(gvsRenderPar${BITS}${DAR} geovispar.cpp ${mpi_source_files})
    target_link_libraries(gvsRenderPar${BITS}${DAR} gvs${BITS}${DAR}  gsl gslcblas dl mpi_cxx mpi)
    if (TIFF_AVAILABLE)
        target_link_libraries(gvsRenderPar${BITS}${DAR} tiff)
    endif()
    if (PNG_AVAILABLE)
        target_link_libraries(gvsRenderPar${BITS}${DAR} png)
    endif()
endif()


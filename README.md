
# GeoViS - General relativistic ray tracing in 4D spacetimes

Current contact:  
Dr. Thomas Müller   
Haus der Astronomie/Max Planck Institute for Astronomy  
69117 Heidelberg, Germany  
Email: tmueller [at] mpia.de  

-----------------------------------------------

## Contents:

* Description
* Requirements
* Installation with cmake
* Run example


## Description

GeoViS is a code for general relativistic ray tracing in 
four-dimensional Lorentzian spacetimes. 


## Requirements

* GeoViS is based on the Motion4D library

* As the Motion4D library has some integrators
  taken from the Gnu Scientific Library (GSL),
  you also need GSL:
  http://www.gnu.org/software/gsl/

* Standard output of images is in the ppm-Format.
  If you have tiff or png available, you can also
  compile GeoViS with the corresponding libraries.


## Install GeoViS with cmake

1. Generate subdirs:    
   
        mkdir -p build/Debug 
        mkdir -p build/Release

2. Enter e.g. build/Release folder

3. ccmake ../..                      <= Please note the extra c in ccmake !!!

      Press 'c' to configure.

      Adjust the build variables:

         CMAKE_BUILD_TYPE   Release
         GSL_DIR            /path/to/gsl
         GSL_LIB_DIR        /path/to/gsl/lib
         M4D_ROOT_DIR       /absolute/path/to/m4d/
         MPI_AVAILABLE      ON or OFF
         MPI_DIR            /path/to/mpi
         MPI_LIB_DIR        /path/to/mpi/lib
    
     If you have png library available
    
         PNG_AVAILABLE      ON
         PNG_INC_DIR       /path/to/png/include
         PNG_LIB_DIR       /path/to/png/lib
    
     If you have tiff library available

         TIFF_AVAILABLE     ON
         TIFF_INC_DIR       /path/to/tiff/include
         TIFF_LIB_DIR       /path/to/tiff/lib

     If you have tiff and/or png in the standard paths,
     you do not have to set the INC and LIB paths.

     Press 'c' and 'g'.

4. make


## Run example

To run an example, open a command line and switch into the 
GeoViS folder. Then, write 

        ./gvsRender[d] examples/sphereAroundBlackhole.scm sphere.ppm

If you have MPI available and a multi-CPU machine, you can also 
use the parallel renderer. E.g. with 8 CPU:

        mpirun -np 8 ./gvsRenderPar -tasks 24 -renderdev 0 examples/sphereAroundBlackhole.scm sphere.ppm

The parallel rendering adds an image number to each output image,
e.g.:  sphere_0.ppm



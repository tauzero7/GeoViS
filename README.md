<!-- =========================================================
  Author:  Thomas Mueller
           Visualization Research Center (VISUS)
           University of Stuttgart
           thomas.mueller@vis.uni-stuttgart.de

  Version: 1.0
============================================================== -->

# GeoViS - General relativistic ray tracing in 4D spacetimes

Author: Thomas Müller  
Copyright: 2014  

> Visualization Research Center (VISUS)  
> University of Stuttgart  
> Allmandring 19  
> 70569 Stuttgart  

-----------------------------------------------

## Contents:

* Description
* Requirements
* Installation with cmake
* Installation with Qt
* Run example
* Post-processing with gvsViewer


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

* For post-processing with gvsViewer, you need a
  graphics board that supports OpenGL 3.3 or higher.


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


If you have an older cmake, you might change the 
`cmake_minimum_required` to your version, but it 
is possible that `CMAKE_SIZEOF_VOID_P` does give
a wrong result.



## Install GeoViS with QT (4.8.4 - gcc)

1. Open GeoViSM4D.pro  or  GeoViSM4Dpar.pro in QtCreator

2. Configure project for debug and release (only first time)

      Open Projects-Tab -> Build Settings:  deselect 'Shadow build'
      for 'Debug' and 'Release' build.

3. Adjust the following paths: `GSL_DIR`, `M4D_DIR`.

      If you do not have tiff or png library, remove the corresponding
      USE_... in the CONFIG lines.

4. Press Ctrl+B or go to 'Build -> Build project "GeoViS...pro"'

Please note that GeoViS should be started from command line. There are
no command line arguments within QtCreator.




## Run example

To run an example, open a command line and switch into the 
GeoViS folder. Then, write 

        ./gvsRender32 examples/sphereAroundBlackhole.scm sphere.ppm

Maybe you have to replace '32' by '64'. If you only have debug
version compiled, you also have to add a 'd' at the end of the
program name, e.g.:  `./gvsRender64d`

If you have MPI available and a multi-CPU machine, you can also 
use the parallel renderer. E.g. with 8 CPU:

        mpirun -np 8 ./gvsRenderPar32 -tasks 24 -renderdev 0 examples/sphereAroundBlackhole.scm sphere.ppm

The parallel rendering adds an image number to each output image,
e.g.:  sphere_0.ppm


## Post-processing with gvsViewer

### Installation:
    
Before you can compile gvsViewer, you have to compile the
Anttweakbar which is a small GUI API:
     
1. Move to Viewer/AntTweakbar/src folder

2. make
  
3. cd ../..

    
gvsViewer has to be compiled with 'cmake':

1. Generate subdirs within Viewer folder:
  
         mkdir -p build/Debug 
         mkdir -p build/Release

2. Enter build/Debug or build/Release folder

3. ccmake ../..
  
       Press 'c' to configure.
      
       Adjust the build variables:

          CMAKE_BUILD_TYPE   Debug  (or Release)
          GSL_DIR            /path/to/gsl
          GSL_LIB_DIR        /path/to/gsl/lib
          M4D_DIR            /absolute/path/to/m4d/src
        
       If you have png library available
 
          PNG_AVAILABLE      ON
          PNG_INC_DIR       /path/to/png/include
          PNG_LIB_DIR       /path/to/png/lib
    
       If you have tiff library available
 
          TIFF_AVAILABLE     ON
          TIFF_INC_DIR       /path/to/tiff/include
          TIFF_LIB_DIR       /path/to/tiff/lib

       Press 'c' and 'g'.

    
4. make


## Usage:
  
You can use gvsViewer to watch your rgb images. But what is more
is that you can also watch the pdz-data files.

If you have rendered the above image 'sphere.ppm', then you can 
watch it with gvsViewer via: (maybe you have to replace '32' by '64')
  
     ./gvsViewer32 -f .
     
     Select 'sphere.ppm" in  -> Load -> Files.

  To study also the pdz-data, go to the scene description file
  "examples/sphereAroundBlackhole.scm" and change the camera filter
  from "FilterRGB" to "FilterRGBpdz" and render the scene again.
  
  If you now select 'sphere.pdz' in ->Load->Files, you also have to 
  select which part of the data shall be shown. In ->Value->Channel
  you can select, e.g., the coordinate 'x0'. Which represents the
  time coordinate 't' in Schwarzschild spacetime. If you move the 
  mouse, the time of intersection is shown in ->Value->DValue.
  By modifying "pos min x0" and "pos max x0" within ->MinMax->Position,
  you can change the color mapping: 
    min value = black, max value = white

  Take care that you have selected the 'standard.frag' fragment
  shader within ->Load->Shader.  


----------------------------------------------------

The following list shows the directory structure of GeoViS:

    Cam                  |  Observer cameras  
    Dev                  |  Device, project, sample manager  
    examples                  
    Img                  |  Image handling  
    Light                |  Light sources and light source manager  
    MpiUtils             |  Utilities for parallel rendering using MPI  
    Obj                  
     + Comp              |  compound object classes: local and global  
     + PlanarObj         |  planar objects: triangle, ring  
     + SolidObj          |  solid objects: box, ellipsoid, CSG objects  
     + STMotion          |  spacetime motion: geodesic, const, local tetrad  
    Parser               |  parser for scene description languange   
    Ray                  |  light ray classes  
    Shader                 
     + Surface           |  surface shading  
    Texture              |  texture handling  
    Utils                |  diverse utility methods  
    Viewer               |  post-processing software: gvsViewer  
     + AntTweakBar       |     graphical user interface (GUI)  
     + gl3w              |     opengl extensions  
     + glm               |     mathematical library  
     + Shader            |     shader programs   



### list of files generated with 'tree'  

├── Cam  
│   ├── Gvs2PICam.cpp  
│   ├── Gvs2PICam.h  
│   ├── Gvs4PICam.cpp  
│   ├── Gvs4PICam.h  
│   ├── GvsCamera.cpp  
│   ├── GvsCamera.h  
│   ├── GvsPanoramaCam.cpp  
│   ├── GvsPanoramaCam.h  
│   ├── GvsPinHoleCam.cpp  
│   └── GvsPinHoleCam.h  
├── CMakeLists.txt  
├── Dev  
│   ├── GvsDevice.cpp  
│   ├── GvsDevice.h  
│   ├── GvsProjector.cpp   
│   ├── GvsProjector.h  
│   ├── GvsSampleMgr.cpp  
│   └── GvsSampleMgr.h  
├── doxyfile  
├── examples  
│   ├── fallEddFinkIn.scm  
│   ├── minkWheel.scm  
│   ├── morristhorne.scm  
│   ├── schwarzschild.scm  
│   ├── sphereAroundBlackhole.scm  
│   └── whBox.scm  
├── geovis.cpp  
├── GeoViSM4Dpar.pro  
├── GeoViSM4D.pri  
├── GeoViSM4D.pro  
├── geovispar.cpp  
├── GvsGlobalDefs.h  
├── Img  
│   ├── GvsChannelImg2D.cpp  
│   ├── GvsChannelImg2D.h  
│   ├── GvsColor.cpp  
│   ├── GvsColor.h  
│   ├── GvsPicIOEnvelope.cpp  
│   ├── GvsPicIOEnvelope.h  
│   ├── GvsPictureIO.cpp  
│   ├── GvsPictureIO.h  
│   ├── GvsPngIO.cpp  
│   ├── GvsPngIO.h  
│   ├── GvsPpmIO.cpp  
│   ├── GvsPpmIO.h  
│   ├── GvsTiffIO.cpp  
│   └── GvsTiffIO.h  
├── Light  
│   ├── GvsFlashLight.cpp  
│   ├── GvsFlashLight.h  
│   ├── GvsLightSrc.cpp  
│   ├── GvsLightSrc.h  
│   ├── GvsLightSrcMgr.cpp  
│   ├── GvsLightSrcMgr.h  
│   ├── GvsPointLight.cpp  
│   └── GvsPointLight.h  
├── MpiUtils  
│   ├── GvsMpiDefs.h  
│   ├── MpiImage.cpp  
│   ├── MpiImage.h  
│   ├── MpiTaskManager.cpp  
│   └── MpiTaskManager.h  
├── Obj  
│   ├── Comp  
│   │   ├── GvsCompoundObj.cpp  
│   │   ├── GvsCompoundObj.h  
│   │   ├── GvsLocalCompObj.cpp  
│   │   └── GvsLocalCompObj.h  
│   ├── GvsBase.cpp  
│   ├── GvsBase.h  
│   ├── GvsBoundBox4D.cpp  
│   ├── GvsBoundBox4D.h  
│   ├── GvsBoundBox.cpp  
│   ├── GvsBoundBox.h  
│   ├── GvsObjPtrList.cpp  
│   ├── GvsObjPtrList.h  
│   ├── GvsSceneObj.cpp  
│   ├── GvsSceneObj.h  
│   ├── GvsSurface.cpp  
│   ├── GvsSurface.h  
│   ├── GvsTransformObj.cpp  
│   ├── GvsTransformObj.h  
│   ├── PlanarObj  
│   │   ├── GvsPlanarSurf.cpp  
│   │   ├── GvsPlanarSurf.h  
│   │   ├── GvsRing.cpp  
│   │   ├── GvsRing.h  
│   │   ├── GvsTriangle.cpp  
│   │   └── GvsTriangle.h  
│   ├── SolidObj  
│   │   ├── GvsSolBackground.cpp  
│   │   ├── GvsSolBackground.h  
│   │   ├── GvsSolBox.cpp  
│   │   ├── GvsSolBox.h  
│   │   ├── GvsSolConvexPrim.cpp  
│   │   ├── GvsSolConvexPrim.h  
│   │   ├── GvsSolCylinder.cpp  
│   │   ├── GvsSolCylinder.h  
│   │   ├── GvsSolEllipsoid.cpp  
│   │   ├── GvsSolEllipsoid.h  
│   │   ├── GvsSolidCSGObj.cpp  
│   │   ├── GvsSolidCSGObj.h  
│   │   ├── GvsSolidDifferObj.cpp  
│   │   ├── GvsSolidDifferObj.h  
│   │   ├── GvsSolidIntersecObj.cpp  
│   │   ├── GvsSolidIntersecObj.h  
│   │   ├── GvsSolidObj.cpp  
│   │   ├── GvsSolidObj.h  
│   │   ├── GvsSolidUnifiedObj.cpp  
│   │   ├── GvsSolidUnifiedObj.h  
│   │   ├── GvsSolObjSpanList.cpp  
│   │   └── GvsSolObjSpanList.h  
│   └── STMotion  
│       ├── GvsLocalTetrad.cpp  
│       ├── GvsLocalTetrad.h  
│       ├── GvsStMotionConstVelocity.cpp  
│       ├── GvsStMotionConstVelocity.h  
│       ├── GvsStMotion.cpp  
│       ├── GvsStMotionGeodesic.cpp  
│       ├── GvsStMotionGeodesic.h  
│       ├── GvsStMotion.h  
│       ├── GvsStMotionWorldline.cpp  
│       └── GvsStMotionWorldline.h  
├── Parser  
│   ├── dynload.c  
│   ├── dynload.h  
│   ├── GvsParser.cpp  
│   ├── GvsParser.h  
│   ├── GvsParseScheme.cpp  
│   ├── GvsParseScheme.h  
│   ├── init.scm  
│   ├── opdefines.h  
│   ├── parse_camera.cpp  
│   ├── parse_camera.h  
│   ├── parse_compObj.cpp  
│   ├── parse_compObj.h  
│   ├── parse_csgObj.cpp  
│   ├── parse_csgObj.h   
│   ├── parse_device.cpp    
│   ├── parse_device.h    
│   ├── parse_helper.cpp  
│   ├── parse_helper.h  
│   ├── parse_light.cpp  
│   ├── parse_light.h  
│   ├── parse_locTed.cpp  
│   ├── parse_locTed.h   
│   ├── parse_metric.cpp  
│   ├── parse_metric.h  
│   ├── parse_motion.cpp  
│   ├── parse_motion.h  
│   ├── parse_planeObjects.cpp  
│   ├── parse_planeObjects.h  
│   ├── parse_projector.cpp  
│   ├── parse_projector.h  
│   ├── parse_raygen.cpp  
│   ├── parse_raygen.h  
│   ├── parse_shader.cpp  
│   ├── parse_shader.h  
│   ├── parse_solidObjects.cpp  
│   ├── parse_solidObjects.h  
│   ├── parse_solver.cpp  
│   ├── parse_solver.h  
│   ├── parse_texture.cpp  
│   ├── parse_texture.h  
│   ├── parse_transformation.cpp  
│   ├── parse_transformation.h  
│   ├── parse_transformObject.cpp  
│   ├── parse_transformObject.h  
│   ├── scheme.c  
│   ├── scheme.h  
│   └── scheme-private.h  
├── Ray  
│   ├── GvsRayAllIS.cpp  
│   ├── GvsRayAllIS.h  
│   ├── GvsRayAnyIS.cpp  
│   ├── GvsRayAnyIS.h  
│   ├── GvsRayClosestIS.cpp  
│   ├── GvsRayClosestIS.h  
│   ├── GvsRay.cpp  
│   ├── GvsRayGen.cpp  
│   ├── GvsRayGen.h  
│   ├── GvsRay.h  
│   ├── GvsRayOneIS.cpp  
│   ├── GvsRayOneIS.h  
│   ├── GvsRayVisual.cpp  
│   ├── GvsRayVisual.h  
│   ├── GvsSurfIntersec.cpp  
│   └── GvsSurfIntersec.h  
├── README.md  
├── Shader  
│   ├── GvsShader.cpp  
│   ├── GvsShader.h  
│   └── Surface  
│       ├── GvsSurfaceShader.cpp  
│       └── GvsSurfaceShader.h  
├── Texture  
│   ├── GvsCheckerT2D.cpp  
│   ├── GvsCheckerT2D.h  
│   ├── GvsImg2DSampler.cpp  
│   ├── GvsImg2DSampler.h  
│   ├── GvsTexture2D.cpp  
│   ├── GvsTexture2D.h  
│   ├── GvsTexture.cpp  
│   ├── GvsTexture.h  
│   ├── GvsUniTex.cpp  
│   └── GvsUniTex.h  
├── Utils  
│   ├── GvsGeodSolver.cpp  
│   ├── GvsGeodSolver.h  
│   ├── GvsGramSchmidt.cpp  
│   ├── GvsGramSchmidt.h  
│   ├── GvsLog.cpp  
│   └── GvsLog.h  
└── Viewer  
    ├── AntTweakBar  
    │   ├── ...  
    ├── CMakeLists.txt  
    ├── gl3w  
    │   ├── GL3  
    │   │   ├── gl3.h  
    │   │   └── gl3w.h  
    │   └── gl3w.c  
    ├── glm  
    │   ├── ...  
    ├── GLShader.cpp  
    ├── GLShader.h  
    ├── main.cpp  
    ├── Shader  
    │   ├── schwarzschildDisc.frag  
    │   ├── standard.frag  
    │   ├── standard.vert  
    │   └── texCoords.frag  
    ├── Utils.cpp  
    └── Utils.h  




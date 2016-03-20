
CAM_DIR  = $$TOP_DIR/Cam
DEV_DIR  = $$TOP_DIR/Dev
IMG_DIR  = $$TOP_DIR/Img
LGH_DIR  = $$TOP_DIR/Light
OBJ_DIR  = $$TOP_DIR/Obj
RAY_DIR  = $$TOP_DIR/Ray
UTL_DIR  = $$TOP_DIR/Utils
PRS_DIR  = $$TOP_DIR/Parser
SHD_DIR  = $$TOP_DIR/Shader
TEX_DIR  = $$TOP_DIR/Texture

TEMPLATE = app
CONFIG += console warn_off
CONFIG -= app_bundle
CONFIG -= qt

# necessary for tinyscheme
DEFINES  += USE_INTERFACE STANDALONE=0 USE_MATH

INCLUDEPATH += $$TOP_DIR $$GSL_DIR/include

QMAKE_CXXFLAGS     += -Wall #-Wno-switch
QMAKE_CFLAGS_DEBUG += -Wall #-Wno-switch
QMAKE_RPATHDIR     += $$GSL_DIR/lib

HEADERS += GvsGlobalDefs.h \
           $$CAM_DIR/GvsCamera.h \
           $$CAM_DIR/GvsPanoramaCam.h \
           $$CAM_DIR/GvsPinHoleCam.h \
           $$CAM_DIR/GvsPinHoleStereoCam.h \
           $$CAM_DIR/Gvs2PICam.h \
           $$CAM_DIR/Gvs4PICam.h \
           $$DEV_DIR/GvsDevice.h \
           $$DEV_DIR/GvsProjector.h \
           $$DEV_DIR/GvsSampleMgr.h \
           $$IMG_DIR/GvsColor.h \
           $$IMG_DIR/GvsChannelImg2D.h \
           $$IMG_DIR/GvsIntersecOutput.h \
           $$IMG_DIR/GvsPicIOEnvelope.h \
           $$IMG_DIR/GvsPictureIO.h \
           $$LGH_DIR/GvsLightSrc.h \
           $$LGH_DIR/GvsLightSrcMgr.h \
           $$LGH_DIR/GvsFlashLight.h \
           $$LGH_DIR/GvsPointLight.h \
           $$OBJ_DIR/GvsBase.h \
           $$OBJ_DIR/GvsBoundBox.h \
           $$OBJ_DIR/GvsBoundBox4D.h \
           $$OBJ_DIR/GvsObjPtrList.h \
           $$OBJ_DIR/GvsSceneObj.h \
           $$OBJ_DIR/GvsSurface.h \
           $$OBJ_DIR/GvsTransformObj.h \
           $$OBJ_DIR/Comp/GvsCompoundObj.h \
           $$OBJ_DIR/Comp/GvsLocalCompObj.h \
           $$OBJ_DIR/MeshObj/GvsOBJMesh.h \
           $$OBJ_DIR/PlanarObj/GvsPlanarRing.h \
           $$OBJ_DIR/PlanarObj/GvsPlanarSurf.h \
           $$OBJ_DIR/PlanarObj/GvsTriangle.h \
           $$OBJ_DIR/SolidObj/GvsSolBackground.h \
           $$OBJ_DIR/SolidObj/GvsSolBox.h \
           $$OBJ_DIR/SolidObj/GvsSolConvexPrim.h \
           $$OBJ_DIR/SolidObj/GvsSolCylinder.h \
           $$OBJ_DIR/SolidObj/GvsSolEllipsoid.h \
           $$OBJ_DIR/SolidObj/GvsSolRing.h \
           $$OBJ_DIR/SolidObj/GvsSolidObj.h \
           $$OBJ_DIR/SolidObj/GvsSolObjSpanList.h \
           $$OBJ_DIR/SolidObj/GvsSolidCSGObj.h \
           $$OBJ_DIR/SolidObj/GvsSolidDifferObj.h \
           $$OBJ_DIR/SolidObj/GvsSolidIntersecObj.h \
           $$OBJ_DIR/SolidObj/GvsSolidUnifiedObj.h \
           $$OBJ_DIR/STMotion/GvsLocalTetrad.h \
           $$OBJ_DIR/STMotion/GvsStMotion.h \
           $$OBJ_DIR/STMotion/GvsStMotionConstVelocity.h \
           $$OBJ_DIR/STMotion/GvsStMotionGeodesic.h \
           $$PRS_DIR/GvsParser.h \
           $$PRS_DIR/GvsParseScheme.h \
           $$RAY_DIR/GvsRay.h \
           $$RAY_DIR/GvsRayAllIS.h \
           $$RAY_DIR/GvsRayAnyIS.h \
           $$RAY_DIR/GvsRayClosestIS.h \
           $$RAY_DIR/GvsRayGen.h \
           $$RAY_DIR/GvsRayOneIS.h \
           $$RAY_DIR/GvsRayVisual.h \
           $$RAY_DIR/GvsSurfIntersec.h \
           $$SHD_DIR/GvsShader.h \
           $$SHD_DIR/Surface/GvsSurfaceShader.h \
           $$TEX_DIR/GvsCheckerT2D.h \
           $$TEX_DIR/GvsChequeredT2D.h \
           $$TEX_DIR/GvsColorGradTex.h \
           $$TEX_DIR/GvsDieT2D.h \
           $$TEX_DIR/GvsImg2DSampler.h \
           $$TEX_DIR/GvsTexture.h \
           $$TEX_DIR/GvsTexture2D.h \
           $$TEX_DIR/GvsUniTex.h \
           $$UTL_DIR/GvsGramSchmidt.h \
           $$UTL_DIR/GvsGeodSolver.h \
           $$UTL_DIR/GvsLog.h

SOURCES += $$PROJECT_MAIN \
           $$CAM_DIR/GvsCamera.cpp \
           $$CAM_DIR/GvsPanoramaCam.cpp \
           $$CAM_DIR/GvsPinHoleCam.cpp \
           $$CAM_DIR/GvsPinHoleStereoCam.cpp \
           $$CAM_DIR/Gvs2PICam.cpp \
           $$CAM_DIR/Gvs4PICam.cpp \
           $$DEV_DIR/GvsDevice.cpp \
           $$DEV_DIR/GvsProjector.cpp \
           $$DEV_DIR/GvsSampleMgr.cpp \
           $$IMG_DIR/GvsColor.cpp \
           $$IMG_DIR/GvsChannelImg2D.cpp \
           $$IMG_DIR/GvsIntersecOutput.cpp \
           $$IMG_DIR/GvsPicIOEnvelope.cpp \
           $$IMG_DIR/GvsPictureIO.cpp \
           $$LGH_DIR/GvsLightSrc.cpp \
           $$LGH_DIR/GvsLightSrcMgr.cpp \
           $$LGH_DIR/GvsFlashLight.cpp \
           $$LGH_DIR/GvsPointLight.cpp \
           $$OBJ_DIR/GvsBase.cpp \
           $$OBJ_DIR/GvsBoundBox.cpp \
           $$OBJ_DIR/GvsBoundBox4D.cpp \
           $$OBJ_DIR/GvsObjPtrList.cpp \
           $$OBJ_DIR/GvsSceneObj.cpp \
           $$OBJ_DIR/GvsSurface.cpp \
           $$OBJ_DIR/GvsTransformObj.cpp \
           $$OBJ_DIR/Comp/GvsCompoundObj.cpp \
           $$OBJ_DIR/Comp/GvsLocalCompObj.cpp \
           $$OBJ_DIR/MeshObj/GvsOBJMesh.cpp \
           $$OBJ_DIR/PlanarObj/GvsPlanarRing.cpp \
           $$OBJ_DIR/PlanarObj/GvsPlanarSurf.cpp \
           $$OBJ_DIR/PlanarObj/GvsTriangle.cpp \
           $$OBJ_DIR/SolidObj/GvsSolBackground.cpp \
           $$OBJ_DIR/SolidObj/GvsSolBox.cpp \
           $$OBJ_DIR/SolidObj/GvsSolConvexPrim.cpp \
           $$OBJ_DIR/SolidObj/GvsSolCylinder.cpp \
           $$OBJ_DIR/SolidObj/GvsSolEllipsoid.cpp \
           $$OBJ_DIR/SolidObj/GvsSolRing.cpp \
           $$OBJ_DIR/SolidObj/GvsSolidObj.cpp \
           $$OBJ_DIR/SolidObj/GvsSolObjSpanList.cpp \
           $$OBJ_DIR/SolidObj/GvsSolidCSGObj.cpp \
           $$OBJ_DIR/SolidObj/GvsSolidDifferObj.cpp \
           $$OBJ_DIR/SolidObj/GvsSolidIntersecObj.cpp \
           $$OBJ_DIR/SolidObj/GvsSolidUnifiedObj.cpp \
           $$OBJ_DIR/STMotion/GvsLocalTetrad.cpp \
           $$OBJ_DIR/STMotion/GvsStMotion.cpp \
           $$OBJ_DIR/STMotion/GvsStMotionConstVelocity.cpp \
           $$OBJ_DIR/STMotion/GvsStMotionGeodesic.cpp \
           $$PRS_DIR/GvsParser.cpp \
           $$PRS_DIR/GvsParseScheme.cpp \
           $$RAY_DIR/GvsRay.cpp \
           $$RAY_DIR/GvsRayAllIS.cpp \
           $$RAY_DIR/GvsRayAnyIS.cpp \
           $$RAY_DIR/GvsRayClosestIS.cpp \
           $$RAY_DIR/GvsRayGen.cpp \
           $$RAY_DIR/GvsRayOneIS.cpp \
           $$RAY_DIR/GvsRayVisual.cpp \
           $$RAY_DIR/GvsSurfIntersec.cpp \
           $$SHD_DIR/GvsShader.cpp \
           $$SHD_DIR/Surface/GvsSurfaceShader.cpp \
           $$TEX_DIR/GvsCheckerT2D.cpp \
           $$TEX_DIR/GvsChequeredT2D.cpp \
           $$TEX_DIR/GvsColorGradTex.cpp \
           $$TEX_DIR/GvsDieT2D.cpp \
           $$TEX_DIR/GvsImg2DSampler.cpp \
           $$TEX_DIR/GvsTexture.cpp \
           $$TEX_DIR/GvsTexture2D.cpp \
           $$TEX_DIR/GvsUniTex.cpp \
           $$UTL_DIR/GvsGramSchmidt.cpp \
           $$UTL_DIR/GvsGeodSolver.cpp \
           $$UTL_DIR/GvsLog.cpp

HEADERS += $$PRS_DIR/scheme.h \
           $$PRS_DIR/scheme-private.h \
           $$PRS_DIR/parse_camera.h \
           $$PRS_DIR/parse_compObj.h \
           $$PRS_DIR/parse_csgObj.h \
           $$PRS_DIR/parse_device.h \
           $$PRS_DIR/parse_helper.h \
           $$PRS_DIR/parse_light.h \
           $$PRS_DIR/parse_locTed.h \
           $$PRS_DIR/parse_metric.h \
           $$PRS_DIR/parse_motion.h \
           $$PRS_DIR/parse_meshObjects.h \
           $$PRS_DIR/parse_planeObjects.h \
           $$PRS_DIR/parse_projector.h \
           $$PRS_DIR/parse_raygen.h \
           $$PRS_DIR/parse_shader.h \
           $$PRS_DIR/parse_solidObjects.h \
           $$PRS_DIR/parse_solver.h \
           $$PRS_DIR/parse_texture.h \
           $$PRS_DIR/parse_transformation.h \
           $$PRS_DIR/parse_transformObject.h

SOURCES += $$PRS_DIR/scheme.c \
           $$PRS_DIR/parse_camera.cpp \
           $$PRS_DIR/parse_compObj.cpp \
           $$PRS_DIR/parse_csgObj.cpp \
           $$PRS_DIR/parse_device.cpp \
           $$PRS_DIR/parse_helper.cpp \
           $$PRS_DIR/parse_light.cpp \
           $$PRS_DIR/parse_locTed.cpp \
           $$PRS_DIR/parse_metric.cpp \
           $$PRS_DIR/parse_motion.cpp \
           $$PRS_DIR/parse_meshObjects.cpp \
           $$PRS_DIR/parse_planeObjects.cpp \
           $$PRS_DIR/parse_projector.cpp \
           $$PRS_DIR/parse_raygen.cpp \
           $$PRS_DIR/parse_shader.cpp \
           $$PRS_DIR/parse_solidObjects.cpp \
           $$PRS_DIR/parse_solver.cpp \
           $$PRS_DIR/parse_texture.cpp \
           $$PRS_DIR/parse_transformation.cpp \
           $$PRS_DIR/parse_transformObject.cpp


OTHER_FILES += $$PRS_DIR/init.scm

USE_M4D_SRC {
    include($$M4D_DIR/m4d_sources.pri)
    HEADERS += $$M4D_HEADERS
    SOURCES += $$M4D_SOURCES
    INCLUDEPATH += $$M4D_DIR $$M4D_SRC_DIR
}

USE_M4D_LIB {
   unix: LIBS += -L$$M4D_DIR/lib -lMotion4Dextra -lMotion4Dmath -lMotion4Dmetric -lMotion4Dmotion \
                 -Wl,-rpath $$M4D_DIR/lib
}


INCLUDEPATH += $$GSL_DIR/include
LIBS += -L$$GSL_LIB_DIR -lgsl -lgslcblas

USE_TIFF {
    HEADERS += $$IMG_DIR/GvsTiffIO.h
    SOURCES += $$IMG_DIR/GvsTiffIO.cpp
    DEFINES += HAVE_LIBTIFF
    LIBS    += -L$$TIFF_LIB_DIR -ltiff
    INCLUDEPATH += $$TIFF_DIR/include
}

USE_PPM {
    HEADERS += $$IMG_DIR/GvsPpmIO.h
    SOURCES += $$IMG_DIR/GvsPpmIO.cpp
    DEFINES += HAVE_LIBPPM
}

USE_PNG {
    HEADERS += $$IMG_DIR/GvsPngIO.h
    SOURCES += $$IMG_DIR/GvsPngIO.cpp
    DEFINES += HAVE_LIBPNG
    LIBS    += -L$$PNG_LIB_DIR -lpng
    INCLUDEPATH += $$PNG_DIR/include
}




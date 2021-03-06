/**
 * @file    GvsParser.h
 * @author  Thomas Mueller
 *
 *  This file is part of GeoViS.
 */
#ifndef GVS_PARSER_H
#define GVS_PARSER_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

#include <Parser/parse_camera.h>
#include <Parser/parse_compObj.h>
#include <Parser/parse_csgObj.h>
#include <Parser/parse_device.h>
#include <Parser/parse_helper.h>
#include <Parser/parse_light.h>
#include <Parser/parse_locTed.h>
#include <Parser/parse_meshObjects.h>
#include <Parser/parse_metric.h>
#include <Parser/parse_motion.h>
#include <Parser/parse_planeObjects.h>
#include <Parser/parse_projector.h>
#include <Parser/parse_raygen.h>
#include <Parser/parse_shader.h>
#include <Parser/parse_solidObjects.h>

#include <Parser/parse_solver.h>
#include <Parser/parse_texture.h>
#include <Parser/parse_transformObject.h>
#include <Parser/parse_transformation.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "Cam/GvsCamera.h"
#include "Dev/GvsDevice.h"
#include "Dev/GvsProjector.h"
#include "GvsGlobalDefs.h"
#include "Img/GvsChannelImg2D.h"
#include "Light/GvsLightSrc.h"
#include "Light/GvsLightSrcMgr.h"
#include "Obj/Comp/GvsLocalCompObj.h"
#include "Obj/GvsSceneObj.h"
#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Obj/STMotion/GvsStMotion.h"
#include "Obj/SolidObj/GvsSolidCSGObj.h"
#include "Obj/SolidObj/GvsSolidDifferObj.h"
#include "Obj/SolidObj/GvsSolidIntersecObj.h"
#include "Obj/SolidObj/GvsSolidUnifiedObj.h"
#include "Ray/GvsRayGen.h"
#include "Shader/GvsShader.h"
#include "Utils/GvsGeodSolver.h"

//#include <Texture/GvsTexture.h>

#include <metric/m4dMetricDatabase.h>

class GvsDevice;

class API_EXPORT GvsParser
{
public:
    GvsParser();
    virtual ~GvsParser();

    void deleteAll();
    void read_scene(const char* name);
    int getNumDevices() const;

    void initStandard(GvsDevice* device);
    void getDevice(GvsDevice* device, unsigned int k = 0);

    void printAll(FILE* fptr = stderr) const;

    m4d::Metric* getMetric(unsigned int k = 0);
    GvsGeodSolver* getSolver(unsigned int k = 0);
    GvsCamera* getCamera(unsigned int k = 0);
    GvsProjector* getProjector(unsigned int k = 0);
    GvsRayGen* getRayGen(unsigned int k = 0);
    GvsLightSrcMgr* getLightMgr(unsigned int k = 0);
    GvsLightSrc* getLight(unsigned int k = 0);
    GvsSceneObj* getSceneObj(unsigned int k = 0);
    GvsStMotion* getMotion(unsigned int k = 0);

protected:
    std::string getFilepath(std::string& filename);
    std::string getFullPathname();
};

#endif

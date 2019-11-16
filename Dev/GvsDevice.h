/**
 * @file    GvsDevice.h
 * @author  Thomas Mueller
 *
 * @brief  The device class stores all the objects of a scene.
 *        For each image of an image sequence, a device has to
 *        be initialized in .scm file.
 *
 *  This file is part of GeoViS.
 */
#ifndef GVS_DEVICE_H
#define GVS_DEVICE_H

#include <string>
#include <vector>

#include "Cam/GvsCamera.h"
#include "Dev/GvsProjector.h"
#include "GvsGlobalDefs.h"
#include "Light/GvsLightSrcMgr.h"
#include "Obj/GvsBase.h"

class GvsSceneObj;
class Metric;
class GvsLightSrcMgr;

typedef struct GvsChangeObj_t {
    GvsBase* objectPtr;
    std::string objectIDname;
    std::string paramName;
    GvsDataType type;
    void* val;
} GvsChangeObj;

class API_EXPORT GvsDevice : public GvsBase
{
public:
    GvsDevice();
    virtual ~GvsDevice();

    /**
     * @brief GvsDevice::setChangeObj
     * @param obj
     * @param idName
     * @param name
     * @param tp
     * @param value
     */
    void setChangeObj(GvsBase* obj, std::string idName, std::string name, GvsDataType tp, void* value);
    bool makeChange();
    void clearChangeObj();

    void clear();

    void setManual(bool manual);
    virtual void Print(FILE* fptr = stderr);

public:
    GvsCamera* camera;
    GvsProjector* projector;
    m4d::Metric* metric;
    GvsLightSrcMgr* lightSrcMgr;
    GvsSceneObj* sceneGraph;

    /// List of object parameters that change from one image to the other.
    std::vector<GvsChangeObj*> mChangeObj;

    bool isManual;
    GvsCamEye camEye;
};

#endif

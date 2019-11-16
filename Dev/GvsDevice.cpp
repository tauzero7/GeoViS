/**
 * @file    GvsDevice.cpp
 * @author  Thomas Mueller
 *
 *  This file is part of GeoViS.
 */
#include "GvsGlobalDefs.h"

#include "Dev/GvsDevice.h"
#include "Obj/GvsSceneObj.h"
#include "Parser/GvsParser.h"

#include "metric/m4dMetric.h"

#include "Utils/GvsLog.h"
extern GvsLog& LOG;

GvsDevice ::GvsDevice()
{
    camera = nullptr;
    projector = nullptr;
    metric = nullptr;
    lightSrcMgr = nullptr;
    sceneGraph = nullptr;

    mChangeObj.clear();
    isManual = false;
    camEye = gvsCamEyeStandard;
}

GvsDevice::~GvsDevice()
{
    clearChangeObj();
    // delete metric;
    // TODO: clear metric, camera,...
}

void GvsDevice::setChangeObj(GvsBase* obj, std::string idName, std::string name, GvsDataType tp, void* value)
{
    // std::cerr << "GvsDevice :: setChangeObj" << std::endl;
    GvsChangeObj* chObj = new GvsChangeObj;
    chObj->objectPtr = obj;
    chObj->objectIDname = idName;
    chObj->paramName = name;
    chObj->type = tp;
    chObj->val = value;

    mChangeObj.push_back(chObj);
}

bool GvsDevice::makeChange()
{
#ifdef GVS_VERBOSE
    std::cerr << "GvsDevice::makeChange...\n";
#endif
    bool changed = true;
    bool adjustTetrad = false;

    for (unsigned int i = 0; i < mChangeObj.size(); i++) {
        if (mChangeObj[i]->objectPtr == nullptr) {
            return false;
        }

        int setHint = gvsSetParamNone;
        switch (mChangeObj[i]->type) {
            case gvsDT_INT: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName, *static_cast<int*>(mChangeObj[i]->val));
                break;
            }
            case gvsDT_DOUBLE: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName, *static_cast<double*>(mChangeObj[i]->val));
                break;
            }
            case gvsDT_VEC2: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName, *(static_cast<m4d::vec2*>(mChangeObj[i]->val)));
                break;
            }
            case gvsDT_VEC3: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName, *(static_cast<m4d::vec3*>(mChangeObj[i]->val)));
                break;
            }
            case gvsDT_VEC4: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName, *(static_cast<m4d::vec4*>(mChangeObj[i]->val)));
                break;
            }
            case gvsDT_MAT2D: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName,
                                  *static_cast<m4d::Matrix<double, 2, 3>*>(mChangeObj[i]->val));
                break;
            }
            case gvsDT_MAT3D: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName,
                                  *static_cast<m4d::Matrix<double, 3, 4>*>(mChangeObj[i]->val));
                break;
            }
            case gvsDT_IVEC2: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName, *(static_cast<m4d::ivec2*>(mChangeObj[i]->val)));
                break;
            }
            case gvsDT_IVEC3: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName, *(static_cast<m4d::ivec3*>(mChangeObj[i]->val)));
                break;
            }
            case gvsDT_IVEC4: {
                setHint = (mChangeObj[i]->objectPtr)
                              ->SetParam(mChangeObj[i]->paramName, *(static_cast<m4d::ivec4*>(mChangeObj[i]->val)));
                break;
            }
            case gvsDT_STRING: {
                char* tt = (static_cast<char*>(mChangeObj[i]->val));
                std::string pv = std::string(tt);
                setHint = (mChangeObj[i]->objectPtr)->SetParam(mChangeObj[i]->paramName, pv);
            }
            default: {
                changed = false;
                break;
            }
        }

        adjustTetrad |= (setHint == gvsSetParamAdjustTetrad);
    }

    // THE FOLLOWING IS NOT ALLOWED WHEN SETTING TETRAD VECTORS MANUALLY !!!!
    // In that case, you have to call
    //   (setparam ("tedID" "calc" 0))
    // to manually adjust tetrad when a metric parameter has changed.

    // If the projector is described by a motion, then it is not allowed
    // to adjust the local tetrad !!
    if (!isManual) {
        if (projector != nullptr) {
            GvsStMotion* motion = projector->getMotion();
            GvsLocalTetrad* lt = projector->getLocalTetrad();
            if (motion == nullptr && lt != nullptr) {
                // lt->adjustTetrad(); // ???CHECK if allowed
            }
        }
    }

    if (adjustTetrad && projector != nullptr) {
        GvsLocalTetrad* lt = projector->getLocalTetrad();
        if (lt != nullptr) {
            lt->adjustTetrad();
        }
    }
    return changed;
}

void GvsDevice::clearChangeObj()
{
    if (!mChangeObj.empty()) {
        for (unsigned int i = 0; i < mChangeObj.size(); i++) {
            if (mChangeObj[i] != nullptr) {
                if (mChangeObj[i]->val != nullptr) {
                    if (mChangeObj[i]->type == gvsDT_INT) {
                        delete reinterpret_cast<int*>(mChangeObj[i]->val);
                    }
                    else if (mChangeObj[i]->type == gvsDT_DOUBLE) {
                        delete reinterpret_cast<double*>(mChangeObj[i]->val);
                    }
                    else if (mChangeObj[i]->type == gvsDT_VEC3) {
                        delete reinterpret_cast<m4d::vec3*>(mChangeObj[i]->val);
                    }
                    else if (mChangeObj[i]->type == gvsDT_VEC4) {
                        delete reinterpret_cast<m4d::vec4*>(mChangeObj[i]->val);
                    }
                    else if (mChangeObj[i]->type == gvsDT_MAT2D) {
                        delete reinterpret_cast<m4d::Matrix<double, 2, 3>*>(mChangeObj[i]->val);
                    }
                    else if (mChangeObj[i]->type == gvsDT_MAT3D) {
                        delete reinterpret_cast<m4d::Matrix<double, 3, 4>*>(mChangeObj[i]->val);
                    }
                    else if (mChangeObj[i]->type == gvsDT_STRING) {
                        delete reinterpret_cast<char*>(mChangeObj[i]->val);
                    }
                }
                mChangeObj[i]->objectPtr = nullptr;
                delete mChangeObj[i];
            }
            mChangeObj[i] = nullptr;
        }
    }
    mChangeObj.clear();
}

void GvsDevice ::clear(void)
{
    camera = nullptr;
    projector = nullptr;
    metric = nullptr;
    lightSrcMgr = nullptr;
    sceneGraph = nullptr;
}

void GvsDevice ::setManual(bool manual)
{
    isManual = manual;
}

void GvsDevice::Print(FILE* fptr)
{
    LOG.printf("Device consists of the following components:\n");
    LOG.printf("--------------------------------------------\n");
    if (camera != nullptr) {
        camera->Print(fptr);
    }
    if (projector != nullptr) {
        projector->Print(fptr);
    }
    if (metric != nullptr) {
        metric->printF(fptr);
    }
    if (lightSrcMgr != nullptr) {
        lightSrcMgr->Print(fptr);
    }
    if (sceneGraph != nullptr) {
        sceneGraph->Print(fptr);
    }

    if (camera->isStereoCam()) {
        if (this->camEye == gvsCamEyeLeft) {
            LOG.printf("\tStereoCam:  Left\n");
        }
        else if (this->camEye == gvsCamEyeRight) {
            LOG.printf("\tStereoCam:  Right\n");
        }
    }

    LOG.printf("\nChange:\n");
    for (unsigned int i = 0; i < mChangeObj.size(); i++) {
        LOG.printf("\tIDname    [%d]:  %s\n", i, mChangeObj[i]->objectIDname.c_str());
        LOG.printf("\tParamName [%d]:  %s\n", i, mChangeObj[i]->paramName.c_str());
        LOG.printf("\tParamType [%d]:  %s\n", i, GvsDataTypeName[mChangeObj[i]->type].c_str());
        LOG.printf("\tParamValue[%d]:  ", i);
        switch (mChangeObj[i]->type) {
            case gvsDT_VOID:
            case gvsDT_UNKNOWN: {
                break;
            }
            case gvsDT_INT: {
                LOG.printf("%d\n", (*static_cast<int*>(mChangeObj[i]->val)));
                break;
            }
            case gvsDT_FLOAT: {
                LOG.printf("%f\n", static_cast<double>((*static_cast<float*>(mChangeObj[i]->val))));
                break;
            }
            case gvsDT_DOUBLE: {
                LOG.printf("%f\n", (*static_cast<double*>(mChangeObj[i]->val)));
                break;
            }
            case gvsDT_VEC2: {
                m4d::vec2* v = reinterpret_cast<m4d::vec2*>(mChangeObj[i]->val);
                // TODO: move output to logfile
                v->printS(fptr);
                break;
            }
            case gvsDT_VEC3: {
                m4d::vec3* v = reinterpret_cast<m4d::vec3*>(mChangeObj[i]->val);
                v->printS(fptr);
                break;
            }
            case gvsDT_VEC4: {
                m4d::vec4* v = reinterpret_cast<m4d::vec4*>(mChangeObj[i]->val);
                v->printS(fptr);
                break;
            }
            case gvsDT_MAT2D: {
                m4d::Matrix<double, 2, 3>* mat = reinterpret_cast<m4d::Matrix<double, 2, 3>*>(mChangeObj[i]->val);
                mat->printS(fptr);
                break;
            }
            case gvsDT_MAT3D: {
                m4d::Matrix<double, 3, 4>* mat = reinterpret_cast<m4d::Matrix<double, 3, 4>*>(mChangeObj[i]->val);
                mat->printS(fptr);
                break;
            }
            case gvsDT_IVEC2: {
                m4d::ivec2* v = static_cast<m4d::ivec2*>(mChangeObj[i]->val);
                v->printS(fptr);
                break;
            }
            case gvsDT_IVEC3: {
                m4d::ivec3* v = static_cast<m4d::ivec3*>(mChangeObj[i]->val);
                v->printS(fptr);
                break;
            }
            case gvsDT_IVEC4: {
                m4d::ivec4* v = static_cast<m4d::ivec4*>(mChangeObj[i]->val);
                v->printS(fptr);
                break;
            }
            case gvsDT_STRING: {
                char* v = static_cast<char*>(mChangeObj[i]->val);
                fprintf(fptr, "%s", v);
                break;
            }
        }
    }
    LOG.printf("End of Change.\n");
}

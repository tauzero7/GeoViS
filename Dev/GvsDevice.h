// ---------------------------------------------------------------------
//  Copyright (c) 2013-2014, Universitaet Stuttgart, VISUS, Thomas Mueller
//
//  This file is part of GeoViS.
//
//  GeoViS is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  GeoViS is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with GeoViS.  If not, see <http://www.gnu.org/licenses/>.
// ---------------------------------------------------------------------
#ifndef GVS_DEVICE_H
#define GVS_DEVICE_H

#include <string>
#include <vector>

#include "GvsGlobalDefs.h"
#include "Cam/GvsCamera.h"
#include "Dev/GvsProjector.h"
#include "Light/GvsLightSrcMgr.h"
#include "Obj/GvsBase.h"


class GvsSceneObj;
class Metric;
class GvsLightSrcMgr;


typedef struct GvsChangeObj_t
{
    GvsBase*     objectPtr;
    std::string  objectIDname;
    std::string  paramName;
    GvsDataType  type;
    void*        val;
} GvsChangeObj;

/**
 * @brief The device class stores all the objects of a scene.
 *        For each image of an image sequence, a device has to
 *        be initialized in .scm file.
 *
 */
class API_EXPORT GvsDevice : public GvsBase
{
public:
    GvsDevice();
    virtual ~GvsDevice();

    void  setChangeObj( GvsBase* obj, std::string idName, std::string name, GvsDataType tp, void* value );
    bool  makeChange();
    void  clearChangeObj();

    void  clear();

    void  setManual ( bool manual );
    virtual void  Print( FILE* fptr = stderr );


public:
    GvsCamera*       camera;         //!< Camera model
    GvsProjector*    projector;      //!< Projector
    m4d::Metric*     metric;         //!< Metric of the spacetime
    GvsLightSrcMgr*  lightSrcMgr;    //!< Light source manager
    GvsSceneObj*     sceneGraph;     //!< Scene graph.

    std::vector<GvsChangeObj*> mChangeObj;   //!< List of object parameters that change from one image to the other.

    bool isManual;
    GvsCamEye  camEye;
};

#endif

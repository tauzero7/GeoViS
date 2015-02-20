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
#ifndef GVS_PIN_HOLE_STEREO_CAM_H
#define GVS_PIN_HOLE_STEREO_CAM_H

#include "GvsGlobalDefs.h"
#include "GvsPinHoleCam.h"

#include "m4dGlobalDefs.h"

/**
 * @brief The pinhole camera is that what you think.
 */
class GvsPinHoleStereoCam : public GvsPinHoleCam
{
public:
    GvsPinHoleStereoCam();
    GvsPinHoleStereoCam(const m4d::vec3 &dir, const m4d::vec3 &vup, const m4d::vec2 &fov, const m4d::ivec2 &res, const double eyeSep );
    virtual ~GvsPinHoleStereoCam();

    m4d::vec3 GetLeftEyePos();
    m4d::vec3 GetRightEyePos();

    std::string  install ( );

    virtual m4d::vec3 GetRayDir ( const double x, const double y );

    virtual int SetParam ( std::string pName, double sep );

    virtual void Print( FILE* fptr = stderr );

private:

};

#endif

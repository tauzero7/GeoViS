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
#ifndef GVS_4PI_CAM_H
#define GVS_4PI_CAM_H

#include <GvsGlobalDefs.h>
#include "GvsCamera.h"

#include "m4dGlobalDefs.h"

/**
 * @brief  Camera model for 4Pi projection. Mapping the complete sky
 *
 */
class Gvs4PICam : public GvsCamera
{
public:
    Gvs4PICam();
    Gvs4PICam(const double angle, const m4d::ivec2 &res);
    virtual ~Gvs4PICam();

    std::string  install ( );

    virtual m4d::vec3 GetRayDir ( const double x, const double y );
    virtual void      PixelToAngle ( const double x, const double y, double &ksi, double &chi );

    virtual int  SetParam ( std::string pName, double angle );
    virtual void Print( FILE* fptr = stderr );

private:
    double mAngle;  // in degrees
};

#endif

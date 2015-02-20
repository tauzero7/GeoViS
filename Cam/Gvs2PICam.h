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
#ifndef GVS_2PI_CAM_H
#define GVS_2PI_CAM_H

#include <GvsGlobalDefs.h>
#include "Cam/GvsCamera.h"

#include "m4dGlobalDefs.h"

/**
 * @brief  Camera model for 2Pi projection that can be used as domemaster
 *         for planetarium projections.
 *
 */
class Gvs2PICam : public GvsCamera
{
public:
    Gvs2PICam();
    Gvs2PICam(const double heading, const double pitch, const int res);
    virtual ~Gvs2PICam();

    void   setHeading ( const double heading );
    void   setPitch   ( const double pitch );

    double getHeading() const;
    double getPitch() const;

    std::string  install ( );

    virtual m4d::vec3 GetRayDir ( const double x, const double y );

    virtual int  SetParam ( std::string pName, double val );

    virtual void Print( FILE* fptr = stderr );

private:
    double viewHeading;   //!< View heading in degrees
    double viewPitch;     //!< View pitch in degrees
};

#endif

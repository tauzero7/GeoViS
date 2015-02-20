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
#ifndef GVS_FLASH_LIGHT_H
#define GVS_FLASH_LIGHT_H

#include <GvsGlobalDefs.h>

#include "Img/GvsColor.h"
#include "Light/GvsLightSrc.h"


class GvsDevice;


enum GvsFlashType {
    gvsFTsingleFlash = 0, gvsFTmultiFlash
};

const std::string GvsFlashTypeName[2] = {"SingleFlash","MultiFlash"};

struct gvs_flashlight
{
    double min;
    double max;
    bool   onoff;
};

class GvsFlashLight : public GvsLightSrc
{
public:
    GvsFlashLight ();
    GvsFlashLight ( const m4d::vec4 pos, const GvsColor col );
    GvsFlashLight ( const m4d::vec4 pos, const GvsColor col,
                    const GvsFlashType flType, const double flStart,
                    const std::string flPattern, const double flOn, const double flOff );

    ~GvsFlashLight();

    virtual  GvsColor getDirectLight ( GvsDevice*       device,
                                       const m4d::vec3& lightDir,
                                       const double     lightDist,
                                       const m4d::vec3& localLightDir,
                                       GvsSurfIntersec* surfIntersec,
                                       GvsShader&       intersecShader,
                                       const m4d::vec3& outDirection,
                                       int  rayIndex,
                                       bool globalIllum  );


    virtual  GvsColor getDirectLight ( GvsDevice*       device,
                                       const m4d::vec3& lightDir,
                                       const double     lightTime,
                                       const double     lightDist,
                                       const m4d::vec3& localLightDir,
                                       GvsSurfIntersec* surfIntersec,
                                       GvsShader&       intersecShader,
                                       const m4d::vec3& outDirection,
                                       int   rayIndex,
                                       bool  globalIllum     );


    void setFlashType      ( const GvsFlashType flType );
    void setFlashStartTime ( const double flStart );
    void setFlashLengths   ( const double flOn, const double flOff );
    void setFlashPattern   ( const std::string pattern );

    void calcFlashPeriod   ( void );


    GvsFlashType getFlashType      ( void ) const;
    double       getFlashStartTime ( void ) const;
    void         getFlashLengths   ( double &flOn, double &flOff );
    std::string       getFlashPattern   ( void ) const;

    bool         testFlash ( const double lightTime );

    virtual void Print ( FILE *fptr = stderr );


protected:
    GvsFlashType  flashType;
    std::string   flashPattern;
    double        flashLengthOn;
    double        flashLengthOff;
    double        flashPeriod;

    double        flashStartTime;

    std::vector<gvs_flashlight>  flash;

};

#endif

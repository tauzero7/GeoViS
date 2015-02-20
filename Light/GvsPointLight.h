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
#ifndef GVS_POINT_LIGHT_H
#define GVS_POINT_LIGHT_H

#include <GvsGlobalDefs.h>
#include "Img/GvsColor.h"
#include "Light/GvsLightSrc.h"


class GvsDevice;

class GvsPointLight : public GvsLightSrc
{
  public:
    GvsPointLight ();
    GvsPointLight ( const m4d::vec4 pos, const GvsColor col );


    virtual  GvsColor getDirectLight ( GvsDevice*       device,
                                       const m4d::vec3& lightDir,
                                       const double     lightDist,
                                       const m4d::vec3& localLightDir,
                                       GvsSurfIntersec* surfIntersec,
                                       GvsShader&       intersecShader,
                                       const m4d::vec3& outDirection,
                                       int  rayIndex,
                                       bool globalIllum );

    virtual  GvsColor getDirectLight ( GvsDevice*       device,
                                       const m4d::vec3& lightDir,
                                       const double     lightTime,
                                       const double     lightDist,
                                       const m4d::vec3& localLightDir,
                                       GvsSurfIntersec* surfIntersec,
                                       GvsShader&       intersecShader,
                                       const m4d::vec3& outDirection,
                                       int  rayIndex,
                                       bool globalIllum     );


    virtual void Print ( FILE* fptr = stderr );
};


#endif

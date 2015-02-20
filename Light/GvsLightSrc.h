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
#ifndef GVS_LIGHT_SRC_H
#define GVS_LIGHT_SRC_H

#include <Obj/GvsBase.h>
#include <GvsGlobalDefs.h>
#include <Shader/GvsShader.h>


class GvsDevice;
class GvsColor;
class GvsSceneObj;
class GvsSurfIntersec;

enum GvsLightSrcType {
    lightSrc
};

class GvsLightSrc : public GvsBase
{
public:
    GvsLightSrc ();
    virtual ~GvsLightSrc ();

    virtual GvsColor getDirectLight ( GvsDevice*       device,
                                      const m4d::vec3& lightDir,
                                      const double     lightDist,
                                      const m4d::vec3& localLightDir,
                                      GvsSurfIntersec* intersec,
                                      GvsShader&       intersecShader,
                                      const m4d::vec3& outDirection,
                                      int   rayIndex,
                                      bool  globalIllum ) = 0;

    virtual  GvsColor getDirectLight ( GvsDevice*       device,
                                       const m4d::vec3&       lightDir,
                                       const double     lightTime,
                                       const double     lightDist,
                                       const m4d::vec3&       localLightDir,
                                       GvsSurfIntersec* surfIntersec,
                                       GvsShader&       intersecShader,
                                       const m4d::vec3&     outDirection,
                                       int            rayIndex,
                                       bool           globalIllum     ) = 0;

    virtual void       setPosition  ( const m4d::vec4 pos );
    virtual m4d::vec4  getPosition  ( void ) const;


    void     setColor     ( const GvsColor& col );
    GvsColor color        ( void ) const;

    virtual void     Print ( FILE* fptr = stderr );


protected:
    m4d::vec4  lightPosition;
    GvsColor   lightColor;
};


inline void GvsLightSrc :: setColor( const GvsColor& col ) {
    lightColor = col;
}

inline GvsColor GvsLightSrc :: color() const {
    return lightColor;
}

#endif

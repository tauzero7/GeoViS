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

#include "GvsLightSrc.h"
#include <Dev/GvsDevice.h>
#include <Light/GvsLightSrcMgr.h>
#include <Obj/GvsSceneObj.h>
#include <Ray/GvsRayAnyIS.h>
#include <Ray/GvsRayVisual.h>


GvsLightSrc :: GvsLightSrc () {
}

GvsLightSrc :: ~GvsLightSrc () {
}


void GvsLightSrc :: setPosition  ( const m4d::vec4 pos ) {
    lightPosition = pos;
}

m4d::vec4 GvsLightSrc :: getPosition  ( void ) const {
    return lightPosition;
}


void GvsLightSrc::Print ( FILE* fptr ) {
    fprintf(fptr,"Light {\n");
    fprintf(fptr,"\tpos: "); lightPosition.printS(fptr);
    fprintf(fptr,"}\n");
}

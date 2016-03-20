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

#include "GvsLightSrcMgr.h"


GvsLightSrcMgr :: GvsLightSrcMgr () {
    ambLight = GvsColor(1.0);
    useShadowRays = false;

    if (lightSrcList.size()!=0) {
        lightSrcList.clear();
    }
    listLength = 0;
}

GvsLightSrcMgr :: ~GvsLightSrcMgr() {
    clear();
}


void GvsLightSrcMgr::clear() {
    if (lightSrcList.size()!=0) {
        for (unsigned int i=0; i<lightSrcList.size(); i++) {
            delete lightSrcList[i];
        }
        lightSrcList.clear();
    }
    listLength = 0;
}


void
GvsLightSrcMgr :: remove ( int num )
{
    if ((!lightSrcList.empty()) && (num<listLength))
    {
        lightSrcListPtr = lightSrcList.begin()+num;
        lightSrcList.erase(lightSrcListPtr);
        listLength = (int)lightSrcList.size();
    }
}


void GvsLightSrcMgr :: append ( GvsLightSrc* elem ) {
    lightSrcList.push_back(elem);
    listLength = (int)lightSrcList.size();
}


GvsLightSrc*
GvsLightSrcMgr :: getLightSrc ( int num, bool &lightFound )
{
    if ((!lightSrcList.empty()) & ( num < listLength )) {
        lightFound = true;
        return lightSrcList[num];
    }
    else {
        lightFound = false;
        return NULL;
    }
}


void GvsLightSrcMgr :: Print ( FILE* fptr ) {
    fprintf(fptr,"Light source mgr {\n");
    fprintf(fptr,"\tambient:    "); ambLight.Print(fptr);
    fprintf(fptr,"\t#sources: %d\n",listLength);
    for(int i=0; i<listLength; i++) {
        lightSrcList[i]->Print(fptr);
    }
    fprintf(fptr,"}\n\n");
}

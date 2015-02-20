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

#include "GvsPinHoleStereoCam.h"


GvsPinHoleStereoCam::GvsPinHoleStereoCam() : GvsPinHoleCam() {
    AddParam("eyesep",gvsDT_DOUBLE);
    mEyeSep = 0.1;
    mIsStereoCam = true;
    install();
}

GvsPinHoleStereoCam::GvsPinHoleStereoCam(const m4d::vec3 &dir, const m4d::vec3 &vup, const m4d::vec2 &fov, const m4d::ivec2 &res, double eyeSep) :
    GvsPinHoleCam(dir,vup,fov,res) {
    AddParam("eyesep",gvsDT_DOUBLE);
    mEyeSep = eyeSep;
    if (eyeSep>0.0) {
        mIsStereoCam = true;
    }
    install();
}

GvsPinHoleStereoCam::~GvsPinHoleStereoCam() {
    DelAllParam();
}

m4d::vec3 GvsPinHoleStereoCam::GetLeftEyePos() {
    return -0.5 * mEyeSep * viewHorizHalfVector;
}

m4d::vec3 GvsPinHoleStereoCam::GetRightEyePos() {
    return 0.5 * mEyeSep * viewHorizHalfVector;
}

std::string GvsPinHoleStereoCam::install() {
    // std::cerr << "GvsPinHoleCam :: install()...";
    return GvsPinHoleCam::install();
}


m4d::vec3 GvsPinHoleStereoCam::GetRayDir ( const double x, const double y )  {
    double sx, sy;
    m4d::vec3 dir;

    sx = (x + 0.5) / viewResolution.x(0);
    sy = (y + 0.5) / viewResolution.x(1);
    //fprintf(stderr,"x: %f y: %f  sx: %f sy: %f  %d %d\n",x,y,sx,sy,viewResolution.x(0),viewResolution.x(1));

    dir = viewDirection
          + (2.0 * sx - 1.0)/aspectRatio * viewHorizHalfVector
          + (1.0 - 2.0 * sy) * viewVertHalfVector;
    return dir.getNormalized();
}


int GvsPinHoleStereoCam::SetParam(std::string pName, double sep ) {
    int isOkay = GvsBase::SetParam(pName,sep);
    if (isOkay >= gvsSetParamNone && pName=="eyesep") {
        mEyeSep = sep;
        install();
    }
    return isOkay;
}


void GvsPinHoleStereoCam::Print( FILE* fptr ) {
    fprintf(fptr,"PinholeStereoCamera {\n");
    fprintf(fptr,"\tdir    %6.3f %6.3f %6.3f\n",viewDirection.x(0),viewDirection.x(1),viewDirection.x(2));
    fprintf(fptr,"\tvup    %6.3f %6.3f\n",viewUpVector.x(0),viewUpVector.x(1));
    fprintf(fptr,"\tfov    %6.3f %6.3f\n",viewField.x(0),viewField.x(1));
    fprintf(fptr,"\tres    %4d %4d\n",viewResolution.x(0),viewResolution.x(1));
    fprintf(fptr,"\tres    %4f\n",mEyeSep);
    fprintf(fptr,"\tfilter %s\n",GvsCamFilterNames[camFilter].c_str());
    fprintf(fptr,"}\n\n");
}

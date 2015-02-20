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

#include "Cam/Gvs2PICam.h"

#include "math/TransfMat.h"

Gvs2PICam :: Gvs2PICam() {
    viewHeading = 0.0;
    viewPitch   = 0.0;
    viewResolution = m4d::ivec2(100,100);

    AddParam("heading",gvsDT_DOUBLE);
    AddParam("pitch",gvsDT_DOUBLE);
    install();
}

Gvs2PICam :: Gvs2PICam(const double heading, const double pitch, const int res) {
    viewHeading = heading;
    viewPitch   = pitch;
    viewResolution = m4d::ivec2(res,res);

    AddParam("heading",gvsDT_DOUBLE);
    AddParam("pitch",gvsDT_DOUBLE);
    install();
}

Gvs2PICam::~Gvs2PICam() {
}


void Gvs2PICam::setHeading( const double heading ) {
    viewHeading = heading;
}

void Gvs2PICam::setPitch( const double pitch ) {
    viewPitch = pitch;
}

double Gvs2PICam::getHeading() const {
    return viewHeading;
}

double Gvs2PICam::getPitch() const {
    return viewPitch;
}


std::string Gvs2PICam :: install() {
    return std::string();
}


m4d::vec3 Gvs2PICam::GetRayDir ( const double x, const double y ) {
    //double sx = 2.0*((x+0.5)/static_cast<double>(viewResolution.x(0)) - 0.5);
    double sx = 2.0*(0.5-(x+0.5)/static_cast<double>(viewResolution.x(0)));
    double sy = 2.0*(0.5-(y+0.5)/static_cast<double>(viewResolution.x(0)));
    double sxy2 = sx*sx + sy*sy;
    if (sxy2 > 1.0) {
        return m4d::vec3();
    }

    double sz = sqrt(1.0 - sxy2);

    m4d::vec3 dir = m4d::vec3(sx,sy,sz);
    dir = dir.getNormalized();

    dir = m4d::RotateMat3D(m4d::axis_Z,-viewHeading*DEG_TO_RAD) *
            m4d::RotateMat3D(m4d::axis_Y,-viewPitch*DEG_TO_RAD) *
            m4d::RotateMat3D(m4d::axis_Z,1.5*M_PI) * dir;
    return dir;
}


int Gvs2PICam::SetParam( std::string pName, double val ) {
    int isOkay = GvsBase::SetParam(pName,val);
    if (isOkay >= gvsSetParamNone) {
        if (pName=="heading") {
            viewHeading = val;
        } else if (pName=="pitch") {
            viewPitch = val;
        }
    }
    return isOkay;
}


void Gvs2PICam::Print( FILE* fptr ) {
    fprintf(fptr,"2PICam {\n");
    fprintf(fptr,"\theading: %f\n",viewHeading);
    fprintf(fptr,"\tpitch:   %f\n",viewPitch);
    fprintf(fptr,"\tres:     %d\n",viewResolution.x(0));
    fprintf(fptr,"\tfilter:  %s\n",GvsCamFilterNames[camFilter].c_str());
    fprintf(fptr,"}\n");
}

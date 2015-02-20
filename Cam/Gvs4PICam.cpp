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

#include "Gvs4PICam.h"
#include <GvsGlobalDefs.h>

//----------------------------------------------------------------------------
//         Constructor, Destructor
//----------------------------------------------------------------------------
Gvs4PICam :: Gvs4PICam()
{
    // In case of cartesian coordinates the direction of view is in negative
    // x-direction, the upvector points in the positive z-direction
    // (this depends on the standard orientation of the projector)

    viewResolution = m4d::ivec2(720,576);
    mAngle = 0.0;
    AddParam("angle",gvsDT_DOUBLE);

    install();
}

Gvs4PICam :: Gvs4PICam(const double angle, const m4d::ivec2 &res)
{
  viewResolution = res;
  mAngle = angle;
  AddParam("angle",gvsDT_DOUBLE);

  install();
}

Gvs4PICam::~Gvs4PICam() {

}


std::string Gvs4PICam::install() {
    //cerr << "GvsPinHoleCam :: install()...";
    return std::string();
}


m4d::vec3 Gvs4PICam::GetRayDir ( const double x, const double y )  {
    double sx, sy;
    sx = (x + 0.5) / viewResolution.x(0);
    sy = (y + 0.5) / viewResolution.x(1);

    double theta = sy*M_PI;
    double phi   = (1.0-2.0*sx)*M_PI + mAngle * DEG_TO_RAD;
    return m4d::vec3( sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta) );
}


void Gvs4PICam::PixelToAngle ( const double x, const double y, double &ksi, double &chi ) {
    double sx, sy;
    sx = (x + 0.5) / viewResolution.x(0);
    sy = (y + 0.5) / viewResolution.x(1);

    chi = sy*M_PI;
    ksi = (1.0-2.0*sx)*M_PI + mAngle * DEG_TO_RAD;
}


int Gvs4PICam::SetParam( std::string pName, double angle ) {
    int isOkay = GvsBase::SetParam(pName,angle);
    if (isOkay >= gvsSetParamNone && pName=="angle") {
        mAngle = angle;
    }
    return isOkay;
}


void Gvs4PICam::Print( FILE* fptr ) {
    fprintf(fptr,"PinholeCamera {\n");
    fprintf(fptr,"\tangle  %6.3f (deg)\n",mAngle);
    fprintf(fptr,"\tres    %4d %4d\n",viewResolution.x(0),viewResolution.x(1));
    fprintf(fptr,"\tfilter %s\n",GvsCamFilterNames[camFilter].c_str());
    fprintf(fptr,"}\n\n");
}

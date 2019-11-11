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

#include "GvsSolidObj.h"


//----------------------------------------------------------------------------
//         constructor, destructor
//----------------------------------------------------------------------------
GvsSolidObj::GvsSolidObj(GvsSurfaceShader* shader) : GvsSurface(shader)
{
}

GvsSolidObj::~GvsSolidObj() {

}


bool GvsSolidObj::testIntersection( GvsRay &) {
    std::cerr << "Error in GvsSolidObj::testIntersection(GvsRay&): not implemented." << std::endl;
    return false;
}

bool GvsSolidObj::testLocalIntersection(GvsRay &, const int ,
                                         GvsLocalTetrad* , GvsLocalTetrad* ,
                                         const m4d::vec4 , const m4d::vec4 )
{
    std::cerr << "Error in GvsSolidObj::testLocalIntersection(GvsRay&): not implemented." << std::endl;
    return false;
}


bool GvsSolidObj::getRaySpanList(GvsRay &, GvsSolObjSpanList& ) {
    std::cerr << "Error in GvsSceneObj::getRaySpanList(GvsRay& ray): not implemented." << std::endl;
    return false;
}



int GvsSolidObj::SetParam( std::string pName, m4d::Matrix<double,3,4> mat ) {
    int isOkay = GvsBase::SetParam(pName,mat);
    if (isOkay >= gvsSetParamNone && getLowCase(pName)=="transform")   {
        volParamTransfMat = mat * volTransfMat;
        volParamInvTransfMat = volParamTransfMat;
        volParamInvTransfMat.invert();
        mHaveSetParamTransfMat = true;
    }
    return isOkay;
}


bool GvsSolidObj :: haveSetParamTransfMat () const {
    return mHaveSetParamTransfMat;
}

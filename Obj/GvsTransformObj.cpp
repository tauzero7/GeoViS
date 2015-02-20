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

#include "GvsGlobalDefs.h"
#include "Obj/GvsTransformObj.h"

#include "math/TransfMat.h"

GvsTransformObj::GvsTransformObj() {
}

GvsTransformObj::~GvsTransformObj() {
}

void GvsTransformObj::scale( const m4d::vec3& scaleVec ) {
    transform ( m4d::ScaleMat3D( scaleVec ) );
}

void GvsTransformObj::translate( const m4d::vec3& transVec ) {
    transform( m4d::TranslateMat3D( transVec ) );
}

void GvsTransformObj::rotate( const m4d::vec3& rotAxis, double rotAngle ) {
    transform( m4d::RotateMat3D( rotAxis, rotAngle ) );
}

void GvsTransformObj::transform (const m4d::Matrix<double,3,4>& ) {
#ifdef GVS_VERBOSE
    fprintf(stderr,"Transform of object not implemented.\n");
#endif
    // not implemented here
    // TODO: make pure virtual
}



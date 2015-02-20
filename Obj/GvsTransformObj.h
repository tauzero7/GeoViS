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
#ifndef GVS_TRANSFORM_OBJ_h
#define GVS_TRANSFORM_OBJ_h

#include "Obj/GvsBase.h"
#include "math/Mat.h"

/**
 * @brief The GvsTransformObj class
 */
class GvsTransformObj : public GvsBase
{
public:
    GvsTransformObj();
    virtual ~GvsTransformObj();

    virtual void scale     ( const m4d::vec3& scaleVec );
    virtual void translate ( const m4d::vec3& transVec );
    virtual void rotate    ( const m4d::vec3& rotAxis, double rotAngle );
    virtual void transform ( const m4d::Matrix<double,3,4>& mat );
};

#endif

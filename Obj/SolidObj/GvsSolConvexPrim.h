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
#ifndef GVS_SOL_CONVEX_PRIM_H
#define GVS_SOL_CONVEX_PRIM_H

#include <GvsGlobalDefs.h>
#include "Obj/SolidObj/GvsSolidObj.h"
#include "Obj/GvsBoundBox.h"
#include "Ray/GvsRay.h"


/**
 * @brief The GvsSolConvexPrim class
 */
class GvsSolConvexPrim : public GvsSolidObj
{
public:
    GvsSolConvexPrim(GvsSurfaceShader* shader);
    virtual ~GvsSolConvexPrim() {}

    virtual void  scale     ( const m4d::vec3 &scaleVec);
    virtual void  translate ( const m4d::vec3 &transVec);
    virtual void  rotate    ( const m4d::vec3 &rotAxis, double rotAngle);
    virtual void  transform ( const m4d::Matrix<double,3,4> &mat);

    virtual GvsBoundBox boundingBox ( ) const;

    //! Test intersection with a coordinate object.
    virtual bool testIntersection ( GvsRay &ray );

    virtual bool testLocalIntersection ( GvsRay &ray, const int seg,
                                         GvsLocalTetrad* lt0, GvsLocalTetrad* lt1, const m4d::vec4 p0, const m4d::vec4 p1 );

    virtual bool getTentryTexit ( const m4d::vec3& p0, const m4d::vec3& p1,
                                  double tp0, double tp1,
                                  double& time_Entry, double& time_Exit,
                                  short &entryFace, short &exitFace ) const;


protected:
    virtual void calcBoundBox( ) = 0;

protected:
    GvsBoundBox  volBoundBox;
};


#endif

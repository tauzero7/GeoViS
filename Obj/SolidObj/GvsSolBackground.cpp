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

#include "Obj/SolidObj/GvsSolBackground.h"
#include "Ray/GvsRay.h"
#include "Ray/GvsSurfIntersec.h"
#include "Shader/Surface/GvsSurfaceShader.h"

#include "metric/m4dMetric.h"


GvsSolBackground :: GvsSolBackground(GvsSurfaceShader* shader,
                                      m4d::Metric*      metric,
                                      GvsObjType        objType )
    : GvsSolEllipsoid(m4d::vec3(0,0,0), m4d::vec3(1,1,1), shader, metric, objType )
{
}


bool GvsSolBackground :: testIntersection ( GvsRay &ray ) {
    return GvsSolConvexPrim::testIntersection( ray );
}


bool GvsSolBackground :: testLocalIntersection ( GvsRay &ray, const int seg,
                                                 GvsLocalTetrad* lt0, GvsLocalTetrad* lt1,
                                                 const m4d::vec4 p0, const m4d::vec4 p1 )
{
    return GvsSolConvexPrim :: testLocalIntersection( ray, seg, lt0, lt1, p0, p1 );
}


void GvsSolBackground :: Print( FILE* fptr ) {
    fprintf(fptr,"SolBackground {\n");
    fprintf(fptr,"\tShader:\n");
    if (surfShader!=NULL) {
        surfShader->Print(fptr);
    }
    fprintf(stderr,"}\n");
}


bool
GvsSolBackground :: PtInsideEllipsoid ( const m4d::vec4& pt ) const
{
  m4d::vec3 pt_3D( pt.x(1), pt.x(1), pt.x(1) );
  m4d::vec3 localPt = volInvTransfMat * pt_3D;

  return ( localPt.x(0) * localPt.x(0) +
           localPt.x(1) * localPt.x(1) +
           localPt.x(2) * localPt.x(2) <= 1.0 );
}




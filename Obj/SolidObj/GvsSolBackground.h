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
#ifndef GVS_SOL_BACKGROUND_H
#define GVS_SOL_BACKGROUND_H

#include <GvsGlobalDefs.h>

#include "Obj/SolidObj/GvsSolConvexPrim.h"
#include "Obj/SolidObj/GvsSolEllipsoid.h"
#include "Obj/STMotion/GvsLocalTetrad.h"


class GvsRay;
class GvsSurfaceShader;


class GvsSolBackground : public GvsSolEllipsoid
{
  public:
    GvsSolBackground();

    GvsSolBackground( GvsSurfaceShader* shader,
                      m4d::Metric*      metric,
                      GvsObjType objType = local );

    virtual bool testIntersection      ( GvsRay& ray );

    virtual bool testLocalIntersection ( GvsRay& ray, const int seg,
                                         GvsLocalTetrad* lt0, GvsLocalTetrad* lt1,
                                         const m4d::vec4 center, const m4d::vec4 halfAxisLength );

    virtual bool PtInsideEllipsoid     ( const m4d::vec4& pt ) const;

    virtual void Print( FILE* fptr = stderr );
};

#endif

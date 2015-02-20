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
#ifndef GVS_SOL_CYLINDER_H
#define GVS_SOL_CYLINDER_H

#include <Obj/SolidObj/GvsSolConvexPrim.h>
#include <Obj/STMotion/GvsLocalTetrad.h>
#include <GvsGlobalDefs.h>

class GvsRay;
class GvsSurfIntersec;
class GvsSurfaceShader;
class GvsStMotion;
class GvsSolObjSpanList;


class GvsSolCylinder : public GvsSolConvexPrim
{
public:
    GvsSolCylinder ( const m4d::vec3&  basePt,
                     const m4d::vec3&  topPt,
                     const m4d::vec2&  radii,
                     GvsSurfaceShader* shader,
                     m4d::Metric*      metric,
                     GvsObjType objType = local );

    GvsSolCylinder ( const m4d::vec3&        basePt,
                     const m4d::vec3&        topPt,
                     const m4d::vec2&        radii,
                     GvsSurfaceShader* shader,
                     m4d::Metric*        metric,
                     GvsStMotion*      motion,
                     GvsObjType objType = local );


    void setGeometry   ( const m4d::vec3& basePt, const m4d::vec3& topPt, const m4d::vec2& radii );
    void setBasePoint  ( const m4d::vec3& basePt );
    void setTopPoint   ( const m4d::vec3& topPt  );
    void setRadii      ( const m4d::vec2& radii  );


    virtual bool getRaySpanList        ( GvsRay &ray, GvsSolObjSpanList& isl );

    virtual void calcNormal            ( GvsSurfIntersec & intersec ) const;
    virtual void calcTexUVParam        ( GvsSurfIntersec & intersec ) const;
    virtual void calcDerivatives       ( GvsSurfIntersec & intersec ) const;



    virtual void Print( FILE* fptr = stderr );

    bool         PtInsideCylinder      ( const m4d::vec3& pt       ) const;

protected:

    virtual void calcBoundBox     ( void );


    bool         getTentryTexit ( const m4d::vec3& p0, const m4d::vec3& p1, double tp0, double tp1,
                                  double& time_Entry, double& time_Exit,
                                  short& entryFace, short& exitFace ) const;

private:
    m4d::vec3  mBasePoint;
    m4d::vec3  mTopPoint;
    m4d::vec2  mRadii;
};


#endif

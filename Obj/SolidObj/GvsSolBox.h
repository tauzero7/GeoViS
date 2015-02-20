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
#ifndef GVS_SOL_BOX_H
#define GVS_SOL_BOX_H

#include <GvsGlobalDefs.h>

#include <Obj/SolidObj/GvsSolConvexPrim.h>
#include <Obj/STMotion/GvsLocalTetrad.h>

class GvsRay;
class GvsSurfIntersec;
class GvsSurfaceShader;
class GvsStMotion;

const m4d::vec3 boxNormal[] = { m4d::vec3(-1.0, 0.0, 0.0),
                          m4d::vec3( 1.0, 0.0, 0.0),
                          m4d::vec3( 0.0,-1.0, 0.0),
                          m4d::vec3( 0.0, 1.0, 0.0),
                          m4d::vec3( 0.0, 0.0,-1.0),
                          m4d::vec3( 0.0, 0.0, 1.0)
                        };

/**
 * @brief The GvsSolBox class
 */
class GvsSolBox : public GvsSolConvexPrim
{
  public:
    GvsSolBox( const m4d::vec3 corner0, const m4d::vec3 corner1,
               GvsSurfaceShader* shader, m4d::Metric* spacetime, GvsObjType objType=local);

    GvsSolBox( const m4d::vec3 corner0, const m4d::vec3 corner1,
               GvsSurfaceShader* shader,  m4d::Metric* spacetime,
               GvsStMotion* motion, GvsObjType objType=local);
    ~GvsSolBox () {}

    bool getTentryTexit                ( const m4d::vec3& p0, const m4d::vec3& p1,
                                         double tp0, double tp1,
                                         double& time_Entry, double& time_Exit,
                                         short &entryFace, short &exitFace) const;

    virtual bool testIntersection      ( GvsRay &ray );
    
    virtual bool testLocalIntersection ( GvsRay &ray, const int seg,
                                         GvsLocalTetrad* lt0, GvsLocalTetrad* lt1,
                                         const m4d::vec4 p0, const m4d::vec4 p1 );
//OF
    virtual bool getRaySpanList ( GvsRay &ray, GvsSolObjSpanList& isl );
//OF Ende
    virtual void calcNormal     ( GvsSurfIntersec &intersec ) const;
    virtual void calcTexUVParam ( GvsSurfIntersec &intersec ) const;

    virtual void Print( FILE* fptr = stderr );

  protected:
    virtual void calcBoundBox();
    m4d::vec3 mCornerLL;
    m4d::vec3 mCornerUR;
};


#endif

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
#ifndef GVS_SOL_ELLIPSOID_H
#define GVS_SOL_ELLIPSOID_H

#include <GvsGlobalDefs.h>

#include <Obj/SolidObj/GvsSolConvexPrim.h>
#include <Obj/STMotion/GvsLocalTetrad.h>

class GvsRay;
class GvsSurfIntersec;
class GvsSurfaceShader;
class GvsStMotion;

class GvsSolEllipsoid : public GvsSolConvexPrim
{
public:
    GvsSolEllipsoid( const m4d::vec3&  center,
                     const m4d::vec3&  halfAxisLength,
                     GvsSurfaceShader* shader,
                     m4d::Metric*      metric,
                     GvsObjType objType = local );

    GvsSolEllipsoid( const m4d::vec3&  center,
                     const m4d::vec3&  halfAxisLength,
                     GvsSurfaceShader* shader,
                     m4d::Metric*      metric,
                     GvsStMotion*      motion,
                     GvsObjType objType = local );


    void setGeometry       ( const m4d::vec3 center, const m4d::vec3 halfAxisLength );
    void setCenter         ( const m4d::vec3 center );
    void setHalfAxisLength ( const m4d::vec3 halfAxisLength );

    m4d::vec3 getCenter          () const;
    m4d::vec3 getHalfAxisLength  () const;


    bool getTentryTexit    ( const m4d::vec3& p0, const m4d::vec3& p1,
                             double tp0, double tp1,
                             double& time_Entry, double& time_Exit,
                             short &entryFace, short &exitFace) const;

    virtual bool getRaySpanList        ( GvsRay& ray, GvsSolObjSpanList& spanList );

    virtual void calcNormal            ( GvsSurfIntersec &intersec ) const;
    virtual void calcTexUVParam        ( GvsSurfIntersec &intersec ) const;

    virtual void calcDerivatives       ( GvsSurfIntersec &intersec ) const;
    virtual bool PtInsideEllipsoid     ( const m4d::vec4& pt ) const;

    virtual int SetParam ( std::string pName, m4d::vec3 p );

    virtual void Print( FILE* fptr = stderr );

protected:
    virtual void calcBoundBox();

private:
    m4d::vec3 mCenter;                   //Zentrum des Ellipsoiden
    m4d::vec3 mHalfAxisLength;           //Halbachsenabschnitte
};

#endif

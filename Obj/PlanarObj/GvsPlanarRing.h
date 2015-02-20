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
#ifndef GVS_PLANAR_RING_H
#define GVS_PLANAR_RING_H

#include "Obj/PlanarObj/GvsPlanarSurf.h"

class GvsRay;
class GvsRayAllIS;
class GvsStMotion;


class GvsPlanarRing : public GvsPlanarSurf
{
public:
    GvsPlanarRing( );
    GvsPlanarRing( const m4d::vec3& center, const m4d::vec3& normal,
                   const double rOuter, const double rInner,
                   GvsSurfaceShader* shader, m4d::Metric* metric,
                   GvsObjType objType=local );
    virtual ~GvsPlanarRing( void );

    virtual void  rotate( const m4d::vec3 &rotAxis, double rotAngle);
    virtual void  transform( const m4d::Matrix<double,3,4> &mat);

    virtual GvsPlanarSurf* getClone      ( void ) const;

    virtual bool  isValidHit ( m4d::vec3 rp );

    virtual void  calcNormal       ( GvsSurfIntersec & intersec ) const;
    virtual void  calcTexUVParam   ( GvsSurfIntersec & intersec ) const;

    virtual void  Print ( FILE* fptr = stderr );

protected:
    virtual void calcBoundBox();

protected:
    m4d::vec3    ringCenter;
    m4d::vec3    e1,e2;
    double       ringOuterRadius;
    double       ringInnerRadius;
};

#endif

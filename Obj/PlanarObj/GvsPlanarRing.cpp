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

#include "Obj/PlanarObj/GvsPlanarRing.h"

#include <Ray/GvsRay.h>
#include <Ray/GvsRayAllIS.h>
#include <Obj/STMotion/GvsStMotion.h>

#include "math/TransfMat.h"

/**
 * @brief GvsPlanarRing::GvsPlanarRing
 */
GvsPlanarRing::GvsPlanarRing()
    :  GvsPlanarSurf( NULL )
{
    ringCenter  = m4d::vec3(0.0,0.0,0.0);
    planeNormal = m4d::vec3(0.0,0.0,1.0);
    planeDist   = 0.0;
    e1 = m4d::vec3(1.0,0.0,0.0);
    e2 = m4d::vec3(0.0,1.0,0.0);

    ringOuterRadius = 2.0;
    ringInnerRadius = 1.0;

    mMetric = NULL;
    stMotion = NULL;
    mObjType = inCoords;
    calcBoundBox();
}

/**
 * @brief GvsPlanarRing::GvsPlanarRing
 * @param center
 * @param normal
 * @param rOuter
 * @param rInner
 * @param shader
 * @param metric
 * @param objType
 */
GvsPlanarRing :: GvsPlanarRing(const m4d::vec3& center, const m4d::vec3& normal,
                   const double rOuter, const double rInner,
                   GvsSurfaceShader* shader, m4d::Metric* metric,
                   GvsObjType objType )
    :  GvsPlanarSurf ( shader )
{
    ringCenter  = center;
    planeNormal = normal.getNormalized();

    m4d::vec3 z = m4d::vec3(0.0,0.0,1.0);
    if ((e1^z).getNorm()>GVS_EPS) {
        e1 = (z^planeNormal).getNormalized();
        e2 = planeNormal^e1;
    } else {
        e1 = m4d::vec3(1.0,0.0,0.0);
        e2 = m4d::vec3(0.0,1.0,0.0);
    }

    planeDist = ringCenter|planeNormal;

    ringOuterRadius = rOuter;
    ringInnerRadius = rInner;

    mMetric = metric;
    stMotion = NULL;
    mObjType = objType;
    calcBoundBox();
}

GvsPlanarRing::~GvsPlanarRing(){
}


void GvsPlanarRing::rotate( const m4d::vec3 &rotAxis, double rotAngle) {
    m4d::Matrix<double,3,4> mat = m4d::RotateMat3D( rotAxis, rotAngle );
    ringCenter = mat * ringCenter;
    planeNormal = mat * planeNormal;
    planeDist = ringCenter|planeNormal;    
    calcBoundBox();
}

void GvsPlanarRing::transform( const m4d::Matrix<double,3,4> &mat) {
    if ( !mat.isIdentMat() ) {
        ringCenter = mat * ringCenter;
        planeNormal = mat * planeNormal;
        planeDist = ringCenter|planeNormal;
        calcBoundBox();
    }
}

GvsPlanarSurf* GvsPlanarRing :: getClone( void ) const {
    GvsPlanarRing* clone = new GvsPlanarRing( *this );
    assert( clone != NULL );
    return clone;
}

bool GvsPlanarRing::isValidHit ( m4d::vec3 rp ) {
    double dist = (rp-ringCenter).getNorm();
    if (dist>=ringInnerRadius && dist<=ringOuterRadius) {
        return true;
    }
    return false;
}


void GvsPlanarRing::calcNormal( GvsSurfIntersec & intersec ) const {
    intersec.setNormal( planeNormal );
}


void GvsPlanarRing::calcTexUVParam( GvsSurfIntersec & intersec ) const {
    m4d::vec3 locPoint = intersec.localPoint();

    double u = locPoint|e1;
    double v = locPoint|e2;

    double dist = (locPoint-ringCenter).getNorm();
    double r = (dist - ringInnerRadius) / (ringOuterRadius - ringInnerRadius);
    double phi = atan2(v,u)/(2.0*M_PI) + 0.5;
    //fprintf(stderr," %f %f  %f %f\n",u,v,r,phi);
    //intersec.setTexUVParam(phi,r);
    intersec.setTexUVParam(r,phi);
}


void GvsPlanarRing::Print ( FILE* fptr ) {
    fprintf(fptr,"PlanarRing {\n");
    fprintf(fptr,"\tcenter: ");ringCenter.printS(fptr);
    fprintf(fptr,"\tnormal: ");planeNormal.printS(fptr);
    fprintf(fptr,"\tRout:   %f\n",ringOuterRadius);
    fprintf(fptr,"\tRin:    %f\n",ringInnerRadius);
    fprintf(fptr,"}\n");
}


void GvsPlanarRing::calcBoundBox() {
    m4d::vec3 z = m4d::vec3(0.0,0.0,1.0);
    m4d::vec3 v1 = planeNormal^z;
    if (v1.isZero()) {
        z = m4d::vec3(1.0,0.0,0.0);
        v1 = planeNormal^z;
    }
    v1 = v1.getNormalized();

    m4d::vec3 v2 = planeNormal^v1;
    planarSurfBoundBox = GvsBoundBox( ringCenter+ringOuterRadius*v1, ringCenter+ringOuterRadius*v2 );
}



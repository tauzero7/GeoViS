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

#include "Obj/SolidObj/GvsSolCylinder.h"
#include "Obj/SolidObj/GvsSolObjSpanList.h"
#include "Ray/GvsRay.h"
#include "Ray/GvsSurfIntersec.h"
#include "Shader/Surface/GvsSurfaceShader.h"

#include <metric/m4dMetric.h>
#include "math/TransfMat.h"


#define CYL_BOTT  0
#define CYL_TOP   1
#define CYL_SIDE  2


GvsSolCylinder::GvsSolCylinder( const m4d::vec3&  basePt,
                                const m4d::vec3&  topPt,
                                const m4d::vec2&  radii,
                                GvsSurfaceShader* shader,
                                m4d::Metric*      metric,
                                GvsObjType  objType )
    : GvsSolConvexPrim( shader )
{
    mObjType = objType;
    mMetric = metric;

    setGeometry(basePt,topPt,radii);
}

GvsSolCylinder::GvsSolCylinder ( const m4d::vec3&  basePt,
                                 const m4d::vec3&  topPt,
                                 const m4d::vec2&  radii,
                                 GvsSurfaceShader* shader,
                                 m4d::Metric*      metric,
                                 GvsStMotion*      motion,
                                 GvsObjType objType )
    : GvsSolConvexPrim( shader )
{
    mObjType = objType;
    mMetric = metric;
    stMotion = motion;

    setGeometry(basePt,topPt,radii);
}

/**
 * @brief GvsSolCylinder::setGeometry
 * @param basePt
 * @param topPt
 * @param radii
 */
void GvsSolCylinder :: setGeometry( const m4d::vec3& basePt, const m4d::vec3& topPt, const m4d::vec2& radii )
{
    m4d::vec3 cylAxis = topPt - basePt;

    double alpha = atan2( cylAxis.x(1), cylAxis.x(0) );
    double beta  = atan2( sqrt(cylAxis.x(0)*cylAxis.x(0) + cylAxis.x(1)*cylAxis.x(1)), cylAxis.x(2) );

    volTransfMat = m4d::TranslateMat3D ( basePt ) *
            m4d::RotateMat3D ( m4d::axis_Z, alpha ) *
            m4d::RotateMat3D ( m4d::axis_Y, beta ) *
            m4d::ScaleMat3D  ( radii.x(0), radii.x(1), cylAxis.getNorm() );

    volInvTransfMat = m4d::ScaleMat3D( 1.0/radii.x(0), 1.0/radii.x(1),
                                       1.0/cylAxis.getNorm()     ) *
            m4d::RotateMat3D    ( m4d::axis_Y, -beta ) *
            m4d::RotateMat3D    ( m4d::axis_Z, -alpha ) *
            m4d::TranslateMat3D ( -basePt );


    mBasePoint = basePt;
    mTopPoint  = topPt;
    mRadii     = radii;

    calcBoundBox();
}

/**
 * @brief GvsSolCylinder::setBasePoint
 * @param basePt
 */
void GvsSolCylinder :: setBasePoint( const m4d::vec3& basePt ) {
    mBasePoint = basePt;
}

/**
 * @brief GvsSolCylinder::setTopPoint
 * @param topPt
 */
void GvsSolCylinder :: setTopPoint( const m4d::vec3& topPt  ) {
    mTopPoint  = topPt;
}

/**
 * @brief GvsSolCylinder::setRadii
 * @param radii
 */
void GvsSolCylinder :: setRadii( const m4d::vec2& radii  ) {
    mRadii     = radii;
}

/**
 * @brief GvsSolCylinder::Print
 * @param fptr
 */
void  GvsSolCylinder :: Print( FILE* fptr ) {
    fprintf(fptr,"SolCylinder {\n");
    fprintf(fptr,"\tbase  "); mBasePoint.printS(fptr);
    fprintf(fptr,"\ttop   "); mTopPoint.printS(fptr);
    fprintf(fptr,"\tradii "); mRadii.printS(fptr);
    fprintf(fptr,"Shader:\n");
    if (surfShader!=NULL) {
        surfShader->Print(fptr);
    }
    fprintf(fptr,"objType: %s\n",(mObjType==local)?"local":"inCoords");
    fprintf(fptr,"}\n");
}

/**
 * @brief GvsSolCylinder::calcBoundBox
 */
void GvsSolCylinder :: calcBoundBox () {
    volBoundBox = GvsBoundBox( volTransfMat * m4d::vec3( -1.0, -1.0, 0.0 ),
                               volTransfMat * m4d::vec3( -1.0,  1.0, 0.0 )  );

    volBoundBox.extendBoxToContain( volTransfMat * m4d::vec3(  1.0, -1.0, 0.0 ) );
    volBoundBox.extendBoxToContain( volTransfMat * m4d::vec3(  1.0,  1.0, 0.0 ) );
    volBoundBox.extendBoxToContain( volTransfMat * m4d::vec3( -1.0, -1.0, 1.0 ) );
    volBoundBox.extendBoxToContain( volTransfMat * m4d::vec3( -1.0,  1.0, 1.0 ) );
    volBoundBox.extendBoxToContain( volTransfMat * m4d::vec3(  1.0, -1.0, 1.0 ) );
    volBoundBox.extendBoxToContain( volTransfMat * m4d::vec3(  1.0,  1.0, 1.0 ) );
}

/**
 * @brief GvsSolCylinder::PtInsideCylinder
 * @param pt
 * @return
 */
bool  GvsSolCylinder :: PtInsideCylinder( const m4d::vec3& pt ) const {
    m4d::vec3 localPt = volInvTransfMat * pt;
    return  (localPt.x(2) >= 0.0) &&
            (localPt.x(2) <= 1.0) &&
            (localPt.x(0) * localPt.x(0) + localPt.x(1) * localPt.x(1) <= 1.0);
}


//-------------------------------------------------------------------------
//   method     GvsSol4DEllipsoid :: g e t T e n t r y T e x i t 4 D
//-------------------------------------------------------------------------
//   equation of cylinder:  x^2 + y^2 - 1 = 0
//   ray:      x = x1*t + x0
//             y = y1*t + y0
//   intersec: a*t^2 + b*t + c = 0
//       with: a = x1*x1 + y1*y1,
//             b = 2*(x1*x0 + y1*y0),
//             c = x0*x0 + y0*y0 - 1
//   solution: t = (-b +-sqrt(b*b - 4*a*c)) / (2a)
//               = (-b/2 +-sqrt((b/2)^2 - ac)) / a
//-------------------------------------------------------------------------
bool 
GvsSolCylinder :: getTentryTexit ( const m4d::vec3& p0, const m4d::vec3& p1, double tp0, double tp1,
                                   double& time_Entry, double& time_Exit,
                                   short& entryFace, short& exitFace ) const
{
    double delta_tp = tp1-tp0;
    double mx = (p1.x(0) - p0.x(0)) / delta_tp;
    double my = (p1.x(1) - p0.x(1)) / delta_tp;
    double mz = (p1.x(2) - p0.x(2)) / delta_tp;
    double ax = p0.x(0) - mx*tp0;
    double ay = p0.x(1) - my*tp0;
    double az = p0.x(2) - mz*tp0;

    double a = mx*mx + my*my;
    double b = 2.0*(ax*mx + ay*my);
    double c = ax*ax + ay*ay - 1.0;
    double d = b*b - (4.0*a*c);

    double zEntry, zExit;


    if( d < 0.0 || a == 0.0 )  {
        if( c > 0.0 || mz == 0.0 ) return false;

        time_Entry = - az / mz;
        time_Exit = (1.0 - az) / mz;

        //if( time_Entry-tp0 < GVS_EPS && time_Exit-tp0 < GVS_EPS ) return false;

        if (tp0 < tp1) {
            if(time_Entry > time_Exit) {
                double t = time_Entry;
                time_Entry = time_Exit;
                time_Exit  = t;
                entryFace = CYL_TOP;
                exitFace = CYL_BOTT;
            }
            else {
                entryFace = CYL_BOTT;
                exitFace = CYL_TOP;
            }
        }
        else  {
            if(time_Entry < time_Exit) {
                double t = time_Entry;
                time_Entry = time_Exit;
                time_Exit  = t;
                entryFace = CYL_TOP;
                exitFace = CYL_BOTT;
            }
            else {
                entryFace = CYL_BOTT;
                exitFace = CYL_TOP;
            }
        }
    }
    else  {
        entryFace = exitFace = CYL_SIDE;

        d = sqrt( d );
        time_Entry = (- b - d) / (2.0 * a);
        time_Exit = (- b + d) / (2.0 * a);
        if (tp0 < tp1) {
            if(time_Entry > time_Exit) {
                double t = time_Entry;
                time_Entry = time_Exit;
                time_Exit  = t;
            }
        }
        else {
            if(time_Entry < time_Exit) {
                double t = time_Entry;
                time_Entry = time_Exit;
                time_Exit  = t;
            }
        }

        zEntry = az + time_Entry * mz;
        zExit = az + time_Exit * mz;

        if( zEntry < 0.0 && zExit < 0.0 ) return false;
        if( zEntry > 1.0 && zExit > 1.0 ) return false;

        if( zEntry > 1.0 )  {
            entryFace = CYL_TOP;
            time_Entry = (1.0 - az) / mz;
        }
        else if( zEntry < 0.0 )  {
            entryFace = CYL_BOTT;
            time_Entry = -az / mz;
        }

        if( zExit > 1.0 )  {
            exitFace = CYL_TOP;
            time_Exit = (1.0 - az) / mz;
        }
        else if( zExit < 0.0 )  {
            exitFace = CYL_BOTT;
            time_Exit = -az / mz;
        }
    }

    return true;
}

/**
 * @brief GvsSolCylinder::getRaySpanList
 * @param ray
 * @param spanList
 * @return
 */
bool GvsSolCylinder :: getRaySpanList (GvsRay& ray, GvsSolObjSpanList& spanList) {
    double time_Entry, time_Exit;
    double tEntry, tExit;
    short  rayEntryFace, rayExitFace;
    int chart0,chart1;

    bool entryFound = false;
    bool exitFound  = false;

    GvsSurfIntersec insecEntry, insecExit;

    m4d::Metric* stMetric = mMetric;
    m4d::enum_coordinate_type coords = stMetric->getCoordType();

    int maxSeg   = ray.getNumPoints()-2;
    int startSeg = int(0);
    int endSeg   = maxSeg;

    for( int seg = startSeg; seg < endSeg; seg++ ) {
        m4d::vec4 p0 = ray.getPoint(seg);
        m4d::vec4 p1 = ray.getPoint(seg+1);

        m4d::vec4 p0trans4D = p0;
        m4d::vec4 p1trans4D = p1;

        chart0 = chart1 = 0;
        if (coords!=m4d::enum_coordinate_cartesian) {
            chart0 = stMetric->transToPseudoCart( p0, p0trans4D );
            chart1 = stMetric->transToPseudoCart( p1, p1trans4D );
        }

        if (chart0!=mChart || chart1!=mChart) {   // replace 'and' by 'or' ?!
            continue;
        }

        if (haveMotion) {
            m4d::vec4 p0motTrans4D = p0trans4D;
            m4d::vec4 p1motTrans4D = p1trans4D;
            stMotion->getTransformedPolygon(seg,p0motTrans4D,p1motTrans4D, p0trans4D, p1trans4D);
        }
        m4d::vec4 vtrans4D = p1trans4D - p0trans4D;

        m4d::vec3 p0trans,p1trans;
        if (mHaveSetParamTransfMat) {
            p0trans = volParamInvTransfMat * p0trans4D.getAsV3D();
            p1trans = volParamInvTransfMat * p1trans4D.getAsV3D();
        }
        else {
            p0trans = volInvTransfMat * p0trans4D.getAsV3D();
            p1trans = volInvTransfMat * p1trans4D.getAsV3D();
        }
        m4d::vec3 vtrans  = p1trans - p0trans;

        double tp0 = p0trans4D.x(0);
        double tp1 = p1trans4D.x(0);
        if (!getTentryTexit( p0trans, p1trans, tp0, tp1, time_Entry, time_Exit,
                             rayEntryFace, rayExitFace ) ) {
            continue;
        }

        tEntry = (time_Entry - tp0) / (tp1 - tp0);
        tExit  = (time_Exit  - tp0) / (tp1 - tp0);

        if (GvsRay::isIn(seg,tEntry,maxSeg) && ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tEntry))) {
            insecEntry.setDist( GvsRay::calcRayDist(seg,tEntry));
            insecEntry.setSurface( this );
            insecEntry.setSubType( GvsSurfIntersec::Entering );

            m4d::vec4 point = p0trans4D + tEntry * vtrans4D;
            m4d::vec4 dir   = vtrans4D;
            //m4d::enum_coordinate_type cType = mMetric->getCoordType();
            m4d::TransCoordinates::coordTransf(m4d::enum_coordinate_cartesian,point,dir,coords,point,dir);
            insecEntry.setPoint( point );
            insecEntry.setDirection( dir );

            insecEntry.setLocalPoint( p0trans + tEntry*vtrans );
            insecEntry.setLocalDirection( vtrans );

            insecEntry.partIndex = rayEntryFace;
            insecEntry.setRaySegNumber(seg);
            entryFound = true;
        }

        if (GvsRay::isIn(seg,tExit,maxSeg) && ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tExit))) {
            insecExit.setDist(GvsRay::calcRayDist(seg,tExit));
            insecExit.setSurface( this );
            insecExit.setSubType( GvsSurfIntersec::Exiting );

            m4d::vec4 point = p0trans4D + tExit * vtrans4D;
            m4d::vec4 dir   = vtrans4D;
           // m4d::enum_coordinate_type cType = mMetric->getCoordType();
            m4d::TransCoordinates::coordTransf(m4d::enum_coordinate_cartesian,point,dir,coords,point,dir);
            insecExit.setPoint( point );
            insecExit.setDirection( dir );

            insecExit.setLocalPoint( p0trans + tExit*vtrans );
            insecExit.setLocalDirection( vtrans );

            insecExit.partIndex = rayExitFace;
            insecExit.setRaySegNumber(seg);
            exitFound = true;
        }

        if( entryFound && exitFound ) {
            spanList.insert(insecEntry, insecExit);
            //insecEntry.localPoint().printS();
            //insecExit.localPoint().printS();
            //std::cerr << "\n";
            return true;
        }
    }

    // TO_BE_TESTED !
    // Fall-back for e.g. Morris-Thorne if exit cannot be found
    // because light ray enters an other chart
    if (entryFound && !exitFound) {
        //fprintf(stderr,"hoppala\n");
        insecExit.setDist( 1e30 );
        spanList.insert(insecEntry,insecExit);
        return true;
    }
    return false;  // no intersection
}


void GvsSolCylinder :: calcNormal( GvsSurfIntersec & intersec ) const
{
    m4d::vec3 localNormal;
    int intersecFace = intersec.partIndex;

    switch( intersecFace ) {
        case CYL_BOTT: {
            localNormal = m4d::vec3(0.0, 0.0, -1.0);
            break;
        }
        case CYL_TOP: {
            localNormal = m4d::vec3(0.0, 0.0, 1.0);
            break;
        }
        case CYL_SIDE: {
            localNormal = m4d::vec3(intersec.localPoint().x(0),
                                    intersec.localPoint().x(1),
                                    0.0);
            break;
        }
    }

    m4d::vec3 globalNormal = transposeMult( volInvTransfMat, localNormal );
    //globalNormal.print();
    //intersec.setNormal( globalNormal.getNormalized() );
    intersec.setNormal( localNormal.getNormalized() );
}


//-------------------------------------------------------------------------
//   method    GvsSolCylinder :: c a l c T e x U V P a r a m
//-------------------------------------------------------------------------
void GvsSolCylinder :: calcTexUVParam( GvsSurfIntersec & intersec ) const
{
    if ( (fabs(intersec.localPoint().x(2) - 0.0) < 1e-6) ||
         (fabs(intersec.localPoint().x(2) - 1.0) < 1e-6))
    {
        double uParam = (intersec.localPoint().x(0) + 1.0) / 2.0;
        double vParam = (intersec.localPoint().x(1) + 1.0) / 2.0;
        double x = intersec.localPoint().x(0);
        double y = intersec.localPoint().x(1);
        vParam = sqrt(x*x+y*y);
        uParam = (atan2(y,x)+M_PI)/(2.0*M_PI);
        intersec.setTexUVParam( uParam, vParam );
    }
    else
    {
        double uParam = (intersec.localPoint().x(1) < 0.0) ?
                    (1.0 - acos(intersec.localPoint().x(0)) / (2.0 * M_PI)) :
                    (acos(intersec.localPoint().x(0)) / (2.0 * M_PI));
        intersec.setTexUVParam( uParam, intersec.localPoint().x(2) );
    }
}

void GvsSolCylinder :: calcDerivatives( GvsSurfIntersec & intersec ) const
{
    // noch nicht getestet

    m4d::vec3 Dv = volTransfMat * m4d::vec3(0,0,1);
    intersec.setDerivT( Dv.getNormalized() );

    m4d::vec3 Du = (intersec.derivT() ^ intersec.normal());
    intersec.setDerivS( Du.getNormalized() );
}

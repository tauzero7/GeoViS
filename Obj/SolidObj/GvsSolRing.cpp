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

#include "Obj/SolidObj/GvsSolRing.h"
#include "Obj/SolidObj/GvsSolObjSpanList.h"
#include "Ray/GvsRay.h"
#include "Ray/GvsSurfIntersec.h"
#include "Shader/Surface/GvsSurfaceShader.h"

#include <metric/m4dMetric.h>
#include "math/TransfMat.h"


#define CYL_BOTT  0
#define CYL_TOP   1
#define CYL_SIDE  2


GvsSolRing::GvsSolRing( const m4d::vec3&  basePt,
                        const m4d::vec3&  topPt,
                        const m4d::vec2&  radii,
                        const m4d::vec2&  innerRadii,
                        GvsSurfaceShader* shader,
                        m4d::Metric*      metric,
                        GvsObjType  objType )
    : GvsSolConvexPrim( shader ),
    isCylinder(false)
{
    mObjType = objType;
    mMetric = metric;
    setGeometry(basePt,topPt,radii,innerRadii);
}

GvsSolRing::GvsSolRing ( const m4d::vec3&  basePt,
                         const m4d::vec3&  topPt,
                         const m4d::vec2&  radii,
                         const m4d::vec2&  innerRadii,
                         GvsSurfaceShader* shader,
                         m4d::Metric*      metric,
                         GvsStMotion*      motion,
                         GvsObjType objType ) 
    : GvsSolConvexPrim( shader ),
    isCylinder(false)
{
    mObjType = objType;
    mMetric = metric;
    stMotion = motion;
    setGeometry(basePt,topPt,radii,innerRadii);
}

/**
 * @brief GvsSolCylinder::setGeometry
 * @param basePt
 * @param topPt
 * @param radii
 */
void GvsSolRing :: setGeometry( const m4d::vec3& basePt, const m4d::vec3& topPt, 
                                const m4d::vec2& radii, const m4d::vec2& innerRadii )
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

    if (fabs(innerRadii.x(0)) < GVS_EPS && fabs(innerRadii.x(1)) < GVS_EPS) {
        isCylinder = true;
    } else {
        volTransfMatInner = m4d::TranslateMat3D ( basePt ) *
            m4d::RotateMat3D ( m4d::axis_Z, alpha ) *
            m4d::RotateMat3D ( m4d::axis_Y, beta ) *
            m4d::ScaleMat3D  ( innerRadii.x(0), innerRadii.x(1), cylAxis.getNorm() );

        volInvTransfMatInner = m4d::ScaleMat3D( 1.0/innerRadii.x(0), 1.0/innerRadii.x(1),
                                       1.0/cylAxis.getNorm()     ) *
            m4d::RotateMat3D    ( m4d::axis_Y, -beta ) *
            m4d::RotateMat3D    ( m4d::axis_Z, -alpha ) *
            m4d::TranslateMat3D ( -basePt );
    }

    mBasePoint = basePt;
    mTopPoint  = topPt;
    mRadii     = radii;
    mInnerRadii = innerRadii;
    calcBoundBox();
}

/**
 * @brief GvsSolRing::setBasePoint
 * @param basePt
 */
void GvsSolRing :: setBasePoint( const m4d::vec3& basePt ) {
    mBasePoint = basePt;
}

/**
 * @brief GvsSolRing::setTopPoint
 * @param topPt
 */
void GvsSolRing :: setTopPoint( const m4d::vec3& topPt  ) {
    mTopPoint  = topPt;
}

/**
 * @brief GvsSolRing::setRadii
 * @param radii
 */
void GvsSolRing :: setRadii( const m4d::vec2& radii, const m4d::vec2& innerRadii ) {
    mRadii = radii;
    mInnerRadii = innerRadii;
}

/**
 * @brief GvsSolRing::Print
 * @param fptr
 */
void  GvsSolRing :: Print( FILE* fptr ) {
    fprintf(fptr,"SolRing {\n");
    fprintf(fptr,"\tbase       "); mBasePoint.printS(fptr);
    fprintf(fptr,"\ttop        "); mTopPoint.printS(fptr);
    fprintf(fptr,"\tradii      "); mRadii.printS(fptr);
    fprintf(fptr,"\tinnerRadii "); mInnerRadii.printS(fptr);
    fprintf(fptr,"Shader:\n");
    if (surfShader!=NULL) {
        surfShader->Print(fptr);
    }
    fprintf(fptr,"objType: %s\n",(mObjType==local)?"local":"inCoords");
    fprintf(fptr,"}\n");
}

/**
 * @brief GvsSolRing::calcBoundBox
 */
void GvsSolRing :: calcBoundBox () {
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
 * @brief GvsSolRing::PtInsideRing
 * @param pt
 * @return
 */
bool GvsSolRing::PtInsideRing( const m4d::vec3& pt ) const {
    m4d::vec3 localPt = volInvTransfMat * pt;
    return  (localPt.x(2) >= 0.0) &&
            (localPt.x(2) <= 1.0) &&
            (localPt.x(0) * localPt.x(0) + localPt.x(1) * localPt.x(1) <= 1.0);
}

/**
 *  @brief GvsSolRing::getTentryTexit
 */
bool GvsSolRing::getTentryTexit ( const m4d::vec3& p0, const m4d::vec3& p1, double tp0, double tp1,
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
 * @brief GvsSolRing::getRaySpanList
 *       This method is not valid for the SolidRing object.
 *       Hence, the SolidRing object cannot be used for CSG objects.
 * 
 * @param ray
 * @param spanList
 * @return
 */
bool GvsSolRing :: getRaySpanList( GvsRay& ray, GvsSolObjSpanList& spanList ) {
    return false; 
}

int GvsSolRing::SetParam( std::string pName, m4d::Matrix<double,3,4> mat ) {
    int isOkay = GvsBase::SetParam(pName,mat);
    if (isOkay >= gvsSetParamNone && getLowCase(pName)=="transform") {
        
        volParamTransfMat = mat * volTransfMat;
        volParamInvTransfMat = volParamTransfMat;
        volParamInvTransfMat.invert();
        
        volParamTransfMatInner = mat * volTransfMatInner;
        volParamInvTransfMatInner = volParamTransfMatInner;
        volParamInvTransfMatInner.invert();
        
        mHaveSetParamTransfMat = true;
    }
    return isOkay;
}

/**
 * 
 */
bool GvsSolRing::testIntersection ( GvsRay &ray ) {
    assert( mMetric != NULL );

    double time_Entry, time_Exit, time_EntryInner, time_ExitInner;
    double tEntry, tExit, tEntryInner, tExitInner;
    short entryFace, exitFace, entryFaceInner, exitFaceInner;
    int chart0,chart1;
    bool validEntry  = true;
    bool validExit   = true;
    bool validEntryInner = true;
    bool validExitInner  = true;

    m4d::enum_coordinate_type  coords = mMetric->getCoordType();

    int maxSeg   = ray.getNumPoints()-2;
    int startSeg = int(0);
    int endSeg   = maxSeg;

    // --- loop over all segments of the ray
    for (int seg = startSeg; seg < endSeg; seg++) {
        validEntry = validExit = validEntryInner = validExitInner = true; 
        m4d::vec4 p0 = ray.getPoint(seg);
        m4d::vec4 p1 = ray.getPoint(seg+1);

        m4d::vec4 p0trans4D = p0;
        m4d::vec4 p1trans4D = p1;

        chart0 = chart1 = 0;
        if (coords != m4d::enum_coordinate_cartesian)  {
            chart0 = mMetric->transToPseudoCart( p0, p0trans4D );
            chart1 = mMetric->transToPseudoCart( p1, p1trans4D );
        }

        if (chart0!=mChart || chart1!=mChart) {
            continue;
        }

        if (haveMotion) {
            m4d::vec4 p0motTrans4D = p0trans4D;
            m4d::vec4 p1motTrans4D = p1trans4D;
            stMotion->getTransformedPolygon(seg,p0motTrans4D,p1motTrans4D, p0trans4D, p1trans4D);
        }
        m4d::vec4 vtrans4D  = p1trans4D - p0trans4D;

        m4d::vec3 p0trans,p1trans,p0transInner,p1transInner;
        if (mHaveSetParamTransfMat) {
            p0trans = volParamInvTransfMat * p0trans4D.getAsV3D();
            p1trans = volParamInvTransfMat * p1trans4D.getAsV3D();
            p0transInner = volParamInvTransfMatInner * p0trans4D.getAsV3D();
            p1transInner = volParamInvTransfMatInner * p1trans4D.getAsV3D();
        }
        else {
            p0trans = volInvTransfMat * p0trans4D.getAsV3D();
            p1trans = volInvTransfMat * p1trans4D.getAsV3D();
            p0transInner = volInvTransfMatInner * p0trans4D.getAsV3D();
            p1transInner = volInvTransfMatInner * p1trans4D.getAsV3D();
        }

        m4d::vec3 vtrans = p1trans - p0trans;
        m4d::vec3 vtransInner = p1transInner - p0transInner;
        
        double tp0 = p0trans4D.x(0);
        double tp1 = p1trans4D.x(0);
    
        if (!getTentryTexit( p0trans,p1trans, tp0,tp1, time_Entry, time_Exit, entryFace, exitFace) ) {
            continue;
        }
        tEntry = (time_Entry - tp0) / (tp1 - tp0);
        tExit  = (time_Exit  - tp0) / (tp1 - tp0);

        if (isCylinder || 
            !getTentryTexit( p0transInner, p1transInner, tp0, tp1, time_EntryInner, time_ExitInner, entryFaceInner, exitFaceInner )) {
            time_EntryInner = time_ExitInner = -FLT_MAX;
            validEntryInner = validExitInner = false;
        }
        tEntryInner = (time_EntryInner - tp0) / (tp1 - tp0);
        tExitInner  = (time_ExitInner  - tp0) / (tp1 - tp0);


        if (fabs(tEntry-tEntryInner) < GVS_EPS)   validEntry = validEntryInner = false; 
        if (fabs(tEntry-tExitInner) < GVS_EPS)    validEntry = validExitInner  = false;
        if (fabs(tExit-tEntryInner) < GVS_EPS)    validExit  = validEntryInner = false;
        if (fabs(tExit-tExitInner) < GVS_EPS)     validExit  = validExitInner  = false;
        

        if (validEntry && 
            GvsRay::isIn(seg,tEntry,maxSeg) && 
            ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tEntry))) {
            //std::cerr << "Schnitt tEntry: " << tEntry << std::endl;

            GvsSurfIntersec surfIntersec;
            surfIntersec.setDist ( GvsRay::calcRayDist(seg,tEntry) );
            surfIntersec.setSurface ( this );

            // global intersection point and direction in proper metric coordinates
            m4d::vec4 point = p0trans4D + tEntry * vtrans4D;
            m4d::vec4 dir   = vtrans4D;
            //m4d::enum_coordinate_type cType = mMetric->getCoordType();
            m4d::TransCoordinates::coordTransf(m4d::enum_coordinate_cartesian,point,dir,coords,point,dir);
            surfIntersec.setPoint( point );
            surfIntersec.setDirection( dir );

            // local intersection point in standard object system
            surfIntersec.setLocalPoint( p0trans + tEntry * vtrans );
            surfIntersec.setLocalDirection( vtrans );

            surfIntersec.partIndex = entryFace;
            surfIntersec.setRaySegNumber(seg);
            if (ray.store(surfIntersec) == GvsRayStatus::finished) {
                return true;
            }
        }
        else if (validExitInner && 
            GvsRay::isIn(seg,tExitInner,maxSeg) && 
            ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tExitInner))) {
  
            GvsSurfIntersec surfIntersec;
            surfIntersec.setDist ( GvsRay::calcRayDist(seg,tExitInner) );
            surfIntersec.setSurface ( this );

            // global intersection point and direction in proper metric coordinates
            m4d::vec4 point = p0trans4D + tExitInner * vtrans4D;
            m4d::vec4 dir   = vtrans4D;
            //m4d::enum_coordinate_type cType = mMetric->getCoordType();
            m4d::TransCoordinates::coordTransf(m4d::enum_coordinate_cartesian,point,dir,coords,point,dir);
            surfIntersec.setPoint( point );
            surfIntersec.setDirection( dir );

            // local intersection point in standard object system
            surfIntersec.setLocalPoint( p0transInner + tExitInner * vtransInner );
            surfIntersec.setLocalDirection( vtransInner );

            surfIntersec.partIndex = exitFaceInner;
            surfIntersec.setRaySegNumber(seg);
            if (ray.store(surfIntersec) == GvsRayStatus::finished) {
                return true;
            }
        }
        else if (validExit && 
            GvsRay::isIn(seg,tExit,maxSeg) && 
            ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tExit))) {
            //std::cerr << "Exit-Schnitt: " << tExit << std::endl;

            GvsSurfIntersec surfIntersec;
            surfIntersec.setDist ( GvsRay::calcRayDist(seg,tExit) );
            surfIntersec.setSurface ( this );

            // global intersection point and direction in proper metric coordinates
            m4d::vec4 point = p0trans4D + tExit * vtrans4D;
            m4d::vec4 dir   = vtrans4D;
            //m4d::enum_coordinate_type cType = mMetric->getCoordType();
            m4d::TransCoordinates::coordTransf(m4d::enum_coordinate_cartesian,point,dir,coords,point,dir);
            surfIntersec.setPoint( point );
            surfIntersec.setDirection( dir );

            // local intersection point in standard object system
            surfIntersec.setLocalPoint( p0trans + tExit * vtrans );
            surfIntersec.setLocalDirection( vtrans );

            surfIntersec.partIndex = exitFace;
            surfIntersec.setRaySegNumber(seg);
            if (ray.store(surfIntersec) == GvsRayStatus::finished) {
                return true;
            }
        }
        else if (validEntryInner && 
            GvsRay::isIn(seg,tEntryInner,maxSeg) && 
            ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tEntryInner))) {
  
            GvsSurfIntersec surfIntersec;
            surfIntersec.setDist ( GvsRay::calcRayDist(seg,tEntryInner) );
            surfIntersec.setSurface ( this );

            // global intersection point and direction in proper metric coordinates
            m4d::vec4 point = p0trans4D + tEntryInner * vtrans4D;
            m4d::vec4 dir   = vtrans4D;
            //m4d::enum_coordinate_type cType = mMetric->getCoordType();
            m4d::TransCoordinates::coordTransf(m4d::enum_coordinate_cartesian,point,dir,coords,point,dir);
            surfIntersec.setPoint( point );
            surfIntersec.setDirection( dir );

            // local intersection point in standard object system
            surfIntersec.setLocalPoint( p0transInner + tEntryInner * vtransInner );
            surfIntersec.setLocalDirection( vtransInner );

            surfIntersec.partIndex = entryFaceInner;
            surfIntersec.setRaySegNumber(seg);
            if (ray.store(surfIntersec) == GvsRayStatus::finished) {
                return true;
            }
        }
    }
    return false;
}

void GvsSolRing::calcNormal( GvsSurfIntersec & intersec ) const {
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

void GvsSolRing::calcTexUVParam( GvsSurfIntersec & intersec ) const {

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

void GvsSolRing::calcDerivatives( GvsSurfIntersec & intersec ) const {
    // not yet tested

    if (mHaveSetParamTransfMat) {    
        m4d::vec3 Dv = volTransfMatInner * m4d::vec3(0,0,1);
        intersec.setDerivT( Dv.getNormalized() );

        m4d::vec3 Du = (intersec.derivT() ^ intersec.normal());
        intersec.setDerivS( Du.getNormalized() );
    } else {
        m4d::vec3 Dv = volTransfMat * m4d::vec3(0,0,1);
        intersec.setDerivT( Dv.getNormalized() );

        m4d::vec3 Du = (intersec.derivT() ^ intersec.normal());
        intersec.setDerivS( Du.getNormalized() );
    }
}

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

#include "GvsSolConvexPrim.h"
#include <Ray/GvsSurfIntersec.h>

#include "math/TransfMat.h"


GvsSolConvexPrim :: GvsSolConvexPrim(GvsSurfaceShader* shader) : GvsSolidObj(shader) {
    volTransfMat.setIdent();
    volInvTransfMat.setIdent();
    volParamTransfMat.setIdent();
    volParamInvTransfMat.setIdent();

    //  std::cerr << "addparam\n";
    AddParam("transform",gvsDT_MAT3D);
    mHaveSetParamTransfMat = false;

    mID = ++mObjCounter;
}


void GvsSolConvexPrim::scale ( const m4d::vec3 &scaleVec) {
    volTransfMat = m4d::ScaleMat3D ( scaleVec ) * volTransfMat;
    volInvTransfMat *= m4d::ScaleMat3D ( 1.0/scaleVec.x(0), 1.0/scaleVec.x(1), 1.0/scaleVec.x(2) );
    calcBoundBox();
}


void GvsSolConvexPrim :: translate ( const m4d::vec3 &transVec) {
    volTransfMat = m4d::TranslateMat3D ( transVec ) * volTransfMat;
    m4d::vec3  minTrans = -transVec;
    volInvTransfMat *= m4d::TranslateMat3D ( minTrans );
    calcBoundBox();
}


void GvsSolConvexPrim :: rotate ( const m4d::vec3 &rotAxis, double rotAngle) {
    volTransfMat = m4d::RotateMat3D ( rotAxis, rotAngle ) * volTransfMat;
    volInvTransfMat *= m4d::RotateMat3D ( rotAxis, -rotAngle);
    calcBoundBox();
}


void GvsSolConvexPrim :: transform ( const m4d::Matrix<double,3,4> &mat) {
    //  std::cerr << "GvsSolConvexPrim :: transform()...\n";
    if ( !mat.isIdentMat() ) {
        volTransfMat = mat * volTransfMat;
        m4d::Matrix<double,3,4> invMat = mat;
        invMat.invert();
        volInvTransfMat = volInvTransfMat * invMat;
        calcBoundBox();
    }
}


GvsBoundBox GvsSolConvexPrim :: boundingBox ( ) const {
    return volBoundBox;
}

/*
bool
GvsSolConvexPrim :: getTentryTexit  ( const m4d::vec3& p0, const m4d::vec3& p1,
                                      double tp0, double tp1,
                                      double& time_Entry, double& time_Exit,
                                      short &entryFace, short &exitFace) const
{
  std::cerr << "GvsSolConvexPrim :: getTentryTexit... not implemented yet.\n";
  return false;
}
*/

bool
GvsSolConvexPrim :: getTentryTexit  ( const m4d::vec3& , const m4d::vec3& ,
                                      double , double ,
                                      double& , double& ,
                                      short &, short &) const
{
    std::cerr << "GvsSolConvexPrim :: getTentryTexit... not implemented yet.\n";
    return false;
}


/**
 * @param ray
 * @return
 */
bool GvsSolConvexPrim::testIntersection ( GvsRay &ray ) {
    assert( mMetric != NULL );

    double time_Entry, time_Exit;
    double tEntry, tExit;
    short entryFace, exitFace;
    int chart0,chart1;

    m4d::enum_coordinate_type  coords = mMetric->getCoordType();

    int maxSeg   = ray.getNumPoints()-2;
    int startSeg = int(0);
    int endSeg   = maxSeg;

    // --- loop over all segments of the ray
    for (int seg = startSeg; seg < endSeg; seg++) {
        m4d::vec4 p0 = ray.getPoint(seg);
        m4d::vec4 p1 = ray.getPoint(seg+1);

        m4d::vec4 p0trans4D = p0;
        m4d::vec4 p1trans4D = p1;

        chart0 = chart1 = 0;
        if (coords != m4d::enum_coordinate_cartesian)  {
            chart0 = mMetric->transToPseudoCart( p0, p0trans4D );
            chart1 = mMetric->transToPseudoCart( p1, p1trans4D );
        }

        if (chart0!=mChart && chart1!=mChart) {
            continue;
        }

        if (haveMotion) {
            m4d::vec4 p0motTrans4D = p0trans4D;
            m4d::vec4 p1motTrans4D = p1trans4D;
            stMotion->getTransformedPolygon(seg,p0motTrans4D,p1motTrans4D, p0trans4D, p1trans4D);
        }
        m4d::vec4 vtrans4D  = p1trans4D - p0trans4D;

        m4d::vec3 p0trans,p1trans;
        if (mHaveSetParamTransfMat) {
            p0trans = volParamInvTransfMat * p0trans4D.getAsV3D();
            p1trans = volParamInvTransfMat * p1trans4D.getAsV3D();
        }
        else {
            p0trans = volInvTransfMat * p0trans4D.getAsV3D();
            p1trans = volInvTransfMat * p1trans4D.getAsV3D();
        }
        m4d::vec3 vtrans = p1trans - p0trans;

        double tp0 = p0trans4D.x(0);
        double tp1 = p1trans4D.x(0);
        if (!getTentryTexit( p0trans,p1trans, tp0,tp1, time_Entry, time_Exit, entryFace, exitFace) ) {
            continue;
        }

        tEntry = (time_Entry - tp0) / (tp1 - tp0);
        tExit  = (time_Exit  - tp0) / (tp1 - tp0);

        if (GvsRay::isIn(seg,tEntry,maxSeg) && ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tEntry))) {
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
            return ray.store( surfIntersec );
        }
        else if (GvsRay::isIn(seg,tExit,maxSeg) && ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tExit))) {
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
            return ray.store( surfIntersec );

        }
    }
    return false;
}


bool GvsSolConvexPrim::testLocalIntersection( GvsRay &ray, const int seg,
                                              GvsLocalTetrad* lt0, GvsLocalTetrad* lt1, 
                                              const m4d::vec4 p0, const m4d::vec4 p1 )
{
    // std::cerr << "GvsSolConvexPrim :: testLocalIntersection\n";

    assert (mObjType==local);

    double time_Entry, time_Exit;
    double tEntry, tExit;
    short entryFace, exitFace;

    double tp0 = p0.x(0);
    double tp1 = p1.x(0);

    m4d::vec3 p0trans,p1trans;

    int maxSeg   = ray.getNumPoints()-2;

    if (stMotion != NULL)
    {
        // std::cerr << "GvsSolConvexPrim :: have motion...\n";
        if (stMotion->getMotionType() == gvsMotionConstVelocity )
        {
            m4d::vec4 p0trans4D;
            m4d::vec4 p1trans4D;
            stMotion->getTransformedPolygon(seg,p0,p1, p0trans4D, p1trans4D);
            p0trans = volInvTransfMat * m4d::vec3(p0trans4D.x(1),p0trans4D.x(2),p0trans4D.x(3));
            p1trans = volInvTransfMat * m4d::vec3(p1trans4D.x(1),p1trans4D.x(2),p1trans4D.x(3));
        }
        else {
            std::cerr << "GvsSolConvexPrim :: testLocalIntersection()...Motion not allowed...\n";
        }
    }
    else {
        if (mHaveSetParamTransfMat) {
            p0trans = volParamInvTransfMat * p0.getAsV3D(); // ::vec3(p0.x(1),p0.x(2),p0.x(3));
            p1trans = volParamInvTransfMat * p1.getAsV3D(); // ::vec3(p1.x(1),p1.x(2),p1.x(3));
        }
        else {
            p0trans = volInvTransfMat * p0.getAsV3D(); // ::vec3(p0.x(1),p0.x(2),p0.x(3));
            p1trans = volInvTransfMat * p1.getAsV3D(); // ::vec3(p1.x(1),p1.x(2),p1.x(3));            
        }
    }

    m4d::vec3 vtrans = p1trans - p0trans;
    if (!getTentryTexit(p0trans,p1trans,
                        tp0,tp1, time_Entry, time_Exit,
                        entryFace, exitFace))
    {
        return false;
    }

    tEntry = (time_Entry - tp0) / (tp1 - tp0);
    tExit  = (time_Exit - tp0) / (tp1 - tp0);

    if (GvsRay::isIn(seg,tEntry,maxSeg) && ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tEntry)))  {
        // std::cerr << "Schnitt tEntry: " << tEntry << std::endl;

        GvsSurfIntersec surfIntersec;
        surfIntersec.setDist( GvsRay::calcRayDist(seg,tEntry) );

        m4d::vec4 vp0 = ray.getPoint(seg);
        m4d::vec4 vp1 = ray.getPoint(seg+1);

        m4d::vec4 vtrans4D = vp1 - vp0; 
        m4d::vec4 point    = vp0 + tEntry * vtrans4D;

        //  stMetric->coordTransf( m4d::enum_coordinate_cartesian,point,stMetric->getCoordType(),point);
        surfIntersec.setPoint   ( point );
        surfIntersec.setSurface ( this );
        surfIntersec.setDirection ( vtrans4D );

        surfIntersec.setLocalIntersec ( true );
        
        GvsLocalTetrad* lt = lt0->getInterpolatedTetrad(lt0,lt1,tEntry);
        surfIntersec.setLocalTetrad( lt );
        surfIntersec.setLocalTime( (1.0-tEntry)*lt0->getLocalTime() + tEntry*lt1->getLocalTime() );
        surfIntersec.setLocalPoint( p0trans + tEntry*vtrans );
        surfIntersec.setLocalDirection ( vtrans );
        surfIntersec.partIndex = entryFace;

        surfIntersec.setRaySegNumber(seg);
        return ray.store( surfIntersec );
    }
    else if (GvsRay::isIn(seg,tExit,maxSeg) && ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tExit)))  {
       // std::cerr << "Schnitt tExit: " << tExit << std::endl;

        GvsSurfIntersec surfIntersec;
        surfIntersec.setDist( GvsRay::calcRayDist(seg,tExit) );

        m4d::vec4 vp0 = ray.getPoint(seg);
        m4d::vec4 vp1 = ray.getPoint(seg+1);

        m4d::vec4 vtrans4D = vp1 - vp0;
        m4d::vec4 point    = vp0 + tExit * vtrans4D;

        //stMetric->coordTransf( m4d::enum_coordinate_cartesian,point,stMetric->getCoordType(),point);
        surfIntersec.setPoint   ( point );
        surfIntersec.setSurface ( this );
        surfIntersec.setDirection ( vtrans4D );

        surfIntersec.setLocalIntersec ( true );

        GvsLocalTetrad* lt = lt0->getInterpolatedTetrad(lt0,lt1,tExit);
        surfIntersec.setLocalTetrad(lt);
        surfIntersec.setLocalTime( (1.0-tExit)*lt0->getLocalTime() + tExit*lt1->getLocalTime() );
        surfIntersec.setLocalPoint( p0trans + tExit*vtrans );
        surfIntersec.setLocalDirection ( vtrans );
        surfIntersec.partIndex = exitFace;

        surfIntersec.setRaySegNumber(seg);
        return ray.store( surfIntersec );
    }
    return false;
}


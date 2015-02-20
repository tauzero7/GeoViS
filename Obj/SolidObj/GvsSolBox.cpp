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

#include "GvsSolBox.h"
#include "GvsSolObjSpanList.h"
#include "Ray/GvsRay.h"
#include "Ray/GvsSurfIntersec.h"
#include "Shader/Surface/GvsSurfaceShader.h"

#include "math/TransfMat.h"
#include "metric/m4dMetric.h"


GvsSolBox :: GvsSolBox(const m4d::vec3 corner0, const m4d::vec3 corner1, 
                       GvsSurfaceShader* shader, m4d::Metric* metric, GvsObjType objType)
    : GvsSolConvexPrim ( shader )
{

    mObjType = objType;

    // checks if it is a real 3d object (not only a 2d-surface)
    m4d::vec3 bsize = (corner1-corner0).getVabs();
    assert( fabs(bsize[0] + bsize[1] + bsize[2]) > 0.0 );

    m4d::vec3 lowLeft ( GVS_MIN2(corner0[0],corner1[0]),
            GVS_MIN2(corner0[1],corner1[1]),
            GVS_MIN2(corner0[2],corner1[2]) );

    volTransfMat    = m4d::TranslateMat3D ( lowLeft ) * m4d::ScaleMat3D ( bsize );
    volInvTransfMat = m4d::ScaleMat3D ( m4d::vec3(1.0/bsize.x(0), 1.0/bsize.x(1), 1.0/bsize.x(2)) )
            * m4d::TranslateMat3D ( -lowLeft );

    calcBoundBox();

    mMetric = metric;
    mCornerLL = m4d::vec3(corner0[0],corner0[1],corner0[2]);
    mCornerUR = m4d::vec3(corner1[0],corner1[1],corner1[2]);
}

GvsSolBox :: GvsSolBox(const m4d::vec3 corner0, const m4d::vec3 corner1,
                       GvsSurfaceShader* shader, m4d::Metric* metric,
                       GvsStMotion* motion, GvsObjType objType )
    : GvsSolConvexPrim( shader )
{
    mObjType = objType;

    m4d::vec3 bsize = (corner1-corner0).getVabs();
    assert( fabs(bsize[0] + bsize[1] + bsize[2]) > 0.0 );

    m4d::vec3 lowLeft ( GVS_MIN2(corner0[0],corner1[0]),
            GVS_MIN2(corner0[1],corner1[1]),
            GVS_MIN2(corner0[2],corner1[2]) );

    volTransfMat    = m4d::TranslateMat3D ( lowLeft ) * m4d::ScaleMat3D ( bsize );
    volInvTransfMat = m4d::ScaleMat3D ( m4d::vec3(1.0/bsize.x(0), 1.0/bsize.x(1), 1.0/bsize.x(2)) )
            * m4d::TranslateMat3D ( -lowLeft );

    calcBoundBox();

    mMetric = metric;
}


void
GvsSolBox :: calcBoundBox ()
{
    volBoundBox = GvsBoundBox (0.0, 0.0, 0.0,  1.0, 1.0, 1.0 );
    volBoundBox.transform ( volTransfMat );
    //    volBoundBox.print(cerr);
}

void GvsSolBox::calcNormal ( GvsSurfIntersec &intersec ) const {
    int boxIntersecFace = intersec.partIndex;

    if ( (boxIntersecFace < 0) || (boxIntersecFace >= 6) )     {
        std::cerr << "Error in GvsSolBox::calcNormal(): Invalid face number." << std::endl;
        return;
    }

    //m4d::vec3 globalNormal = transposeMult( volInvTransfMat, boxNormal[boxIntersecFace] );
    intersec.setNormal( boxNormal[boxIntersecFace].getNormalized() );
}


void
GvsSolBox :: calcTexUVParam ( GvsSurfIntersec &intersec ) const
{
    int boxIntersecFace = intersec.partIndex;
    switch ( boxIntersecFace )
    {
        case 0:  intersec.setTexUVParam (1.0-intersec.localPoint().x(1), intersec.localPoint().x(2));
            break;
        case 1:  intersec.setTexUVParam (intersec.localPoint().x(1), intersec.localPoint().x(2));
            break;
        case 2:  intersec.setTexUVParam (intersec.localPoint().x(0), intersec.localPoint().x(2));
            break;
        case 3:  intersec.setTexUVParam (1.0-intersec.localPoint().x(0), intersec.localPoint().x(2));
            break;
        case 4:  intersec.setTexUVParam (intersec.localPoint().x(0), intersec.localPoint().x(1));
            break;
        case 5:  intersec.setTexUVParam (intersec.localPoint().x(0), intersec.localPoint().x(1));
            break;
        default: std::cerr << "Error in GvsSolBox::calcTexUVParam(): invalid box"
                           << "intersection face (" << boxIntersecFace << "\n";
            intersec.setTexUVParam(0.0,0.0);
            break;
    }
}

bool 
GvsSolBox :: getTentryTexit  ( const m4d::vec3& p0, const m4d::vec3& p1, 
                               double tp0, double tp1,
                               double& time_Entry, double& time_Exit,
                               short &entryFace, short &exitFace) const
{
    return GvsBoundBox(0,0,0,1,1,1).getTentryTexit(p0,p1,
                                                   tp0,tp1, time_Entry, time_Exit,
                                                   entryFace, exitFace);

}

bool GvsSolBox :: testIntersection ( GvsRay &ray ) {
    return GvsSolConvexPrim::testIntersection( ray );
}

bool GvsSolBox :: testLocalIntersection (GvsRay &ray, const int seg,
                                         GvsLocalTetrad* lt0, GvsLocalTetrad* lt1, 
                                         const m4d::vec4 p0, const m4d::vec4 p1 )
{
    // std::cerr << "GvsSolBox :: testLocalIntersection ()...\n";
    return GvsSolConvexPrim :: testLocalIntersection( ray, seg, lt0, lt1, p0, p1 );
}


bool
GvsSolBox :: getRaySpanList ( GvsRay &ray, GvsSolObjSpanList &spanList )
{
    //  std::cerr << "GvsSolBox :: getRaySpanList() called ...\n";
    //*
    double time_Entry, time_Exit;
    double tEntry, tExit;
    short    rayEntryFace, rayExitFace;

    bool entryFound = false;
    bool exitFound  = false;

    GvsSurfIntersec insecEntry, insecExit;

    m4d::enum_coordinate_type coords;

    // mMetric->setActualMetricNr(mChart);
    m4d::Metric* stMetric = mMetric;
    coords = stMetric->getCoordType();

    int maxSeg   = ray.getNumPoints()-2;
    int startSeg = int(0);
    int endSeg   = int(maxSeg);

    for( int seg = startSeg; seg < endSeg; seg++ )
    {
        m4d::vec4 p0 = ray.getPoint(seg);
        m4d::vec4 p1 = ray.getPoint(seg+1);

        double tp0 = p0.x(0);
        double tp1 = p1.x(0);

        m4d::vec4 p0trans4D, p1trans4D;

        p0trans4D = p0;
        p1trans4D = p1;

        if (coords!=m4d::enum_coordinate_cartesian)
        {
            stMetric->transToPseudoCart (  p0,  p0trans4D );
            stMetric->transToPseudoCart (  p1,  p1trans4D );
        }

        //m4d::vec3 p0trans = volInvTransfMat * m4d::vec3(p0trans4D.x(1),p0trans4D.x(2),p0trans4D.x(3));
        //m4d::vec3 p1trans = volInvTransfMat * m4d::vec3(p1trans4D.x(1),p1trans4D.x(2),p1trans4D.x(3));
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

        if (!GvsBoundBox(0,0,0,1,1,1).getTentryTexit(p0trans,p1trans,
                                                     tp0,tp1, time_Entry, time_Exit,
                                                     rayEntryFace, rayExitFace)) {
            continue;
        }

        tEntry = (time_Entry - tp0) / (tp1 - tp0);
        tExit  = (time_Exit - tp0) / (tp1 - tp0);

        if( GvsRay::isIn( seg,tEntry,maxSeg ) && ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tEntry) ))  {
            insecEntry.setDist ( GvsRay::calcRayDist(seg,tEntry) );

            m4d::vec4 dir   = p1trans4D - p0trans4D;
            m4d::vec4 point = p0trans4D + tEntry * dir;
            m4d::enum_coordinate_type cType = mMetric->getCoordType();
            m4d::TransCoordinates::coordTransf(m4d::enum_coordinate_cartesian,point,dir,cType,point,dir);
            insecEntry.setPoint( point );
            insecEntry.setDirection( dir );

            insecEntry.setSurface   ( this );
            insecEntry.setSubType   ( GvsSurfIntersec::Entering );

            insecEntry.setLocalPoint( p0trans + tEntry * vtrans );
            insecEntry.setLocalDirection ( vtrans );

            insecEntry.partIndex = rayEntryFace;
            insecEntry.setRaySegNumber(seg);
            entryFound = true;
        }

        if(  GvsRay::isIn( seg,tExit,maxSeg ) && ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,tExit)))
        {
            insecExit.setDist       ( GvsRay::calcRayDist(seg,tExit) );

            m4d::vec4 dir   = p1trans4D - p0trans4D;
            m4d::vec4 point = p0trans4D + tExit * dir;
            m4d::enum_coordinate_type cType = mMetric->getCoordType();
            m4d::TransCoordinates::coordTransf(m4d::enum_coordinate_cartesian,point,dir,cType,point,dir);
            insecEntry.setPoint( point );
            insecEntry.setDirection( dir );

            insecExit.setSurface    ( this );
            insecExit.setSubType    ( GvsSurfIntersec::Exiting );

            insecExit.setLocalPoint ( p0trans + tExit * vtrans );
            insecExit.setLocalDirection ( vtrans );

            insecExit.partIndex = rayExitFace;
            insecExit.setRaySegNumber(seg);
            exitFound = true;
        }

        if( entryFound && exitFound ) {
            spanList.insert(insecEntry, insecExit);
            return true;
        }
    }
    return false;  // no intersection
}


void GvsSolBox::Print( FILE* fptr ) {
    fprintf(fptr,"SolBox {\n");
    fprintf(fptr,"\tcornerLL: "); mCornerLL.printS(fptr);
    fprintf(fptr,"\tcornerUR: "); mCornerUR.printS(fptr);
    if (surfShader!=NULL) {
        fprintf(fptr,"\tShader {\n");
        surfShader->Print(fptr);
        fprintf(fptr,"\t}\n");
    }
    fprintf(fptr,"\tobjType:  %s\n",(mObjType==local)?"local":"inCoords");
    fprintf(fptr,"}\n");
}

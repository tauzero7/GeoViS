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

#include "Ray/GvsRayGen.h"

GvsRayGen :: GvsRayGen() :
    actualSolver(NULL) {
    maxNumPoints = 2;
}

GvsRayGen :: GvsRayGen( GvsGeodSolver *solver ) :
    actualSolver(solver) {
    actualSolver->setGeodType(m4d::enum_geodesic_lightlike);
    actualSolver->setTimeDir(m4d::enum_time_backward);
    maxNumPoints = 2;
}

GvsRayGen :: GvsRayGen( GvsGeodSolver *solver,
                        m4d::enum_geodesic_type type, m4d::enum_time_direction dir) :
    actualSolver(solver)  {
    actualSolver->setGeodType(type);
    actualSolver->setTimeDir(dir);
    maxNumPoints = 2;
}

GvsRayGen :: ~GvsRayGen() {
    actualSolver = NULL;
}


void GvsRayGen :: setBoundBox ( const GvsBoundBox4D &box ) {
    boundBox = box;
    actualSolver->setBoundingBox(box.lowBounds(),box.uppBounds());
}

GvsBoundBox4D GvsRayGen :: getBoundBox ( ) const {
    return boundBox;
}

void GvsRayGen :: setMaxNumPoints(const int maxPoints) {
    maxNumPoints = maxPoints;
}

int GvsRayGen :: getMaxNumPoints() const {
    return maxNumPoints;
}


void GvsRayGen :: setActualSolver( GvsGeodSolver *solver ) {
    actualSolver = solver;
    actualSolver->setGeodType(m4d::enum_geodesic_lightlike);
    actualSolver->setTimeDir(m4d::enum_time_backward);
}


GvsGeodSolver* GvsRayGen :: getActualSolver ( ) const {
    return actualSolver;
}


m4d::enum_break_condition GvsRayGen::calcPolyline(const m4d::vec4 &startOrig, const m4d::vec4 &startDir,
                                     m4d::vec4*& points, int &numPoints) {
    assert(actualSolver!=NULL);

    m4d::Metric* metric = actualSolver->getMetric();
    if (metric != NULL) {
        if (metric->breakCondition(startOrig) ) {
            std::cerr << "Error in GvsRayGenSimple :: calcPolyline" << std::endl;
            std::cerr << "StartPos already satisfies breakCondition" << std::endl;
            points = NULL;
            return m4d::enum_break_other;
        }
    }

    m4d::vec4* dirs = NULL;
    m4d::enum_break_condition bc = actualSolver->calculateGeodesic(startOrig,startDir,maxNumPoints,points,dirs,numPoints);
    delete [] dirs;
    return bc;
}


m4d::enum_break_condition
GvsRayGen :: calcPolyline(const m4d::vec4 &startOrig, const m4d::vec4 &startDir,
                          m4d::vec4*& points, m4d::vec4 *&dirs, int &numPoints)
{
    // startOrig and startDir have to be given in coordinates !!    
    assert(actualSolver!=NULL);

    m4d::Metric* metric = actualSolver->getMetric();
    if ( metric!=NULL) {
        if (metric->breakCondition(startOrig) ) {
            std::cerr << "error in GvsRayGenSimple :: calcPolyline" << std::endl;
            std::cerr << "StartPos already satisfies breakCondition" << std::endl;
            return m4d::enum_break_other;
        }
    }

    return actualSolver->calculateGeodesic(startOrig,startDir,maxNumPoints,points,dirs,numPoints);
}


//----------------------------------------------------------------------------
//         calcParTransport
//----------------------------------------------------------------------------
//  Neben der Geodaetenintegration wird noch zusaetzlich die lokale Tetrade
//  des Beobachters bis zum Emissionszeitpunkt parallel-transportiert.
//
GvsLocalTetrad*
GvsRayGen :: calcParTransport ( const m4d::vec4 &startOrig, const m4d::vec4 &startDir, const GvsLocalTetrad *lt,
                                int &numPoints, m4d::enum_break_condition &bc )
{
    //  std::cerr << "GvsRayGenSimple :: calcParTransport () ...\n";
    assert(actualSolver!=NULL);

    m4d::Metric* metric = actualSolver->getMetric();
    if ( metric!=NULL) {
        if (metric->breakCondition(startOrig) ) {
            std::cerr << "error in GvsRayGenSimple :: calcPolyline" << std::endl;
            std::cerr << "StartPos already satisfies breakCondition" << std::endl;
            bc = m4d::enum_break_other;
            return NULL;
        }
    }

    m4d::vec4 base[4]; // = new m4d::vec4[4];
    for (int i=0; i<4; i++) {
        base[i] = lt->getE(i);
    }

    GvsLocalTetrad* localT = NULL;
    bc = actualSolver->calcParTransport(startOrig,startDir,base,maxNumPoints,localT,numPoints);
    return localT;
}


bool  GvsRayGen::calcSachsJacobi ( const m4d::vec4 &startOrig, const m4d::vec4 &startDir, const m4d::vec3 &localDir,
                                   const GvsLocalTetrad* lt,
                                   m4d::vec4 *&points, m4d::vec4 *&dirs, double *&lambda,
                                   m4d::vec4 *&sachs1, m4d::vec4 *&sachs2,
                                   m4d::vec5 *&rayJacobi, m4d::vec5 &rayMaxJacobi,
                                   int &numPoints, m4d::enum_break_condition &bc ) {
    assert(actualSolver!=NULL);

    m4d::Metric* metric = actualSolver->getMetric();
    if ( metric!=NULL) {
        if (metric->breakCondition(startOrig) ) {
            std::cerr << "Error in GvsRayGenSimple :: calcPolyline" << std::endl;
            std::cerr << "StartPos already satisfies breakCondition" << std::endl;
            bc = m4d::enum_break_other;
            return false;
        }
    }
    bc = actualSolver->calcSachsJacobi(startOrig,startDir,localDir,maxNumPoints,
                                       lt,points,dirs,lambda,sachs1,sachs2,rayJacobi,rayMaxJacobi,numPoints);
    return true;
}



void GvsRayGen::Print( FILE* fptr )
{
    fprintf(fptr,"RayGen {\n");
    fprintf(fptr,"\tmaxNumPoints: %d",maxNumPoints);
    fprintf(fptr,"}\n");
}

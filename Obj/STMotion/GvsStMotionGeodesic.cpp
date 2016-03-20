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

#include "Obj/STMotion/GvsStMotionGeodesic.h"


GvsStMotionGeodesic::GvsStMotionGeodesic()
    : GvsStMotion()
{
    mMetric = NULL;
    mSolver    = NULL;
    mType      = gvsMotionGeodesic;
    mGeodType  = m4d::enum_geodesic_timelike;

    lambdaStep = 0.01;
    maxNumPoints = 10000;
}

GvsStMotionGeodesic::GvsStMotionGeodesic(m4d::Metric* metric,  GvsGeodSolver* solver)
    : GvsStMotion()
{
    //    std::cerr << "Initialize GvsStMotionGeodesic..." << std::endl;
    mMetric = metric;
    mSolver    = solver;
    mType      = gvsMotionGeodesic;
    mGeodType  = m4d::enum_geodesic_timelike;

    lambdaStep = 0.01;
    maxNumPoints = 10000;

    if (!localTetrad.empty()) deleteAllEntries();

    mSolver->setGeodType( m4d::enum_geodesic_timelike );
    mSolver->setTimeDir( m4d::enum_time_forward );
}

GvsStMotionGeodesic::~GvsStMotionGeodesic()
{
}


void
GvsStMotionGeodesic :: setStartLT ( GvsLocalTetrad *lt )
{
    localTetrad.push_back(lt);
    numPositions = (int)localTetrad.size();
}


//TODO: must be checked !!
void GvsStMotionGeodesic :: setStartVal( const m4d::vec4 &pos, m4d::vec4 &vel, bool inCoords, m4d::enum_nat_tetrad_type lfType ) {
    assert (numPositions==0);

    m4d::vec4 coordVel = vel;

    // If the velocity is not given in coordinates, it must be converted.
    // The transformation depends on the local tetrad.
    if (!inCoords) {
        // velocity is given wrt. local tetrad.
        double vq = vel.x(1)*vel.x(1) + vel.x(2)*vel.x(2) + vel.x(3)*vel.x(3);
        assert ( vq < 1.0 );// Pruefe ob v<c
        double gamma = 1.0/sqrt(1.0-vq);

        // local velocity: u_loc = gamma * (e0 + v1*e1 + v2*e2 + v3*e3)
        m4d::vec4 u_loc = m4d::vec4(gamma,gamma*vel.x(1),gamma*vel.x(2),gamma*vel.x(3));
        mMetric->localToCoord(pos,u_loc,coordVel,lfType);
        //mMetric->print();
    }
    else {
        assert(mSolver!=NULL);
        mSolver->startCondition(&pos,coordVel);
    }

    //cerr << "coordvel:\n";
    //coordVel.print(cerr);


    m4d::Metric* metric = mMetric;
    GvsLocalTetrad* lt = new GvsLocalTetrad(metric,pos,coordVel);
    lt->adjustTetrad();
    localTetrad.push_back(lt);
    numPositions = (int)localTetrad.size();

    //setStartTetrad();
}


void
GvsStMotionGeodesic :: setStartTetrad ( )
{
    //  std::cerr << "GvsStMotionGeodesic :: setStartTetrad()...\n";
    assert (localTetrad[0]!=NULL);
    m4d::vec4 e0 = localTetrad[0]->getE(0);
    m4d::vec4 e1 = localTetrad[0]->getE(1);
    m4d::vec4 e2 = localTetrad[0]->getE(2);
    m4d::vec4 e3 = localTetrad[0]->getE(3);
    setStartTetrad(e0,e1,e2,e3,true);
}

void
GvsStMotionGeodesic :: setStartTetrad ( const m4d::vec4 &e0, const m4d::vec4 &e1, const m4d::vec4 &e2, const m4d::vec4 &e3,
                                        bool inCoords, m4d::enum_nat_tetrad_type lfType  )
{
    //  std::cerr << "GvsStMotionGeodesic :: setStartTetrad( coords )..." << std::endl;
    //  if (inCoords)  std::cerr << "yes\n"; else std::cerr << "no\n";

    // Es muss bereits eine Tetrade mit Startort angegeben sein.
    assert (localTetrad[0]!=NULL);

    m4d::vec4 e0k = e0;
    m4d::vec4 e1k = e1;
    m4d::vec4 e2k = e2;
    m4d::vec4 e3k = e3;
    //e0k.print();
    //e1k.print();
    //e2k.print();
    //e3k.print();
    //  e0 wird zunaechst abhaengig von der Startbewegungsrichtung gesetzt
    //  und liegt in Koordinatendarstellung vor
    e0k = localTetrad[0]->getVelocity();

    // Sind die Tetradenvektoren e1-e3 bzgl einer natuerlichen Tetrade gegeben, so muessen
    // sie noch auf Koordinatendarstellung transformiert werden.
    m4d::Metric* metric = localTetrad[0]->getMetric();
    m4d::vec4 pos = localTetrad[0]->getPosition();

    if (!inCoords)
    {
        // metric->localToCoord(pos,e0,e0k,lfType);
        metric->localToCoord(pos,e1,e1k,lfType);
        metric->localToCoord(pos,e2,e2k,lfType);
        metric->localToCoord(pos,e3,e3k,lfType);
    }
    //e0k.print();
    //e1k.print();
    //e2k.print();
    //e3k.print();


    // anschliessend muss noch orthonormiert werden
    GvsGramSchmidt* gs = new GvsGramSchmidt(metric,pos);
    gs->calculateTetrad(e0k,e1k,e2k,e3k);
    //gs->printS();
    delete gs;

    localTetrad[0]->setTetrad(e0k,e1k,e2k,e3k,true);
    localTetrad[0]->setInCoords(true);
}


void GvsStMotionGeodesic::setLambdaStep ( double h ) {
    lambdaStep = h;
}

void GvsStMotionGeodesic::setMaxNumPoints ( int maxPoints ) {
    maxNumPoints = maxPoints;
}

void GvsStMotionGeodesic::calculateGeodesicMotion(m4d::enum_time_direction geodDir, double deltaT, double tau0, double deltaTau ) {
    //  std::cerr << "GvsStMotionGeodesic::calculateGeodesicMotion\n";
    assert(mMetric != NULL);
    assert(mSolver != NULL);
    assert(maxNumPoints > 2);

    GvsLocalTetrad* locT;

    // If a part of the geodesic is already calculated, the first or last tetrad
    // is the new start tetrad depending in which temporal direction the geodesic
    // is to be continued.
    switch (geodDir)
    {
        case m4d::enum_time_forward: {
            locT = localTetrad[numPositions-1];
            break;
        }
        case m4d::enum_time_backward: {
            localTetradPtr = localTetrad.begin();
            locT = localTetrad[0];
            break;
        }
    }
    locT->setLocalTime(tau0);

    m4d::vec4 actualPos = locT->getPosition();
    m4d::vec4 actualVel = locT->getVelocity();


    m4d::vec4* base = new m4d::vec4[4];
    for (int i=0;i<4;i++) {
        base[i] = locT->getE(i);
    }

    double min,max;
    mSolver->getBoundingTime(min,max);
    mSolver->setGeodType( m4d::enum_geodesic_timelike );
    mSolver->setTimeDir( geodDir );
    actualVel = mSolver->getTimeDir()*actualVel;

    mSolver->setBoundingTime(actualPos.x(0),actualPos.x(0)+geodDir*deltaT);
   // mSolver->Print();


    int numPoints = 0;
    GvsLocalTetrad* localT = new GvsLocalTetrad[maxNumPoints];

    //m4d::enum_break_condition breakCond =
            mSolver->calcParTransport(actualPos,actualVel,base,maxNumPoints,localT,numPoints);
    //if (breakCond==m4d::enum_break_constraint || breakCond==m4d::axis_Xenum_break_step_size) {
    //     mSolver->errorMessage(breakCond);
    //}

    // Start with i=1 because i=0 is already set by the initial value.
    for (int i=1;i<numPoints;i++) {
        locT = new GvsLocalTetrad(localT[i]);
        locT->setInCoords(true);
        //locT->Print();

        if (geodDir == m4d::enum_time_forward) {
            localTetrad.push_back(locT);
        } else {
            localTetrad.push_front(locT);
        }
    }
    numPositions = (int)localTetrad.size();

    delete [] localT;
    delete [] base;
}


void GvsStMotionGeodesic::Print ( FILE* fptr ) {
}

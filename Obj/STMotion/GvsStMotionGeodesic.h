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
#ifndef GVS_STMOTION_GEODESIC_H
#define GVS_STMOTION_GEODESIC_H

#include <iostream>
#include <deque>


#include "GvsGlobalDefs.h"

#include <Obj/STMotion/GvsStMotion.h>
#include "Utils/GvsGeodSolver.h"
#include <Utils/GvsGramSchmidt.h>

#include <metric/m4dMetric.h>



class GvsStMotionGeodesic : public GvsStMotion
{
  public:
    GvsStMotionGeodesic ( );
    GvsStMotionGeodesic ( m4d::Metric* metric, GvsGeodSolver* solver );
    virtual ~GvsStMotionGeodesic();

    GvsGeodSolver*  getSolver ( void ) const;

    // Setze lokale Tetrade als Starttetrade
    void  setStartLT     ( GvsLocalTetrad *lt );

    // Setze Startort (in Koordinaten) und
    // Startgeschwindigkeit (in Koordinaten oder bzgl lokaler Tetrade)
    void  setStartVal    ( const m4d::vec4 &pos, m4d::vec4 &vel, bool inCoords, m4d::enum_nat_tetrad_type lfType = m4d::enum_nat_tetrad_default);

    // Setze Starttetradenvektoren
    void  setStartTetrad ( );
    void  setStartTetrad ( const m4d::vec4 &e0, const m4d::vec4 &e1, const m4d::vec4 &e2, const m4d::vec4 &e3,
                           bool inCoords, m4d::enum_nat_tetrad_type lfType = m4d::enum_nat_tetrad_default);

    void setLambdaStep   ( double h );
    void setMaxNumPoints ( int maxPoints );

    // calculate geodesic for a time interval deltaTau backward or forward
    void calculateGeodesicMotion ( m4d::enum_time_direction geodDir, double deltaT, double tau0 = 0.0, double deltaTau = 0.0 );

    virtual void  Print ( FILE* fptr = stderr );

    // attributes
  protected:
    double lambdaStep;
    int    maxNumPoints;

    m4d::Metric*      mMetric;     // pointer to the spacetime
    GvsGeodSolver*    mSolver;        // pointer to a/the geodesic solver
    m4d::enum_geodesic_type mGeodType;      // type of geodesic: timelike (default), lightlike
};


inline
GvsGeodSolver*
GvsStMotionGeodesic :: getSolver ( void ) const
{
  return mSolver;
}

#endif

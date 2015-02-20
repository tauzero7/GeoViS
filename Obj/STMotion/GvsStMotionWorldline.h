// ---------------------------------------------------------------------
//  Copyright (c) 2013, Universitaet Stuttgart, VISUS, Thomas Mueller
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
#ifndef GVS_STMOTION_WORLDLINE_H
#define GVS_STMOTION_WORLDLINE_H

#include <iostream>
#include <string>
#include <deque>
#include <Geometry/PnD.h>
#include <Geometry/VnD.h>
#include <Geometry/Mat.h>
#include <GvsGlobalDefs.h>

#include <metric/m4dMetric.h>
#include <Geodesics/GvsGeodSolverBase.h>
#include <Obj/STMotion/GvsStMotion.h>

#include <Utils/GvsGramSchmidt.h>


using namespace std;

class GvsStMotionWorldline : public GvsStMotion
{
  public:
    GvsStMotionWorldline ( );
    GvsStMotionWorldline ( m4d::Metric* spacetime );
    GvsStMotionWorldline ( m4d::Metric* spacetime, GvsGeodSolverBase* solver );
    virtual ~GvsStMotionWorldline();

    GvsGeodSolverBase*  getSolver ( void ) const;

    // Setze lokale Tetrade als Starttetrade
    void  setStartLT     ( GvsLocalTetrad *lt );

    // Setze Startort (in Koordinaten) und
    // Startgeschwindigkeit (in Koordinaten oder bzgl lokaler Tetrade)
    void  setStartVal    ( const P5D &pos, V4D &vel, bool inCoords, GvsLFType lfType = GVS_LF);

    // Setze Starttetradenvektoren
    void  setStartTetrad ( );
    void  setStartTetrad ( const V4D &e0, const V4D &e1, const V4D &e2, const V4D &e3,
                           bool inCoords, GvsLFType lfType = GVS_LF);

    void setLambdaStep   ( double h );
    void setMaxNumPoints ( int maxPoints );

    // calculate motion for a time interval deltaTau backward or forward
    void calculateGeneralMotion ( GvsGeodesicDir geodDir, double deltaT, double tau0 = 0.0, double deltaTau = 0.0 );

    // set general motion from file
    void setGeneralMotion ( const string& filename, bool haveTau  );

    // attributes
  protected:
    double lambdaStep;
    int    maxNumPoints;

    m4d::Metric*      mSpacetime;     // pointer to the spacetime
    GvsGeodSolverBase* mSolver;        // pointer to a/the geodesic solver
};


inline
GvsGeodSolverBase*
GvsStMotionWorldline :: getSolver ( void ) const
{
  return mSolver;
}

#endif

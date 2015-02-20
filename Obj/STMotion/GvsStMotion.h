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
#ifndef GVS_STMOTION_H
#define GVS_STMOTION_H

#include <iostream>
#include <cstdio>
#include <cassert>
#include <deque>

#include "GvsGlobalDefs.h"
#include "Obj/GvsBase.h"
#include "Obj/GvsBoundBox4D.h"
#include "Obj/STMotion/GvsLocalTetrad.h"



enum GvsMotionType
{
  gvsMotionNoMotion, gvsMotionGeneral, gvsMotionGeodesic, gvsMotionConstVelocity, gvsMotionUnknown
};

const std::string GvsMotionTypeName[5] = {
    "NoMotion",
    "GeneralMotion",
    "GeodesicMotion",
    "ConstVelMotion",
    "UnknownMotion"
};

typedef std::deque< GvsLocalTetrad* > dequeLocT;
typedef dequeLocT::iterator      dequeLocTit;

class GvsStMotion : public GvsBase
{
  public:
    GvsStMotion();
    GvsStMotion( const GvsStMotion &motion );
    virtual ~GvsStMotion();

    void  setNoMotion();
    int   getNumPositions ( void   ) const;

    GvsMotionType  getMotionType ( ) const;

    m4d::vec4   getActualPosition ( double coordTime ) const;

    // access to a concrete local tetrad
    m4d::vec4   getPosition ( int k ) const;
    m4d::vec4   getVelocity ( int k ) const;
    m4d::vec4   getAccel    ( int k ) const;
    m4d::vec4   getE        ( int k, int j ) const;   // get E(j)

    m4d::vec4   getFirstPos ( void );
    m4d::vec4   getLastPos  ( void );

    void            setLocalTetrad ( GvsLocalTetrad* locT );
    GvsLocalTetrad* getLocalTetrad ( unsigned int k );

    void            setSTBoundBox  ( GvsBoundBox4D* box, int k=0 );
    GvsBoundBox4D*  getSTBoundBox  ( int k = 0 ) const;

    // delete all entries in localTetrad
    virtual void  deleteAllEntries ( );

    // get local tetrad that is closest to the requested time.
    GvsLocalTetrad*  getClosestLT ( double time, int &num );
    
    // zugehörig zur tetrade "num" (vgl. getClosestLT) gehörende
    // eigenzeit am objekt.
    // WICHTIG: keinen integrator mit schrittweitensteuerung verwenden,
    // sonst stimmt die Annahme tau=tau0+tetradNr*dTau nicht mehr!
    double           getLocalTime ( int num );

    // only used for MotionConstVelocity
    virtual bool getTransformedPolygon(const int segNr,
                                       const m4d::vec4& p0in, const m4d::vec4& p1in, m4d::vec4& p0out, m4d::vec4& p1out);


    virtual void Print    ( FILE* fptr = stderr );
    virtual void PrintAll ( FILE* fptr = stderr ) const;
    virtual void PrintToFile ( const char* filename );


    // attributes
  protected:

    int numPositions;               // number of positions
    dequeLocT     localTetrad;      // double-ended queue of all positions and orientations
    dequeLocTit   localTetradPtr;   // iterator of upper queue

    GvsMotionType mType;            // motion: nomotion, general, geodesic, unknown
    
    bool          mPastWarning;      //don't flood sterr with warnings of getClosestLT
    bool          mFutureWarning;
};


//----------------------------------------------------------------------------
//         inline functions
//----------------------------------------------------------------------------
inline int
GvsStMotion :: getNumPositions( void  ) const
{
  return numPositions;
}

inline GvsMotionType
GvsStMotion :: getMotionType ( ) const
{
  return mType;
}

#endif

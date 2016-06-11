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
#ifndef GVS_RAY_H
#define GVS_RAY_H

#include "GvsGlobalDefs.h"
#include "Obj/STMotion/GvsLocalTetrad.h"
//#include <Shader/GvsShader.h>
//#include <Shader/Surface/GvsSurfaceShader.h>

#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>

#include <cassert>
#include <vector>
#include <iostream>
#include <limits.h>


enum GvsRayType {
    polRay, polRayOneIS, polRayClosestIS, polRayVisual,
    polRayAnyIS, polRayAllIS
};


enum class GvsRayStatus {
    active = 0,
    finished
};


class GvsRayGen;
class GvsSurfIntersec;


class GvsRay
{
public:
    GvsRay ( );
    GvsRay ( GvsRayGen* gen );
    GvsRay ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen );
    GvsRay ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen,
             double minSearchDist, double maxSearchDist );

    GvsRay ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad, GvsRayGen* gen );
    GvsRay ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad, GvsRayGen* gen,
             double minSearchDist, double maxSearchDist );

    virtual ~GvsRay();

    
    virtual bool  recalc ( const m4d::vec4 &orig, const m4d::vec4 &dir );
    virtual bool  recalc ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad* tetrad );
    virtual bool  recalcJacobi ( const m4d::vec4 &orig, const m4d::vec4 &dir,
                                 const m4d::vec3 &locRayDir, const GvsLocalTetrad* tetrad );

    void           setSearchInterval ( double minDist, double maxDist );

    GvsRayGen*     getRayGen    () const;
    m4d::vec4*     points       ();
    m4d::vec4*     tangents     ();
    int            getNumPoints () const;

    void         setPoints      ( m4d::vec4* pts  );
    void         setDirs        ( m4d::vec4* dirs );
    void         setNumPoints   ( int noPts );

    m4d::vec4      getPoint       ( int index ) const;
    m4d::vec4      getTangente    ( int index ) const;
    m4d::vec5      getJacobi      ( int index ) const;
    GvsLocalTetrad getTetrad      ( int index ) const;


    ulong          getID          ( ) const;

    double         minSearchDist  ( ) const;
    double         maxSearchDist  ( ) const;

    virtual bool   intersecFound  ( ) const = 0;

    /**
     * Store surface intersection
     * @param surfIntersec  reference to surface intersection object
     * @return finished if no more intersections shall be tested
     */
    virtual GvsRayStatus store( const GvsSurfIntersec &surfIntersec ) = 0;  // pure virtual
    virtual bool   isValidSurfIntersec ( double dist ) const;

    virtual void   Print ( FILE* fptr = stderr );



    static ulong   getNonRayID       ( void );
    static ulong   getNumRaysTraced  ( void );

    void           timeShiftRay      ( double timeDelta );

    static double  calcRayDist  ( const int seg, const double alpha );
    static bool    isIn         ( const int seg, const double alpha , const double endDist);
    static bool    isValidAlpha ( const double alpha );
    static void    splitRayDist ( const double distance, int &seg, double &alpha );

    m4d::enum_break_condition   getBreakCond();

protected:
    void  setMinSearchDist ( double minDist );
    void  setMaxSearchDist ( double maxDist );

    void deleteAll();

private:
    static ulong  getNextRayID();


protected:
    ulong               rayID;
    GvsRayGen*          rayGen;       // only pointer to rayGen, do not delete here!!
    m4d::vec4*          rayPoints;
    m4d::vec4*          rayDirs;
    GvsLocalTetrad*     rayTetrad;    
    double*             rayLambda;
    m4d::vec4*          raySachs1;
    m4d::vec4*          raySachs2;
    m4d::vec5*          rayJacobi;
    m4d::vec5           rayMaxJacobi;

    int                 rayNumPoints;
    double              rayMinSearchDist;
    double              rayMaxSearchDist;

    bool                rayHasTetrad;
    m4d::enum_break_condition rayBreakCond;
};

//----------------------------------------------------------------------------
//            i n l i n e
//----------------------------------------------------------------------------
inline ulong GvsRay :: getNonRayID () {
    return 0UL;
}

inline ulong GvsRay :: getNumRaysTraced () {
  return getNextRayID() - 1;
}

#endif

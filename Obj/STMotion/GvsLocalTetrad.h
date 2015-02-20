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
#ifndef GVS_LOCAL_TETRAD_H
#define GVS_LOCAL_TETRAD_H

#include <iostream>
#include <cassert>

#include "GvsGlobalDefs.h"
#include "Obj/GvsBase.h"
#include "Obj/GvsBoundBox.h"
#include "Obj/GvsBoundBox4D.h"


#include <metric/m4dMetric.h>


class GvsLocalTetrad : public GvsBase
{
public:
    GvsLocalTetrad ( );
    GvsLocalTetrad ( m4d::Metric* metric );
    GvsLocalTetrad ( const GvsLocalTetrad* lt );
    GvsLocalTetrad ( m4d::Metric* metric, const m4d::vec4 &p, const m4d::vec4 &v );
    GvsLocalTetrad ( m4d::Metric* metric,
                     const m4d::vec4 &e_0, const m4d::vec4 &e_1, const m4d::vec4 &e_2, const m4d::vec4 &e_3,
                     const m4d::vec4 &p, const bool coords=true);
    ~GvsLocalTetrad( );

    void  setLocalTetrad ( const GvsLocalTetrad &lT);

    void  setTetrad    ( const m4d::vec4 &e_0, const m4d::vec4 &e_1, const m4d::vec4 &e_2, const m4d::vec4 &e_3, bool gramSchmidt = false );
    void  setPosition  ( const m4d::vec4 &p );
    void  setPositionX ( int coord, double val );
    void  setVelocity  ( const m4d::vec4 &v );
    void  setAccel     ( const m4d::vec4 &a );
    void  setInCoords  ( const bool coords, const m4d::enum_nat_tetrad_type lft = m4d::enum_nat_tetrad_default );
    void  setLocalTime ( double tau );

    void  setLocalVel  ( const m4d::vec3 &v, const m4d::enum_nat_tetrad_type lft = m4d::enum_nat_tetrad_default );

    void  setE         ( int k, const m4d::vec4 &e_k );
    void  setTriad     ( const m4d::vec4 &e_1, const m4d::vec4 &e_2, const m4d::vec4 &e_3 );

    m4d::vec4 getE     ( int k ) const;
    void  getTetrad    ( m4d::vec4 &e_0, m4d::vec4 &e_1, m4d::vec4 &e_2, m4d::vec4 &e_3 );

    m4d::vec4 getBase  ( int k ) const;
    void  getInvTetrad ( m4d::vec4 &b_0, m4d::vec4 &b_1, m4d::vec4 &b_2, m4d::vec4 &b_3 );

    void  calcInvert   ( void );
    void  calcInvert   ( const m4d::vec4 &a0, const m4d::vec4 &a1, const m4d::vec4 &a2, const m4d::vec4 &a3,
                         m4d::vec4 &b0, m4d::vec4 &b1, m4d::vec4 &b2, m4d::vec4 &b3);

    bool  isRightHanded ( ) const;

    double getTime() const;
    double getLocalTime() const;

    m4d::vec4  getPosition() const;
    m4d::vec4  getVelocity() const;
    m4d::vec4  getAccel() const;

    bool       getInCoords ( void ) const;
    void       setLFType   ( const m4d::enum_nat_tetrad_type lftype );
    m4d::enum_nat_tetrad_type  getLFType   ( void ) const;

    void  adjustTetrad ( );  // test position and velocity and adapt the tetrad such that
                             // the base vector e0 points in the direction of motion.

    //! transform tetrad between coordinate and natural tetrad representation
    void  transformTetrad ( const bool coords, const m4d::enum_nat_tetrad_type lft = m4d::enum_nat_tetrad_default);

    // transform a point between coordinate and tetrad representation
    m4d::vec4 transToLocTetrad ( const m4d::vec4 &point ) const;
    m4d::vec4 transToCoords    ( const m4d::vec4 &point ) const;

    // three and four-velocity wrt. local tetrad
    m4d::vec4 getFourVelocity  ( const m4d::vec3 &v ) const;
    m4d::vec4 getFourVelocity  ( const m4d::vec4 &v ) const;
    m4d::vec3 getThreeVelocity ( const m4d::vec4 &u ) const;

    m4d::vec4 localToCoord     ( const m4d::vec4 &vec ) const;
    m4d::vec4 coordToLocal     ( const m4d::vec4 &vec ) const;


    void       setMetric ( m4d::Metric* metric );
    m4d::Metric* getMetric () const;

    void            setSTBoundBox  ( GvsBoundBox4D* box );
    GvsBoundBox4D*  getSTBoundBox  ( ) const;


    GvsLocalTetrad* getInterpolatedTetrad ( GvsLocalTetrad* lt0, GvsLocalTetrad* lt1, double frak );

    int SetParam ( std::string pName, int val );
    int SetParam ( std::string pName, double val );
    int SetParam ( std::string pName, m4d::vec4 pt );
    int SetParam ( std::string pName, m4d::vec3 vt );

    void printP ( ) const;
    void printS ( std::ostream &os = std::cout );

    virtual void Print  ( FILE* fptr = stderr );

    // ------ attributes ------
private:
    //! TRUE  : base std::vectors are given with respect to coordinates
    //! FALSE : base std::vectors are given with respect to a natural local tetrad defined in GvsMetric
    bool inCoords;

    //! If inCoords==false the local tetrad will be defined wrt. the local frame defind in GvsMetric.
    //! The type of the local frame is given by lfType.
    m4d::enum_nat_tetrad_type lfType;

    //! Base vectors of the local tetrad
    m4d::vec4 e[4];        // e_(i) = e_(i)^mu \partial_mu

    //! inverse Base vectors
    m4d::vec4 base[4];

    //! local tetrad is a right-handed system (mRightHanded==true)
    bool mRightHanded;

    //! Position, velocity, and acceleration are with respect to coordinates.
    m4d::vec4 pos;
    m4d::vec4 vel;
    m4d::vec4 acc;

    //! proper time
    double mTau;

    //! local tetrad knows the metric it lives in.
    m4d::Metric* locTetradMetric;

    //! BoundingBox around the local tetrad (will be calculated in LocalCompObj)
    GvsBoundBox4D*   stBoundBox;
};

//----------------------------------------------------------------------------
//         inline getE(i), getPos
//----------------------------------------------------------------------------
inline m4d::vec4
GvsLocalTetrad :: getE( int k ) const {
    assert(k<4);
    return e[k];
}

inline m4d::vec4 GvsLocalTetrad :: getBase( int k ) const {
    assert(k<4);
    return base[k];
}

inline double GvsLocalTetrad :: getTime ( void ) const {
    return pos.x(0);
}

inline double GvsLocalTetrad :: getLocalTime ( void ) const {
    return mTau;
}

inline m4d::vec4 GvsLocalTetrad :: getPosition( void   ) const {
    return pos;
}

inline m4d::vec4 GvsLocalTetrad :: getVelocity( void   ) const {
    return vel;
}

inline m4d::vec4 GvsLocalTetrad :: getAccel( void   ) const {
    return acc;
}

inline bool GvsLocalTetrad :: isRightHanded ( void ) const {
    return mRightHanded;
}

#endif

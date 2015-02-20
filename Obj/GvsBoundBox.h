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
#ifndef GVS_BOUND_BOX_H
#define GVS_BOUND_BOX_H

#include <iostream>
#include <float.h>

#include "GvsGlobalDefs.h"

class GvsBoundBox
{
  public:
    GvsBoundBox ( void );
    GvsBoundBox ( const m4d::vec3& lowBounds, const m4d::vec3& uppBounds, bool testBounds = true );
    GvsBoundBox ( double x,  double y,  double z,
                  double dx, double dy, double dz, bool testBounds = true);
    GvsBoundBox ( const GvsBoundBox& otherBox );

    virtual ~GvsBoundBox(){}

    bool      isEmpty   ( ) const;
    m4d::vec3       lowBounds ( ) const;
    m4d::vec3       uppBounds ( ) const;
    m4d::vec3       size      ( ) const;
    double    size      ( int coord ) const;   // Tiefe, Hoehe
    double    volume    ( ) const;
    double    surface   ( ) const;

    bool      contains  ( const m4d::vec3 &point ) const;
    void      extendBoxToContain ( const m4d::vec3& pt );

    // -------- operators --------
    GvsBoundBox&  operator=  ( const GvsBoundBox &otherBox );
    GvsBoundBox   operator+  ( const GvsBoundBox &otherBox ) const;   // union
    GvsBoundBox&  operator+= ( const GvsBoundBox &otherBox );
    GvsBoundBox   operator*  ( const GvsBoundBox &otherBox ) const;   // intersection
    GvsBoundBox&  operator*= ( const GvsBoundBox &otherBox );

    bool          getTentryTexit ( const m4d::vec3 &p0, const m4d::vec3 &p1, double tp0, double tp1,
                                   double &time_Entry, double &time_Exit,
                                   short &entryFace, short &exitFace ) const;

    virtual   void  scale      ( const m4d::vec3&   scaleVec );
    virtual   void  translate  ( const m4d::vec3&   transVec );
    virtual   void  rotate     ( const m4d::vec3&   rotAxis, double rotAngle );
    virtual   void  transform  ( const m4d::Matrix<double,3,4>& mat      );

    int       getChart ( ) const;
    void      setChart ( const int chart );

    void      Print ( FILE* fptr = stderr );

  protected:
    m4d::vec3  boxLowBounds;
    m4d::vec3  boxUppBounds;
};


inline
m4d::vec3 GvsBoundBox :: lowBounds ( ) const
{
  return boxLowBounds;
}

inline
m4d::vec3 GvsBoundBox :: uppBounds ( ) const
{
  return boxUppBounds;
}

#endif

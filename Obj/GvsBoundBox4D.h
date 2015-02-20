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
#ifndef GVS_BOUND_BOX_4D_H
#define GVS_BOUND_BOX_4D_H

#include <float.h>
#include <iostream>

#include "GvsGlobalDefs.h"


class GvsBoundBox4D
{
  public:
    GvsBoundBox4D ( void );
    GvsBoundBox4D ( const m4d::vec4& lowBounds, const m4d::vec4& uppBounds,
                    bool testBounds = true );
    GvsBoundBox4D ( double t,  double x,  double y,  double z,
                    double dt, double dx, double dy, double dz,
                    bool testBounds = true);
    GvsBoundBox4D ( const GvsBoundBox4D& otherBox );

    virtual ~GvsBoundBox4D(){}

    bool       isEmpty   ( ) const;
    m4d::vec4  lowBounds ( ) const;
    m4d::vec4  uppBounds ( ) const;

    m4d::vec4  size      ( ) const;
    double     size      ( int coord ) const;   // Tiefe, Hoehe
    double     volume    ( ) const;
    double     surface   ( ) const;

    void  Print ( FILE* fptr = stderr );
    
    GvsBoundBox4D& operator= (const GvsBoundBox4D &otherBox);
    GvsBoundBox4D  operator+ (const GvsBoundBox4D &otherBox) const;   // union

  protected:
    m4d::vec4  boxLowBounds;
    m4d::vec4  boxUppBounds;
    int  chart;  // ??
};


//---------------------------- inline functions -----------------------------------
inline m4d::vec4
GvsBoundBox4D :: lowBounds ( ) const
{
  return boxLowBounds;
}

inline m4d::vec4
GvsBoundBox4D :: uppBounds ( ) const
{
  return boxUppBounds;
}

#endif

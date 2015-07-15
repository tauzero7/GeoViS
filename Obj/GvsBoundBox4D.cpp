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

#include "Obj/GvsBoundBox4D.h"


//----------------------------------------------------------------------------
//         constructor, destructor
//----------------------------------------------------------------------------
GvsBoundBox4D::GvsBoundBox4D ( void )
{
}

GvsBoundBox4D::GvsBoundBox4D ( const m4d::vec4& lowBounds, const m4d::vec4& uppBounds,
                               bool testBounds)
{
  if ( testBounds )
  {
    for ( int i = 0; i < 4; i++ )
      if ( lowBounds[i] <= uppBounds[i] )
      {
        boxLowBounds[i] = lowBounds[i];
        boxUppBounds[i] = uppBounds[i];
      }
      else
      {
        boxLowBounds[i] = uppBounds[i];
        boxUppBounds[i] = lowBounds[i];
      }
  }
  else
  {
    for ( int i=0;i<4;i++ )
    {
      boxLowBounds[i] = lowBounds[i];
      boxUppBounds[i] = uppBounds[i];
    }
  }
}

GvsBoundBox4D :: GvsBoundBox4D ( double t,  double x,  double y,  double z,
                                 double dt, double dx, double dy, double dz,
                                 bool testBounds)
{
  if ( testBounds )
  {
    if ( dt >= 0 )
    {
      boxLowBounds[0] = t;
      boxUppBounds[0] = t + dt;
    }
    else
    {
      boxLowBounds[0] = t + dt;
      boxUppBounds[0] = t;
    }

    if ( dx >= 0 )
    {
      boxLowBounds[1] = x;
      boxUppBounds[1] = x + dx;
    }
    else
    {
      boxLowBounds[1] = x + dx;
      boxUppBounds[1] = x;
    }

    if ( dy >= 0 )
    {
      boxLowBounds[2] = y;
      boxUppBounds[2] = y + dy;
    }
    else
    {
      boxLowBounds[2] = y + dy;
      boxUppBounds[2] = y;
    }

    if ( dz >= 0 )
    {
      boxLowBounds[3] = z;
      boxUppBounds[3] = z + dz;
    }
    else
    {
      boxLowBounds[3] = z + dz;
      boxUppBounds[3] = z;
    }
  }
  else
  {
    boxLowBounds[0] = t;
    boxLowBounds[1] = x;
    boxLowBounds[2] = y;
    boxLowBounds[3] = z;
    boxUppBounds[0] = t + dt;
    boxUppBounds[1] = x + dx;
    boxUppBounds[2] = y + dy;
    boxUppBounds[3] = z + dz;
  }
}

GvsBoundBox4D::GvsBoundBox4D ( const GvsBoundBox4D& otherBox )
{
  boxLowBounds = otherBox.boxLowBounds;
  boxUppBounds = otherBox.boxUppBounds;
}

//-------------------------------------------------------------------------
//         isEmpty
//-------------------------------------------------------------------------
bool
GvsBoundBox4D :: isEmpty() const
{
  return (boxLowBounds[0] >= boxUppBounds[0]) ||
         (boxLowBounds[1] >= boxUppBounds[1]) ||
         (boxLowBounds[2] >= boxUppBounds[2]);
}


//-------------------------------------------------------------------------
//         volume
//-------------------------------------------------------------------------
double
GvsBoundBox4D :: volume() const
{
  return (boxUppBounds[0] - boxLowBounds[0]) *
         (boxUppBounds[1] - boxLowBounds[1]) *
         (boxUppBounds[2] - boxLowBounds[2]);
}


//-------------------------------------------------------------------------
//         surface
//-------------------------------------------------------------------------
double
GvsBoundBox4D :: surface() const
{
  m4d::vec4 size = boxUppBounds - boxLowBounds;

  return 2.0 * (size[0] * size[1] + size[1] * size[2] + size[2] * size[0]);
}

//-------------------------------------------------------------------------
//         print
//-------------------------------------------------------------------------
void GvsBoundBox4D ::  Print ( FILE* fptr ) {
    fprintf(fptr,"BoundBox4D {\n");
    fprintf(fptr,"\tlow  %7.4f %7.4f %7.4f %7.4f\n",boxLowBounds[0],boxLowBounds[1],boxLowBounds[2],boxLowBounds[3]);
    fprintf(fptr,"\tupp  %7.4f %7.4f %7.4f %7.4f\n",boxUppBounds[0],boxUppBounds[1],boxUppBounds[2],boxUppBounds[3]);
    fprintf(fptr,"}\n");
}

GvsBoundBox4D& GvsBoundBox4D::operator= (const GvsBoundBox4D &otherBox) {
    boxLowBounds = otherBox.boxLowBounds;
    boxUppBounds = otherBox.uppBounds();
    return *this;
}

GvsBoundBox4D  GvsBoundBox4D::operator+ (const GvsBoundBox4D &otherBox) const {
    GvsBoundBox4D unionBox;
    for(int i=0; i<4; i++) {
        unionBox.boxLowBounds[i] = (boxLowBounds[i] < otherBox.boxLowBounds[i] ? boxLowBounds[i] : otherBox.boxLowBounds[i]);
        unionBox.boxUppBounds[i] = (boxUppBounds[i] > otherBox.boxUppBounds[i] ? boxUppBounds[i] : otherBox.boxUppBounds[i]);
    }
    return unionBox;
}

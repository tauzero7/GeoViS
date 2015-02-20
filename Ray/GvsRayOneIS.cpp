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

#include "GvsRayOneIS.h"
#include <iostream>


GvsRayOneIS :: GvsRayOneIS ( )
    : GvsRay () {
}

GvsRayOneIS :: GvsRayOneIS ( GvsRayGen* gen )
    : GvsRay ( gen ) {
}

GvsRayOneIS :: GvsRayOneIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen )
    : GvsRay ( orig, dir, gen ) {
}

GvsRayOneIS :: GvsRayOneIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen,
                             double minSearchDist, double maxSearchDist )
    : GvsRay ( orig, dir, gen, minSearchDist, maxSearchDist ) {
}

GvsRayOneIS :: GvsRayOneIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad, GvsRayGen* gen )
    : GvsRay ( orig, dir, tetrad, gen ) {
}

GvsRayOneIS :: GvsRayOneIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad, GvsRayGen* gen,
                             double minSearchDist, double maxSearchDist )
    : GvsRay ( orig, dir, tetrad, gen, minSearchDist, maxSearchDist ) {
}

GvsRayOneIS :: ~GvsRayOneIS() {
}


bool GvsRayOneIS :: store( const GvsSurfIntersec &surfIntersec ) {
    if ( isValidSurfIntersec( surfIntersec.dist() ) )
    {
        raySurfIntersec = surfIntersec;
        setMaxSearchDist(surfIntersec.dist());
        //std::cerr << "store: " << raySurfIntersec.dist() << std::endl;
        return true;
    }
    return false;
}


GvsSurfIntersec&  GvsRayOneIS::surfIntersec()  {
    return raySurfIntersec;
}

GvsSurfIntersec *GvsRayOneIS::getSurfIntersec( )  {
    return &raySurfIntersec;
}


bool GvsRayOneIS :: intersecFound ( ) const {
    // std::cerr << "GvsRayOneIS :: intersecFound ( )\n";
    return ( intersecIndex() > 0 );
}

int GvsRayOneIS :: intersecIndex ( ) const {
    /*
    if ( raySurfIntersec.dist() < DBL_MAX )
  return ( rayVolIntersec.exitDist < DBL_MAX ) ? 3 : 1;
    else
  return ( rayVolIntersec.exitDist < DBL_MAX ) ? 2 : 0;
  */
    return 0;
}


GvsShader*
GvsRayOneIS :: intersecShader ( ) const
{
    /*
      switch (intersecIndex())
      {
    case 0:  return NULL;
    case 1:  return raySurfIntersec.shader();
    default: return rayVolIntersec.volume();
      }
  */
    return raySurfIntersec.shader();
}


GvsSceneObj* GvsRayOneIS :: intersecObject  ( ) const {
    return raySurfIntersec.object();
}

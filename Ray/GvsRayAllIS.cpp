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

#include "Ray/GvsRayAllIS.h"


GvsRayAllIS::GvsRayAllIS(const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen)
    : GvsRay ( orig, dir, gen )
{
}

GvsRayAllIS :: GvsRayAllIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen,
                             double minSearchDist, double maxSearchDist )
    : GvsRay ( orig, dir, gen, minSearchDist, maxSearchDist )
{
}

GvsRayAllIS :: ~GvsRayAllIS()
{
}

bool GvsRayAllIS :: intersecFound ( void ) const {
    return true; //...
}

bool GvsRayAllIS :: store( const GvsSurfIntersec &surfIntersec ) {
    if ( isValidSurfIntersec(surfIntersec.dist()) ) {
        return true;
    }
    return false;
}

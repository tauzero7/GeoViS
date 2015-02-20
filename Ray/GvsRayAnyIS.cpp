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

#include "Ray/GvsRayAnyIS.h"


GvsRayAnyIS::GvsRayAnyIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen )
    : GvsRayOneIS ( orig, dir, gen )
{
}

GvsRayAnyIS :: GvsRayAnyIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen,
                             double minSearchDist, double maxSearchDist )
    : GvsRayOneIS ( orig, dir, gen, minSearchDist, maxSearchDist )
{
}

GvsRayAnyIS :: ~GvsRayAnyIS (){}


bool GvsRayAnyIS :: testIntersection( GvsObjPtrList&  ) {
    std::cerr << "GvsRayAnyIS :: testIntersection()... not implemented yet!\n";
    return false;
}


bool GvsRayAnyIS :: store ( const GvsSurfIntersec &surfIntersec ) {
    if (isValidSurfIntersec( surfIntersec.dist() )) {
        raySurfIntersec = surfIntersec;
        return true;
    }
    return false;
}

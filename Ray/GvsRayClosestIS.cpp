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

#include "Ray/GvsRayClosestIS.h"


GvsRayClosestIS :: GvsRayClosestIS ()
    : GvsRayOneIS ()
{
}

GvsRayClosestIS :: GvsRayClosestIS ( GvsRayGen* gen )
    : GvsRayOneIS ( gen )
{
}

GvsRayClosestIS :: GvsRayClosestIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen )
    : GvsRayOneIS ( orig, dir, gen )
{
}

GvsRayClosestIS :: GvsRayClosestIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen,
                                     double minSearchDist, double maxSearchDist )
    : GvsRayOneIS ( orig, dir, gen, minSearchDist, maxSearchDist )
{
}


GvsRayClosestIS :: GvsRayClosestIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad,
                                     GvsRayGen* gen )
    : GvsRayOneIS ( orig, dir, tetrad, gen )
{
}

GvsRayClosestIS :: GvsRayClosestIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad,
                                     GvsRayGen* gen, double minSearchDist, double maxSearchDist )
    : GvsRayOneIS ( orig, dir, tetrad, gen, minSearchDist, maxSearchDist )
{
}

GvsRayClosestIS::~GvsRayClosestIS() {
}

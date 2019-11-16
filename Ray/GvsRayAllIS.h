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
#ifndef GVS_RAY_ALL_IS_H
#define GVS_RAY_ALL_IS_H

#include "GvsGlobalDefs.h"
#include "Ray/GvsSurfIntersec.h"
#include "Ray/GvsRay.h"

#include "m4dGlobalDefs.h"

class GvsRayAllIS : public GvsRay
{
public:
    GvsRayAllIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen);
    GvsRayAllIS ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen,
                  double minSearchDist, double maxSearchDist );

    virtual ~GvsRayAllIS ();

    virtual bool intersecFound ( void ) const;
    virtual bool store ( const GvsSurfIntersec &surfIntersec );
};

#endif

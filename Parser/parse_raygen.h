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
#ifndef PARSE_RAYGEN_H
#define PARSE_RAYGEN_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

class GvsParseScheme;

pointer gvsP_init_raygen (scheme *sc, pointer args);
void    gvsP_init_raygenSimple ( GvsParseScheme* gP );

pointer gvsP_calc_ray      ( scheme *sc, pointer args );
pointer gvsP_calc_proj_ray ( scheme *sc, pointer args );

#endif

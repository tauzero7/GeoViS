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
#ifndef PARSE_TEXTURE_H
#define PARSE_TEXTURE_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

class GvsParseScheme;

pointer gvsP_init_texture (scheme *sc, pointer args);

void gvsP_init_unitex           ( GvsParseScheme* gP );
void gvsP_init_checkerT2D       ( GvsParseScheme* gP );
void gvsP_init_chequeredT2D     ( GvsParseScheme* gP );
//void gvsP_init_chequeredTimeT2D ( GvsParseScheme* gP );

void gvsP_init_dieT2D           ( GvsParseScheme* gP );
void gvsP_init_image2dsampler   ( GvsParseScheme* gP );

void gvsP_init_colorGradTex     ( GvsParseScheme* gP );
#endif

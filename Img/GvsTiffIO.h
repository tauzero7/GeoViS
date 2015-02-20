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
#ifndef Gvs_TIFF_IO_H
#define Gvs_TIFF_IO_H

#if HAVE_LIBTIFF==1

#include "Img/GvsPictureIO.h"
#include "Img/GvsChannelImg2D.h"

class GvsTiffIO : public GvsPictureIO
{
public:
    GvsTiffIO();
    virtual bool readChannelImg  ( GvsChannelImg2D& chanImg, const char *filename );
    virtual bool writeChannelImg ( GvsChannelImg2D& chanImg, const char *filename );
};

#endif // HAVE_LIBTIFF

#endif

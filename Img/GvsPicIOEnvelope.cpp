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

#include "GvsPicIOEnvelope.h"
#include "GvsChannelImg2D.h"

#ifdef HAVE_LIBTIFF
#include "GvsTiffIO.h"
#endif

#ifdef HAVE_LIBPNG
#include "GvsPngIO.h"
#endif

#ifdef HAVE_LIBPPM
#include "GvsPpmIO.h"
#endif

#include <cstring>
#include <cassert>
#include <cstdlib>


GvsPicIOEnvelope :: GvsPicIOEnvelope() {
    PicIOMgr = 0;
}


GvsPicIOEnvelope :: ~GvsPicIOEnvelope () {
    delete PicIOMgr;
}

void GvsPicIOEnvelope :: allocPicIOMgr( const char *filename )
{
    delete PicIOMgr;

#ifdef HAVE_LIBTIFF
    if ( has_extension( filename, ".tiff" ) || has_extension( filename, ".tif" ) ) {
        PicIOMgr = new GvsTiffIO;
        return;
    }
#endif // HAVE_LIBTIFF

#ifdef HAVE_LIBPNG
    if ( has_extension( filename, ".png" ) ) {
        PicIOMgr = new GvsPngIO;
        return;
    }
#endif // HAVE_LIBPNG

#ifdef HAVE_LIBPPM
    if ( has_extension( filename, ".ppm" ) ) {
        PicIOMgr = new GvsPpmIO;
        return;
    }
#endif // HAVE_LIBPPM
    fprintf(stderr,"File ending unknown: %s\n",filename);
    exit(1);
}


bool GvsPicIOEnvelope :: testImageExtension ( const std::string filename ) {
    std::string ext;
    int k = filename.rfind(".");
    ext.assign(filename,k+1,filename.size());

    bool imageExt_exists = false;
    if (ext=="ppm") {
        imageExt_exists = true;
    }
#ifdef HAVE_LIBTIFF
    else if ((ext=="tiff") || (ext=="tif")) {
        imageExt_exists = true;
    }
#endif
#ifdef HAVE_LIBPNG
    else if (ext=="png") {
        imageExt_exists = true;
    }
#endif
    return imageExt_exists;
}


bool GvsPicIOEnvelope :: readChannelImg( GvsChannelImg2D& chanImg,
                                        const char *filename ) {
    allocPicIOMgr( filename );
    chanImg.setFileName( filename );

    return PicIOMgr->readChannelImg( chanImg, filename );
}


bool GvsPicIOEnvelope :: writeChannelImg( GvsChannelImg2D& chanImg,
                                         const char *filename ) {
    allocPicIOMgr( filename );
    return PicIOMgr->writeChannelImg( chanImg, filename );
}


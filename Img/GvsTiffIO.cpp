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

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "Img/GvsChannelImg2D.h"
#include "Img/GvsTiffIO.h"

#if HAVE_LIBTIFF==1
#include <tiffio.h>

GvsTiffIO::GvsTiffIO() {
}


bool GvsTiffIO::readChannelImg( GvsChannelImg2D& chanImg, const char * filename) {
    TIFF *tiffImg;
    unsigned int x, y;

    // open the image file
    tiffImg = TIFFOpen( filename,"r");
    if (tiffImg == NULL ) {
        std::cerr << "Error in GvsTiffIO::read(): can't open input file `"
                  << filename << "'." << std::endl;
        return false;
    }

    int numChannels = 3;

    uint32 xsize1, ysize1;
    TIFFGetField(tiffImg, TIFFTAG_IMAGEWIDTH, &xsize1);
    TIFFGetField(tiffImg, TIFFTAG_IMAGELENGTH, &ysize1);
    unsigned int xsize = xsize1;
    unsigned int ysize = ysize1;

    chanImg.setSize(xsize, ysize, numChannels );

    uint32 *buf = new uint32[xsize*ysize];
    assert( buf != NULL );

    int stopOnError = 0;
    if (TIFFReadRGBAImage(tiffImg, xsize, ysize, buf, stopOnError) == 0) {
        std::cerr << "Error in GvsTiffIO::read(): can't convert file `"
                  << filename << "'." << std::endl;
        return false;
    }

    uchar *pixel = new uchar[ numChannels ];
    assert( pixel != NULL );

    //FILE* fptr = fopen("test.ppm","w");
    //fprintf(fptr,"P6\n%d %d\n%d\n",xsize,ysize,255);

    // Copy buffer from Tiff structure to image
    for ( y = 0; y < ysize; y++ ) {
        for ( x = 0; x < xsize; x++ ) {
            pixel[0] = TIFFGetR(buf[x+xsize*y]);  // Red
            pixel[1] = TIFFGetG(buf[x+xsize*y]);  // Green
            pixel[2] = TIFFGetB(buf[x+xsize*y]);  // Blue
            chanImg.setChannels( x, ysize-1-y, pixel );
            //fwrite(pixel,sizeof(uchar),3,fptr);
        }
    }
    //fclose(fptr);

    delete [] pixel;
    delete [] buf;

    chanImg.setFileName( filename );

    TIFFClose( tiffImg );
    return true;
}

bool GvsTiffIO::writeChannelImg( GvsChannelImg2D& chanImg, const char *filename ) {
    if ( chanImg.numChannels() != 3 ) {
        std::cerr << "Error: GvsTiffIO::write() supports only 3-channel-images." << std::endl;
        return false;
    }

    TIFF *out = TIFFOpen(filename, "w");
    if (out == NULL) {
        std::cerr << "Error: GvsTiffIO::write() cannot open file: " << filename << std::endl;
        return false;
    }

    unsigned int nl = static_cast<unsigned int>(chanImg.height());
    unsigned int nc = static_cast<unsigned int>(chanImg.width());

    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (uint32) nc);
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, (uint32) nl);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
    //TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    // LZW scanline encoding is no longer implemented due to Unisys patent enforcement

    TIFFSetField(out, TIFFTAG_COMPRESSION,1);

    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(out, TIFFTAG_MINSAMPLEVALUE, (uint16) 0);
    TIFFSetField(out, TIFFTAG_MAXSAMPLEVALUE, (uint16) 255);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, (uint32) -1));

    /*
    uchar* ip = chanImg.getImagePtr();
    int pos;
    FILE* fptr = fopen("test.ppm","w");
    fprintf(fptr,"P6\n%d %d\n%d\n",chanImg.width(),chanImg.height(),255);
    for(int i=0; i<chanImg.width()*chanImg.height(); i++) {
        pos = 0*chanImg.width()*chanImg.height() + i;
        fwrite((void*)&ip[pos],sizeof(uchar),1,fptr);
        pos = 1*chanImg.width()*chanImg.height() + i;
        fwrite((void*)&ip[pos],sizeof(uchar),1,fptr);
        pos = 2*chanImg.width()*chanImg.height() + i;
        fwrite((void*)&ip[pos],sizeof(uchar),1,fptr);
    }
    fclose(fptr);
*/
    unsigned int x,y;
    tdata_t buf = _TIFFmalloc(TIFFScanlineSize(out));

    for (y = 0; y < nl; y++) {
        uint8* pp = (uint8*) buf;
        for (x = 0; x < nc; x++) {
            chanImg.sampleChannels( x, y, pp);
            pp += 3;
        }

        if (TIFFWriteScanline(out, buf, y, 0) < 0) {
            _TIFFfree(buf);
            return true;
        }
    }

    _TIFFfree(buf);
    TIFFClose(out);
    return true;
}

#endif

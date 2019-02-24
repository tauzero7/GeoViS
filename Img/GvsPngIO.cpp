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
#include "Img/GvsPngIO.h"

#if HAVE_LIBPNG==1
#include <png.h>

GvsPngIO::GvsPngIO() {
}


bool GvsPngIO::readChannelImg( GvsChannelImg2D& chanImg, const char * filename) {

    png_structp   png_ptr;
    png_infop     info_ptr;
    unsigned int  sig_read = 0;
    png_uint_32   width, height;
    int           bit_depth, color_type, interlace_type;

    FILE          *fp;
#ifdef _WIN32
    fopen_s(&fp,filename,"rb");
#else
    fp = fopen(filename, "rb");
#endif
    if (fp == NULL) {
        fprintf(stderr,"Cannot open file %s for reading.\n",filename);
        return false;
    }

    /* Create and initialize the png_struct with the desired error handler */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr==NULL) {
        fclose(fp);
        return false;
    }

    /* Allocate/initialize memory for image information */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr==NULL) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return false;
    }

    /* setup the input control */
    png_init_io (png_ptr, fp);
    png_set_sig_bytes (png_ptr, sig_read);

    png_read_info(png_ptr, info_ptr);
    png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

    /* checks the color_type and transforms to... */
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }

    //if ((color_type == PNG_COLOR_TYPE_GRAY) && (bit_depth < 8))
    //    png_set_gray_1_2_4_to_8(png_ptr);

    //if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {           // delete alpha channel
    //png_set_strip_alpha(png_ptr);
    //}
    //if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {            // delete alpha channel
    //png_set_strip_alpha(png_ptr);
    //}

    /* updates the pointers */
    png_read_update_info(png_ptr,info_ptr);

    /* allocates memory for the image */
    png_bytep *row_pointers = new png_bytep[height];
    for (unsigned int row = 0; row < height; row++) {
        row_pointers[row] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
    }

    /* read image in one go */
    png_read_image(png_ptr, row_pointers);


    int numChannels = (int)(png_get_rowbytes(png_ptr,info_ptr)/width);
    chanImg.setSize(width, height, numChannels );

    uchar *pixel = new uchar[ numChannels ];
    for(unsigned int i=0; i<height; i++) {
        for(unsigned int j=0; j<width; j++) {
            for(int k=0; k<numChannels; k++) {
                pixel[k] = row_pointers[i][numChannels*j+k];
            }
            chanImg.setChannels( j, i, pixel );
        }
    }
    delete [] pixel;

    /* finish reading */
    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return true;
}

bool GvsPngIO::writeChannelImg( GvsChannelImg2D& chanImg, const char *filename ) {
    png_structp   png_ptr;
    png_infop     info_ptr;

    FILE *fp;

    /* open the file */
#ifdef _WIN32
    fopen_s(&fp,filename,"wb");
#else
    fp = fopen(filename, "wb");
#endif
    if (fp == NULL) {
        fprintf(stderr,"Cannot open file %s for saving.\n",filename);
        return false;
    }

    /* Create and initialize the png_struct */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fclose(fp);
        return false;
    }

    /* Allocate/initialize the image information data */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, NULL);
        return false;
    }

    /* set error handling */
    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    /* set IO method */
    png_init_io(png_ptr, fp);

    int bitDepth    = 8;
    int colorType   = PNG_COLOR_TYPE_RGB;
    int numChannels = 3;

    png_set_IHDR(png_ptr, info_ptr,  chanImg.width(), chanImg.height(), bitDepth, colorType,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr,info_ptr);
    //  png_set_packing(png_ptr);

    GvsColor col;
    unsigned char** row_pointers = new unsigned char*[chanImg.height()];
    for( long i=0; i<chanImg.height(); i++) {
        row_pointers[i] = new unsigned char[chanImg.width()*numChannels];
        unsigned char* rp = row_pointers[i];
        for( long j=0; j<chanImg.width(); j++) {
            col = chanImg.sampleColor(j,i);
            *(rp++) = static_cast<unsigned char>(col.red*255.0);
            *(rp++) = static_cast<unsigned char>(col.green*255.0);
            *(rp++) = static_cast<unsigned char>(col.blue*255.0);
        }
    }

    png_write_image(png_ptr, (png_bytepp)row_pointers);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    for( int i=0; i<chanImg.height(); i++) {
        delete [] row_pointers[i];
    }
    delete [] row_pointers;
    return true;
}

#endif

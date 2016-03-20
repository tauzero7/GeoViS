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

#include <cstdlib>
#include <iostream>
#include "Img/GvsPpmIO.h"

GvsPpmIO :: GvsPpmIO() {
}

GvsPpmIO :: ~GvsPpmIO() {
}

bool GvsPpmIO :: readChannelImg( GvsChannelImg2D& chanImg, const char * filename) {
//    // cerr << "GvsPpmIO::readChannelImg..." << endl;

    FILE *fptr;
#ifdef _WIN32
    fopen_s(&fptr,filename,"r");
#else
    fptr = fopen(filename,"r");
#endif
    if (fptr==NULL) {
        fprintf(stderr,"Cannot read image %s.\n",filename);
        return false;
    }

    int width,height,c;
    char s[10];
#ifdef _WIN32
    fscanf_s(fptr,"%s\n%d %d\n%d\n",s,&width,&height,&c);
#else
    UNUSED_ATTR int r;
    r = fscanf(fptr,"%s\n%d %d\n%d\n",s,&width,&height,&c);
#endif

    if (width>0 && height>0) {
        char* image;
        if ((image = (char*)malloc(3*width*height*sizeof(char)))==NULL) {
            fprintf(stderr,"Cannot allocate memory for image.\n");
            fclose(fptr);
            return false;
        }
        size_t read = fread(image,sizeof(char),3*width*height,fptr);
        if (read != static_cast<size_t>(3*width*height)) {
            fprintf(stderr,"Possible error reading data!\n");
        }
        fclose(fptr);

        int numChannels = 3;
        chanImg.setSize(width, height, numChannels );
        uchar* col = new uchar[numChannels];

        char* iptr = image;
        for (int i = 0; i < height; i++) {
           for (int j = 0; j < width; j++) {
               col[0] = (uchar)(*iptr++);
               col[1] = (uchar)(*iptr++);
               col[2] = (uchar)(*iptr++);
               chanImg.setChannels(j,i,col);
           }
        }
        delete [] col;
        free(image);
    }
    return true;
}


bool GvsPpmIO :: writeChannelImg( GvsChannelImg2D& chanImg, const char *filename ) {
    // cerr << "GvsPpmIO :: writeChannelImg...\n";

    FILE *fptr;
#ifdef _WIN32
    fopen_s(&fptr,filename,"wb");
#else
    fptr = fopen(filename,"wb");
#endif
    if (fptr==NULL) {
        fprintf(stderr,"Cannot write image %s.\n",filename);
        return false;
    }

    int numChannels = chanImg.numChannels();

    if (numChannels!=3) {
        fprintf(stderr,"NumChannels!=3. Can't write Image as ppm!\n");
        return false;
    }

    int width  = chanImg.width();
    int height = chanImg.height();
    int maxval = 255;


#ifdef _WIN32
    fprintf_s(fptr,"P6\n%d %d\n%d\n",width,height,maxval);
#else
    fprintf(fptr,"P6\n%d %d\n%d\n",width,height,maxval);
#endif

    char* image;
    if ((image = (char*)malloc(3*width*height*sizeof(char)))==NULL) {
        fprintf(stderr,"Cannot allocate memory for image.\n");
        fclose(fptr);
        return false;
    }

    char* iptr = image;
    uchar* col = new uchar[numChannels];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            chanImg.sampleChannels(j,i,col);
            (*iptr++) = col[0];
            (*iptr++) = col[1];
            (*iptr++) = col[2];
        }
    }
    delete [] col;

    size_t written = fwrite(image,sizeof(char),3*width*height,fptr);
    if (written != static_cast<size_t>(3*width*height)) {
        fprintf(stderr,"Possible error writing data!\n");
    }
    fclose(fptr);
    free(image);
    return true;
}

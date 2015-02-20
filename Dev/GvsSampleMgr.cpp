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

#include <iostream>

#include "Cam/GvsCamera.h"
#include "Dev/GvsDevice.h"
#include "Dev/GvsSampleMgr.h"
#include "Dev/GvsProjector.h"
#include "Img/GvsPicIOEnvelope.h"

#include "Utils/GvsLog.h"
extern GvsLog& LOG;


GvsSampleMgr ::  GvsSampleMgr ( GvsDevice* rtDev, bool showProgress )
    : sampleDevice(rtDev),
      aspectRatio(1.0),
      mShowProgress(showProgress)
{
    assert(sampleDevice!=NULL);

    resX = 720;
    resY = 576;

    samplePicture = new GvsChannelImg2D( resX, resY, 3 );
    assert(samplePicture!=NULL);

    haveMask = false;
}

GvsSampleMgr :: ~GvsSampleMgr() {
    if (samplePicture!=NULL) {
        delete samplePicture;
        samplePicture = NULL;
    }
}

void GvsSampleMgr :: clear() {
    samplePicture->clear();
}


void GvsSampleMgr::setRegionToImage() {
    assert(sampleDevice!=NULL);
    sampleRegionLL.setX(0,0);
    sampleRegionLL.setX(1,0);
    sampleRegionUR.setX(0,sampleDevice->camera->GetResolution().x(0)-1);
    sampleRegionUR.setX(1,sampleDevice->camera->GetResolution().x(1)-1);

    bool withData = false;
    GvsCamFilter filter = sampleDevice->camera->getCamFilter();
    if ((filter == gvsCamFilterRGBpdz) ||
            (filter == gvsCamFilterRGBjac) ||
            (filter == gvsCamFilterRGBpt)) {
        withData = true;
    }
    resX = sampleDevice->camera->GetResolution().x(0);
    resY = sampleDevice->camera->GetResolution().x(1);
    samplePicture->resize( resX, resY, withData );
}


void GvsSampleMgr :: setRegion ( const m4d::ivec2 &corner1, const m4d::ivec2 &corner2 ) {
    sampleRegionLL = corner1;
    sampleRegionUR = corner2;

    if ( sampleRegionLL.x(0) < 0 ) sampleRegionLL.setX(0,0);
    if ( sampleRegionLL.x(1) < 0 ) sampleRegionLL.setX(1,0);
    if ( sampleRegionUR.x(0) < 0 ) sampleRegionUR.setX(0,0);
    if ( sampleRegionUR.x(1) < 0 ) sampleRegionUR.setX(1,0);

    m4d::ivec2 max = sampleDevice->camera->GetResolution() - m4d::ivec2(1,1);

    // Test if the region to be rendered is still in the image region
    if ( sampleRegionLL.x(0) > max.x(0) ) sampleRegionLL.setX(0,max.x(0));
    if ( sampleRegionLL.x(1) > max.x(1) ) sampleRegionLL.setX(1,max.x(1));
    if ( sampleRegionUR.x(0) > max.x(0) ) sampleRegionUR.setX(0,max.x(0));
    if ( sampleRegionUR.x(1) > max.x(1) ) sampleRegionUR.setX(1,max.x(1));

    // Is  LL < UR ?
    if ( sampleRegionLL.x(0) > sampleRegionUR.x(0) ) {
        int tmp = sampleRegionLL.x(0);
        sampleRegionLL.setX(0,sampleRegionUR.x(0));
        sampleRegionUR.setX(0,tmp);
    }

    if ( sampleRegionLL.x(1) > sampleRegionUR.x(1) ) {
        int tmp = sampleRegionLL.x(1);
        sampleRegionLL.setX(1,sampleRegionUR.x(1));
        sampleRegionUR.setX(1,tmp);
    }

    samplePicture->resize( sampleDevice->camera->GetResolution().x(0),
                           sampleDevice->camera->GetResolution().x(1) );
}


int GvsSampleMgr::calcRegionBytes( int x1, int y1, int x2, int y2 ) const {
    assert (x2 >= x1);
    assert (y2 >= y1);
    return (y2 -y1 + 1) * (x2 - x1 + 1) * samplePicture->numChannels();
}


int GvsSampleMgr::calcRegionPixels ( int x1, int y1, int x2, int y2 ) const {
    assert (x2 >= x1);
    assert (y2 >= y1);
    return (y2 -y1 + 1) * (x2 - x1 + 1);
}


bool GvsSampleMgr::putFirstPixel() {
    samplePixCoord = sampleRegionLL;        
    GvsColor pixcol = calcPixelColor( samplePixCoord.x(0), samplePixCoord.x(1) );
    samplePicture->setColor( samplePixCoord.x(0), samplePixCoord.x(1), pixcol );
    return true;
}


bool GvsSampleMgr::putNextPixel() {
    if ( ++samplePixCoord.x(0) > sampleRegionUR.x(0) ) {
        if ( ++samplePixCoord.x(1) > sampleRegionUR.x(1) ) return false;

        samplePixCoord.x(0) = sampleRegionLL.x(0);
    }

    GvsColor pixcol = calcPixelColor( samplePixCoord.x(0), samplePixCoord.x(1) );
    samplePicture->setColor( samplePixCoord.x(0), samplePixCoord.x(1), pixcol );
    return true;
}

GvsColor GvsSampleMgr :: calcPixelColor( int i, int j ) const {
    if (mShowProgress) {
        fprintf(stderr,"\r%4d %4d / %4d %4d",i+1,j+1,
                sampleDevice->camera->GetResolution().x(0),
                sampleDevice->camera->GetResolution().x(1));
    }

    bool doSample = true;
    if (haveMask) {
        int px = static_cast<int>( i*maskResX/static_cast<double>(resX) );
        int py = static_cast<int>( j*maskResY/static_cast<double>(resY) );
        double val = maskPicture.sampleValue(px,py);
        if (val < 0.9) {
            doSample = false;
        }
    }

    GvsColor col = RgbBlack;
    if (doSample) {
        col = sampleDevice->projector->getSampleColor( sampleDevice, double(i), double (j) );
    }
    return col;
}


void GvsSampleMgr :: extractRegion ( int x1, int y1, int x2, int y2, uchar* p ) const {
    assert (y2 >= y1);
    assert (x2 >= x1);

    int numChan = samplePicture->numChannels();
    uchar* pp = p;
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++, pp += numChan) {
            samplePicture->sampleChannels(x, y, pp);
        }
    }
}

void GvsSampleMgr::extractRegionData (int x1, int y1, int x2, int y2, uchar* p, gvsData *data ) const {
    assert(y2>=y1);
    assert(x2>=x1);

    int numChan = samplePicture->numChannels();
    uchar*   pptr = p;
    gvsData* dptr = data;
    for(int y=y1; y<=y2; y++) {
        for(int x=x1; x<=x2; x++, pptr+=numChan,dptr++) {
            samplePicture->sampleChannels(x,y,pptr);
            samplePicture->sampleData(x,y,dptr);
        }
    }
}


void GvsSampleMgr::setMask ( const char *filename ) {
    haveMask = GvsPicIOEnvelope().readChannelImg(maskPicture, filename);
    maskResX = maskPicture.width();
    maskResY = maskPicture.height();
}


void GvsSampleMgr :: writePicture( char *filename ) const {    
    if (sampleDevice->camEye == gvsCamEyeLeft) {
        std::string newFilename = std::string(filename);
        size_t pos = newFilename.find_last_of(".");
        if (pos != std::string::npos && pos>0) {
            std::string fileExt = newFilename.substr(pos+1,newFilename.length()-pos-1);
            std::string name = newFilename.substr(0,pos);
            newFilename = name + ".left." + fileExt;
            GvsPicIOEnvelope().writeChannelImg( *samplePicture, newFilename.c_str() );
        }
    }
    else if (sampleDevice->camEye == gvsCamEyeRight) {
        std::string newFilename = std::string(filename);
        size_t pos = newFilename.find_last_of(".");
        if (pos != std::string::npos && pos>0) {
            std::string fileExt = newFilename.substr(pos+1,newFilename.length()-pos-1);
            std::string name = newFilename.substr(0,pos);
            newFilename = name + ".right." + fileExt;
            GvsPicIOEnvelope().writeChannelImg( *samplePicture, newFilename.c_str() );
        }
    }
    else {
        GvsPicIOEnvelope().writeChannelImg( *samplePicture, filename );
        GvsCamFilter filter = sampleDevice->camera->getCamFilter();
        if ((filter == gvsCamFilterRGBpdz) ||
                (filter == gvsCamFilterRGBjac) ||
                (filter == gvsCamFilterRGBpt)) {
            samplePicture->writeIntersecData(filename,filter);
        }
    }
    return;
}

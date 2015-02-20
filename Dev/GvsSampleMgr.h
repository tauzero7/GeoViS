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
#ifndef GVS_SAMPLE_MGR_H
#define GVS_SAMPLE_MGR_H

#include <iostream>

#include "GvsGlobalDefs.h"
#include "Img/GvsColor.h"
#include "Img/GvsChannelImg2D.h"

#include "m4dGlobalDefs.h"

class GvsDevice;

/**
 * @brief  The sample manager is responsible for determining the color
 *         of each pixel.
 */
class GvsSampleMgr
{
public:
    GvsSampleMgr ( GvsDevice* rtDev, bool showProgress = false );
    virtual ~GvsSampleMgr ( );

    void     clear();

    void     setRegionToImage ( );
    void     setRegion        ( const m4d::ivec2 &corner1, const m4d::ivec2 &corner2 );
    int      calcRegionBytes  ( int x1, int y1, int x2, int y2 ) const;
    int      calcRegionPixels ( int x1, int y1, int x2, int y2 ) const;

    bool     putFirstPixel  ( );
    bool     putNextPixel   ( );

    /**
     * For each individual pixel (i,j), the projector is instructed to determine
     * the color and additional data like frequency shift etc.
     * @param i  Horizontal pixel id.
     * @param j  Vertical pixel id.
     * @return  Color of the pixel.
     */
    GvsColor calcPixelColor ( int i, int j ) const;

    void     extractRegion     ( int x1, int y1, int x2, int y2, uchar* p ) const;
    void     extractRegionData ( int x1, int y1, int x2, int y2, uchar* p, gvsData* data ) const;

    void     setMask ( const char *filename );

    void     writePicture( char *filename ) const; //!< Write picture to disc.

protected:
    int  resX;
    int  resY;
    m4d::ivec2   sampleRegionLL;    //!< Lower left corner of sample region
    m4d::ivec2   sampleRegionUR;    //!< Upper right corner of sample region
    m4d::ivec2   samplePixCoord;    //!< pixel which has to be calculated

    GvsChannelImg2D*  samplePicture;
    GvsDevice*        sampleDevice;
    double            aspectRatio;
    bool         mShowProgress;

    GvsChannelImg2D   maskPicture;
    int               maskResX;
    int               maskResY;
    bool              haveMask;
};

#endif

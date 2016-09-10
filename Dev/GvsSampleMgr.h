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
#include "Img/GvsIntersecOutput.h"

#include "m4dGlobalDefs.h"

class GvsDevice;

/**
 * The sample manager is responsible for determining the color of each pixel.
 */
class API_EXPORT GvsSampleMgr
{
public:
    GvsSampleMgr ( GvsDevice* rtDev, bool showProgress = false );
    virtual ~GvsSampleMgr ( );

    void clear();

    /**
     * Set region to image.
     *   Resize the internal image to fit the camera resolution of the scene.
     */
    void  setRegionToImage();

    /**
     * Set region.
     *   Resize the internal image to fit the camera resolution of the scene.
     *   However, the region to be sampled is restricted by the lower-left and
     *   upper-right corners.
     * @param corner1   lower-left corner of sampling region
     * @param corner2   upper-right corner of sampling region
     */
    void  setRegion(const m4d::ivec2 &corner1, const m4d::ivec2 &corner2);

    /**
     * Calculate the number of bytes necessary for the region defined by x_i,y_i
     * @param x1   x-coordinate of lower-left corner
     * @param y1   y-coordinate of lower-left corner
     * @param x2   x-coordinate of upper-right corner
     * @param y2   y-coordinate of upper-right corner
     * @return  number of bytes
     */
    int  calcRegionBytes ( int x1, int y1, int x2, int y2 ) const;

    /**
     * Calculate the number of pixel covered by the region
     * @param x1   x-coordinate of lower-left corner
     * @param y1   y-coordinate of lower-left corner
     * @param x2   x-coordinate of upper-right corner
     * @param y2   y-coordinate of upper-right corner
     * @return number of pixels
     */
    int  calcRegionPixels ( int x1, int y1, int x2, int y2 ) const;

    /**
     * Put first pixel.
     *   Render the pixel of the lower-left corner.
     * @return always true
     */
    bool  putFirstPixel();

    /**
     * Put next pixel.
     *    Render the next pixel of the region. Call this function within a while-loop
     *    until false is returned.
     * @return  true if there is another pixel to be rendered, false if the region is finished
     */
    bool  putNextPixel();

    /**
     * For each individual pixel (i,j), the projector is instructed to determine
     * the color and additional data like frequency shift etc.
     * @param i  Horizontal pixel id.
     * @param j  Vertical pixel id.
     * @return  Color of the pixel.
     */
    GvsColor calcPixelColor ( int i, int j ) const;
    void     calcPixelIntersections(int i, int j);

    /**
     * Read image pixels from the region defined by x_i,y_i.
     *   Note: p must point to an allocated memory block which can hold the data of the region.
     * @param x1   x-coordinate of lower-left corner
     * @param y1   y-coordinate of lower-left corner
     * @param x2   x-coordinate of upper-right corner
     * @param y2   y-coordinate of upper-right corner
     * @param p  pointer to memory where data from the image will be copied
     */
    void  extractRegion     ( int x1, int y1, int x2, int y2, uchar* p ) const;

    /**
     * Read image pixels from the region defined by x_i,y_i as well as intersection data.
     *   This method can only be used for camera filters RGBpdz, RGBjac, RGBpt.
     * @param x1   x-coordinate of lower-left corner
     * @param y1   y-coordinate of lower-left corner
     * @param x2   x-coordinate of upper-right corner
     * @param y2   y-coordinate of upper-right corner
     * @param p  pointer to memory where data from the image will be copied
     * @param data  pointer to memory where data from the intersections will be copied
     */
    void  extractRegionData ( int x1, int y1, int x2, int y2, uchar* p, gvsData* data ) const;

    /**
     * Set mask.
     *   A mask is used to constrain the calculation of pixels to those regions, where
     *   the mask image is not black.
     * @param filename
     */
    void  setMask(const char *filename);

    /**
     * Write picture to file.
     * @param filename
     */
    void  writePicture( char *filename ) const;

    /**
     * Writee intersection data to file.
     * @param filename
     */
    void  writeIntersecData(char* filename) const;

protected:
    int  resX;
    int  resY;
    m4d::ivec2   sampleRegionLL;    //!< Lower left corner of sample region
    m4d::ivec2   sampleRegionUR;    //!< Upper right corner of sample region
    m4d::ivec2   samplePixCoord;    //!< pixel which has to be calculated

    GvsChannelImg2D*  samplePicture;
    GvsIntersecOutput*  sampleIntersecPicture;
    GvsDevice*        sampleDevice;
    double            aspectRatio;
    bool         mShowProgress;

    GvsChannelImg2D   maskPicture;
    int               maskResX;
    int               maskResY;
    bool              haveMask;
};

#endif

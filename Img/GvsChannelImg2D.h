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
#ifndef Gvs_CHANNEL_IMG_2D_H
#define Gvs_CHANNEL_IMG_2D_H

#include "GvsGlobalDefs.h"
#include "Img/GvsColor.h"


enum GvsImgOrder {
    gvsIMOblock = 0, gvsIMOrow, gvsIMOunknown
};

static const std::string GvsImgOrderName[3] = {"block","row","unknown"};

enum GvsWrapPolicy {
    GVS_WP_CLAMP, GVS_WP_PERIODIC, GVS_WP_CONST_COLOR
};

static const std::string GvsWrapPolicyName[3] = {"clamp","periodic","const_color"};

/**
 * @brief The GvsChannelImg2D class
 */
class API_EXPORT GvsChannelImg2D
{
public:
    GvsChannelImg2D ();
    GvsChannelImg2D ( int width, int height, int numChannels, bool withData = false );
    virtual ~GvsChannelImg2D();

    GvsChannelImg2D ( const GvsChannelImg2D& T );

    GvsChannelImg2D& operator= ( const GvsChannelImg2D&  );

    // Gets the luminance of one pixel (i,j). Uses luminance()-member of GvsColor
    virtual double   sampleValue    ( int i, int j ) const;
    virtual double   sampleValue    ( double u, double v ) const;
    virtual double   sampleValueDirectly ( int i, int j ) const;
    virtual double   sampleValueDirectly ( double u, double v ) const;

    // Gets RGB of one pixel (i,j). Values are between 0..1
    virtual GvsColor sampleColor    ( int i, int j ) const;
    virtual GvsColor sampleColor    ( double u, double v ) const;
    virtual GvsColor sampleColorDirectly ( int i, int j ) const;
    virtual GvsColor sampleColorDirectly ( double u, double v ) const;

    // Gets values of imgData for one pixel (i,j). Values are written to uchar* and are between 0..255
    virtual void     sampleChannels ( int x, int y, uchar* col ) const;
    virtual void     sampleData     ( int x, int y, gvsData* dat ) const;

    virtual void     setBlock    ( int i, int j, int width, int height, uchar* block );
    virtual void     sampleBlock ( int i, int j, int width, int height, uchar* block ) const;
    virtual void     setBlock    ( int i, int j, int width, int height, double* block );
    virtual void     sampleBlock ( int i, int j, int width, int height, double* block ) const;

    virtual void     setDataBlock( int i, int j, int width, int height, gvsData* block );

    // Gets the image pointer
    virtual uchar*   getImagePtr ( ) const;


    // Sets the luminance of one pixel (i,j) to uchar(val*255), i.e. red=uchar(val*255), green=uchar(val*255) ect.
    virtual void setValue    ( int i, int j, double val );

    // Sets the color of one pixel (i,j). Entries of col have to be 0..1, entries in imgData become 0..255
    virtual void setColor    ( int i, int j, const GvsColor& col );

    // Same as above, but entries of col have to be 0..255
    virtual void setChannels ( int i, int j, uchar *col );
    virtual void setChannel  ( int x, int y, int chNum, uchar ch );


    // calls deleteAll() and creates an empty imgData[width*size*numChannels]
    virtual void setSize  ( int width, int height, int numChannels );
    virtual void resize   ( int newWidth, int newHeight, bool resizeData = false );

    // corrects gamma for an existing imgData. gamma has to be !=0 and !=1
    void     correctGamma ( double gamma );

    // Copy an existing image (no pointer bending)
    void     copyFrom     ( const GvsChannelImg2D& T );



    virtual void clear   ( );
    virtual void clear   ( int left, int bottom, int right, int top );


    int width       ( void ) const;
    int height      ( void ) const;
    int numChannels ( void ) const;

    void     setFileName     ( const char *filename );
    char*    getImgFilename  ( ) const;

    void        blockToRow();  // rearrange the block structure  pix(0,0)rgb pix(0,1)rgb ...
    void        rowToBlock();
    GvsImgOrder getImgOrder() const;


    void   setWrappingPolicy     ( GvsWrapPolicy x_wrap, GvsWrapPolicy y_wrap );
    void   setWrappingConstColor ( const GvsColor &col );
    void   setWrappingConstValue ( double val );
    void   setWrapConstValue     ( uchar *col ) const;

    //! Write intersection data to file.
    void   writeIntersecData( const char *filename, GvsCamFilter filter );

    virtual void Print ( FILE* fptr = stderr );

protected:
    // this one is used for the copy constructor and the overloaded "="
    // It acts like a copy-constructor itself (deep copy). A new imgData is created,
    // it has the same properties and data like the reference of this member.
    virtual void assign ( const GvsChannelImg2D&  );


    virtual void deleteAll ( void );    // deletes imgData[] and sets the rest to 0

    uchar* imgData;
    int    imgWidth;
    int    imgHeight;
    int    imgNumChannels;
    char*  imgFileName;
    int    imgChanSize;   	// width * height, the size of one "color-datablock", see ".EXPLANATION"

    GvsImgOrder imgDataOrder;    // block structure is standard image format

    GvsWrapPolicy imgWrapPolicyX;
    GvsWrapPolicy imgWrapPolicyY;
    double        imgWrapConstValue;
    GvsColor      imgWrapConstColor;

    bool      imgWithData;
    gvsData*  imgIntersecData;
};

#endif

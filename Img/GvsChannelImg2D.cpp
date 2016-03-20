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

#include "GvsChannelImg2D.h"
#include <cassert>
#include <iostream>


GvsChannelImg2D :: GvsChannelImg2D()
    : imgData(NULL),
      imgChanSize(0),
      imgWithData(false),
      imgIntersecData(NULL)
{
    imgWidth = imgHeight = 0;

    imgNumChannels = 0;
    imgDataOrder = gvsIMOblock;
    imgFileName = NULL;

    imgWrapPolicyX = imgWrapPolicyY = GVS_WP_PERIODIC;
    imgWrapConstValue = 0.0;

    imgData = NULL;
}


GvsChannelImg2D :: GvsChannelImg2D(int width, int height, int numChannels , bool withData)
    : imgWidth(width),
      imgHeight(height),
      imgFileName(NULL),
      imgWithData(withData),
      imgIntersecData(NULL)
{
    imgChanSize    = width * height;
    imgNumChannels = numChannels;
    imgDataOrder = gvsIMOblock;
    imgFileName = NULL;

    imgWrapPolicyX = imgWrapPolicyY = GVS_WP_PERIODIC;
    imgWrapConstValue = 0.0;

    imgData = new uchar[ imgChanSize * imgNumChannels ];
    for (int i=0; i<imgChanSize*imgNumChannels; i++) {
        imgData[i] = (uchar)0;
    }
    assert( imgData != NULL );

    if (imgWithData) {
        imgIntersecData = new gvsData[imgChanSize];
        for(int i=0; i<imgChanSize; i++) {
            imgIntersecData[i] = gvsData_T();
        }
    }
}


void GvsChannelImg2D :: setSize( int width, int height, int numChannels ) {
    //  std::cerr << "GvsChennelImg2D::setSize...\n";
    deleteAll();

    imgWidth       = width;
    imgHeight      = height;
    imgChanSize    = width * height;
    imgNumChannels = numChannels;

    imgData = new uchar[ imgChanSize * imgNumChannels ];
    assert( imgData != NULL );
}


void GvsChannelImg2D :: resize(int newWidth, int newHeight , bool resizeData) {

    if ( (imgWidth != newWidth) || (imgHeight != newHeight) )  {
        uchar *newData = new uchar[ newWidth * newHeight * imgNumChannels ];
        assert( newData != NULL );

        int indx = 0;
        int newChanSize = newWidth * newHeight;

        double newWidthD  = double( newWidth  );
        double newHeightD = double( newHeight );

        double factorX = double( imgWidth  ) / newWidthD;
        double factorY = double( imgHeight ) / newHeightD;

        GvsColor interpolColor;

        for ( double y = 0.0; y < newHeightD; y += 1.0 ) {
            double oldY = y * factorY;

            for ( double x = 0.0; x < newWidthD; x += 1.0 ) {
                double oldX = x * factorX;

                // interpolColor = sampleColor( oldX, oldY );
                interpolColor = sampleColor( int(oldX+0.5), int(oldY+0.5) );//FG (richtiges Runden)

                newData[ indx                 ] = uchar( interpolColor.red  *255.0 );
                newData[ indx + newChanSize   ] = uchar( interpolColor.green*255.0 );
                newData[ indx + newChanSize*2 ] = uchar( interpolColor.blue *255.0 );

                indx++;
            }
        }

        delete [] imgData;
        imgData = newData;
        imgWidth = newWidth;
        imgHeight = newHeight;
        imgChanSize = newChanSize;

        if (resizeData) {
            imgWithData = true;
            //std::cerr << "+++RESIZE DATA\n";
            if (imgIntersecData!=NULL) {
                delete [] imgIntersecData;
            }
            imgIntersecData = new gvsData[imgChanSize];
        }
    }
}


void GvsChannelImg2D :: assign( const GvsChannelImg2D& T ) {

    deleteAll();

    imgWidth       = T.width();
    imgHeight      = T.height();
    imgChanSize    = imgWidth * imgHeight;
    imgNumChannels = T.numChannels();

    if ( imgWidth > 0 && imgHeight > 0 ) {
        imgData = new uchar[ imgChanSize * imgNumChannels ];
        assert( imgData != NULL );
        //memcpy( imgData, T.imgData, imgChanSize * imgNumChannels );

        uchar *col = new uchar[ imgNumChannels ];
        for ( int x = 0; x < imgWidth; x++ ) {
            for ( int y = 0; y < imgHeight; y++ ) {
                T.sampleChannels( x, y, col );
                setChannels( x, y, col );
            }
        }
        delete [] col;
    }
    else {
        imgData = NULL;
    }
}


GvsChannelImg2D :: GvsChannelImg2D( const GvsChannelImg2D& T )
    : imgData(NULL),
      imgChanSize(0)
{
    assign( T );
}


GvsChannelImg2D& GvsChannelImg2D :: operator=( const GvsChannelImg2D& T )
{
    deleteAll();
    assign( T );
    return *this;
}


GvsChannelImg2D :: ~GvsChannelImg2D() {
    deleteAll();
}


void GvsChannelImg2D :: deleteAll() {
    //  std::cerr << "deleteAll...\n";
    imgWidth = imgHeight = imgChanSize = imgNumChannels = 0;
    if (imgData!=NULL) {
        delete [] imgData;
        imgData = NULL;
    }

    if (imgIntersecData!=NULL) {
        delete [] imgIntersecData;
        imgIntersecData = NULL;
    }
}


double GvsChannelImg2D :: sampleValue ( double u, double v ) const {
    int x = (int)(u*imgWidth + 0.5);
    int y = (int)(v*imgHeight + 0.5);
    return sampleValue(x,y);
}


double GvsChannelImg2D :: sampleValue ( int x, int y ) const {
    if (x < 0) {
        switch (imgWrapPolicyX) {
            case GVS_WP_CLAMP:
                x = 0;
                break;
            case GVS_WP_PERIODIC:
                x %= imgWidth;
                if (x<0) x+=imgWidth;
                break;
            case GVS_WP_CONST_COLOR:
            default:
                return imgWrapConstValue;
        }
    }
    else if (x >= imgWidth) {
        switch (imgWrapPolicyX) {
            case GVS_WP_CLAMP:
                x = imgWidth-1;
                break;
            case GVS_WP_PERIODIC:
                x %= imgWidth;
                break;
            case GVS_WP_CONST_COLOR:
            default:
                return imgWrapConstValue;
        }
    }

    if (y < 0) {
        switch (imgWrapPolicyY) {
            case GVS_WP_CLAMP:
                y = 0;
                break;
            case GVS_WP_PERIODIC:
                y %= imgHeight;
                if (y<0) y+=imgHeight;
                break;
            case GVS_WP_CONST_COLOR:
            default:
                return imgWrapConstValue;
        }
    }
    else if (y >= imgHeight) {
        switch (imgWrapPolicyY) {
            case GVS_WP_CLAMP:
                y = imgHeight-1;
                break;
            case GVS_WP_PERIODIC:
                y %= imgHeight;
                break;
            case GVS_WP_CONST_COLOR:
            default:
                return imgWrapConstValue;
        }
    }

    return sampleValueDirectly(x,y);
}


double GvsChannelImg2D :: sampleValueDirectly ( double u, double v ) const {
    // The values of u,v are in the interval [0,1]. Here, u and v
    // are transformed to pixel coordinates.
    int x = (int)(u*imgWidth + 0.5);
    int y = (int)(v*imgHeight + 0.5);
    return sampleValueDirectly(x,y);
}


double GvsChannelImg2D :: sampleValueDirectly ( int x, int y ) const {
    static double inv255 = 1.0 / 255.0;
    int indx = y * imgWidth + x;

    switch ( imgNumChannels ) {
        case 1:
            return imgData[ indx ] * inv255;

        case 4:
        case 3:
            return GvsColor( imgData[ indx                 ] * inv255,
                             imgData[ indx + imgChanSize   ] * inv255,
                    imgData[ indx + imgChanSize*2 ] * inv255  ).luminance();


        case 2:
            return GvsColor( imgData[ indx               ] * inv255,
                             imgData[ indx + imgChanSize ] * inv255,
                    0.0                                       ).luminance();
        default:
            std::cerr << "Error in GvsChannelImg2D::sampleValueDirectly(): "
                      << "unsupported # of channels: " << imgNumChannels << std::endl;
            return 0.0;
    }
}


GvsColor GvsChannelImg2D :: sampleColor ( double u, double v ) const {
    int x = (int)(u*imgWidth + 0.5);
    int y = (int)(v*imgHeight + 0.5);
    return sampleColor(x,y);
}


GvsColor GvsChannelImg2D :: sampleColor ( int x, int y ) const {
    if (x < 0) {
        switch (imgWrapPolicyX) {
            case GVS_WP_CLAMP:
                x = 0;
                break;
            case GVS_WP_PERIODIC:
                x %= imgWidth;
                if (x<0) x+=imgWidth;
                break;
            case GVS_WP_CONST_COLOR:
            default:
                return imgWrapConstColor;
        }
    }
    else if (x >= imgWidth) {
        switch (imgWrapPolicyX) {
            case GVS_WP_CLAMP:
                x = imgWidth-1;
                break;
            case GVS_WP_PERIODIC:
                x %= imgWidth;
                break;
            case GVS_WP_CONST_COLOR:
            default:
                return imgWrapConstColor;
        }
    }

    if (y < 0) {
        switch (imgWrapPolicyY) {
            case GVS_WP_CLAMP:
                y = 0;
                break;
            case GVS_WP_PERIODIC:
                y %= imgHeight;
                if (y<0) y+=imgHeight;
                break;
            case GVS_WP_CONST_COLOR:
            default:
                return imgWrapConstColor;
        }
    }
    else if (y >= imgHeight) {
        switch (imgWrapPolicyY) {
            case GVS_WP_CLAMP:
                y = imgHeight-1;
                break;
            case GVS_WP_PERIODIC:
                y %= imgHeight;
                break;
            case GVS_WP_CONST_COLOR:
            default:
                return imgWrapConstColor;
        }
    }
    return sampleColorDirectly(x,y);
}


GvsColor GvsChannelImg2D :: sampleColorDirectly ( double u, double v ) const {
    int x = (int)(u*imgWidth  + 0.5);
    int y = (int)(v*imgHeight + 0.5);

    return sampleColorDirectly(x,y);
}


GvsColor GvsChannelImg2D :: sampleColorDirectly ( int x, int y ) const {
    static double inv255 = 1.0 / 255.0;
    int indx = y * imgWidth + x;

    switch ( imgNumChannels ) {
        case 3: {
            return GvsColor( imgData[ indx                 ] * inv255,
                             imgData[ indx + imgChanSize   ] * inv255,
                    imgData[ indx + imgChanSize*2 ] * inv255  );
        }
        case 4: {
            return GvsColor( imgData[ indx                 ] * inv255,
                             imgData[ indx + imgChanSize   ] * inv255,
                    imgData[ indx + imgChanSize*2 ] * inv255,
                    imgData[ indx + imgChanSize*3 ]);
        }
        case 1: {
            float val = imgData[ indx ] * inv255;
            return GvsColor( val, val, val );
        }
        case 2: {
            return GvsColor( imgData[ indx               ] * inv255,
                             imgData[ indx + imgChanSize ] * inv255,
                              0.0 );
        }
        default: {
            std::cerr << "Error in GvsChannelImg2D::sampleColorDirectly(): "
                      << "unsupported # of channels: " << imgNumChannels << std::endl;
            return RgbBlack;
        }
    }
}


void GvsChannelImg2D::sampleChannels( int x, int y, uchar *col ) const {
    if (x<0 || x>=imgWidth || y<0 || y>=imgHeight) {
        std::cerr << "Error in GvsChannelImg2D::sampleChannels : index out "
                  << "of bounds (imgWidth = " << imgWidth << ", imgHeight = "
                  << imgHeight << ", x = " << x << ", y = " << y << ")" << std::endl;
        x = y = 0;
    }

    int indx = y*imgWidth+x;
    for(int i=0; i<imgNumChannels; i++) {
        col[i] = imgData[indx];
        indx+=imgChanSize;
    }
}


void GvsChannelImg2D::sampleData( int x, int y, gvsData* dat ) const {
    if (x<0 || x>=imgWidth || y<0 || y>=imgHeight) {
        std::cerr << "Error in GvsChannelImg2D::sampleChannels : index out "
                  << "of bounds (imgWidth = " << imgWidth << ", imgHeight = "
                  << imgHeight << ", x = " << x << ", y = " << y << ")" << std::endl;
        x = y = 0;
    }
    int indx = y*imgWidth+x;
    if (imgIntersecData!=NULL) {
        memcpy(dat,&imgIntersecData[indx],sizeof(gvsData)*1);
    }
}


void GvsChannelImg2D::setDataBlock( int i, int j, int width, int height, gvsData* block ) {
    if (!imgWithData) {
        return;
    }
    assert(block!=NULL);
    assert(((i+width)<=imgWidth) && ((j+height)<=imgHeight) && (i>=0) && (j>=0) && (width>0) && (height>0));

    gvsData* bptr = block;
    for(int y=j; y<(j+height); y++) {
        for(int x=i; x<(i+width); x++) {
            if (imgIntersecData!=NULL && block!=NULL) {
                memcpy(&imgIntersecData[y*imgWidth+x],bptr,sizeof(gvsData)*1);
            }
            bptr++;
        }
    }
}

//  block is composed of  rgb rgb rgb
//  in contrast to the imgData
void GvsChannelImg2D::setBlock ( int i, int j, int width, int height, uchar* block ) {
    assert(block!=NULL);
    assert(((i+width)<=imgWidth) && ((j+height)<=imgHeight) && (i>=0) && (j>=0) && (width>0) && (height>0));

    uchar* col = new uchar[imgNumChannels];
    int num=0;
    for (int n=j; n<(j+height); n++) {
        for (int m=i; m<(i+width); m++) {
            for (int c=0; c<imgNumChannels; c++) {
                col[c]=block[num+c];
            }
            setChannels(m,n,col);
            num+=imgNumChannels;
        }
    }
    delete [] col;
}


void GvsChannelImg2D::sampleBlock( int i, int j, int width, int height, uchar* block) const {
    //  std::cerr << "GvsChannelImg2D::sampleBlock...\n";
    assert(block!=NULL);
    assert((i+width)<=imgWidth && (j+height)<=imgHeight && i>=0 && j>=0 && width>0 && height>0);

    uchar* col = new uchar[imgNumChannels];
    int num=0;
    for (int n=j; n<(j+height); n++) {
        for (int m=i; m<(i+width); m++) {
            sampleChannels(m,n,col);
            for (int c=0; c<imgNumChannels; c++) {
                block[num+c]=col[c];
            }
            num+=imgNumChannels;
        }
    }

    delete [] col;
}


void GvsChannelImg2D::setBlock ( int i, int j, int width, int height, double* block ) {
    assert(block!=NULL);
    assert(((i+width)<=imgWidth) && ((j+height)<=imgHeight) && (i>=0) && (j>=0) && (width>0) && (height>0));

    uchar* col = new uchar[imgNumChannels];
    int num=0;
    for (int n=j; n<(j+height); n++) {
        for (int m=i; m<(i+width); m++) {
            for (int c=0; c<imgNumChannels; c++) {
                col[c]=uchar(block[num+c]*255.0);
            }
            setChannels(m,n,col);
            num+=imgNumChannels;
        }
    }

    delete [] col;
}


void GvsChannelImg2D::sampleBlock( int i, int j, int width, int height, double* block) const {
    //  std::cerr << "GvsChannelImg2D::sampleBlock...\n";
    assert(block!=NULL);
    assert((i+width)<=imgWidth && (j+height)<=imgHeight && i>=0 && j>=0 && width>0 && height>0);

    uchar* col = new uchar[imgNumChannels];
    int num=0;
    for (int n=j; n<(j+height); n++) {
        for (int m=i; m<(i+width); m++) {
            sampleChannels(m,n,col);
            for (int c=0; c<imgNumChannels; c++) {
                block[num+c]=uchar(col[c]*255.0);
            }
            num+=imgNumChannels;
        }
    }

    delete [] col;
}


uchar* GvsChannelImg2D :: getImagePtr ( ) const {
    return imgData;
}


void GvsChannelImg2D :: setValue( int x, int y, double val )
{
    int indx = y * imgWidth + x;

    switch ( imgNumChannels ) {
        case 1: {
            imgData[ indx ] = uchar( val * 255.0 );
            break;
        }
        case 4:
        case 3: {
            imgData[ indx ] =
                    imgData[ indx + imgChanSize   ] =
                    imgData[ indx + imgChanSize*2 ] = uchar( val * 255.0 );
            break;
        }
        case 2: {
            imgData[ indx ] =
                    imgData[ indx + imgChanSize ] = uchar( val * 255.0 );
            break;
        }
        default: {
            std::cerr << "Error in GvsChannelImg2D::setVal(): "
                      << "unsupported # of channels: " << imgNumChannels << std::endl;
            break;
        }
    }
}


void GvsChannelImg2D :: setColor( int x, int y, const GvsColor& col )
{
    // col.Print();
    int indx = y * imgWidth + x;

    switch ( imgNumChannels ) {
        case 3: {
            imgData[ indx ] = uchar( col.red   * 255.0 );
            indx += imgChanSize;
            imgData[ indx ] = uchar( col.green * 255.0 );
            indx += imgChanSize;
            imgData[ indx ] = uchar( col.blue  * 255.0 );
            break;
        }
        case 4: {
            imgData[ indx ] = uchar( col.red   * 255.0 );
            indx += imgChanSize;
            imgData[ indx ] = uchar( col.green * 255.0 );
            indx += imgChanSize;
            imgData[ indx ] = uchar( col.blue  * 255.0 );
            indx += imgChanSize;
            imgData[ indx ] = uchar( col.alpha * 255.0 );
            break;
        }
        case 1: {
            imgData[ indx ] = uchar( col.luminance()  * 255.0 );
            break;
        }
        case 2: {
            imgData[ indx ] = uchar( col.red   * 255.0 );
            indx += imgChanSize;
            imgData[ indx ] = uchar( col.green * 255.0 );
            break;
        }
        default: {
            std::cerr << "Error in GvsChannelImg2D::setColor(): "
                      << "unsupported # of channels: " << imgNumChannels << std::endl;
            break;
        }
    }

    if (imgWithData && imgIntersecData!=NULL) {
         imgIntersecData[y * imgWidth + x] = col.data;
    }
}


void GvsChannelImg2D :: setChannels( int x, int y, uchar *col ) {
    int indx = y * imgWidth + x;
    for ( int i = 0; i < imgNumChannels; i++ ) {
        imgData[indx] = col[i];
        indx += imgChanSize;
    }
}


void GvsChannelImg2D :: setChannel  ( int x, int y, int chNum, uchar ch ) {
    int indx = y * imgWidth + x;
    imgData[indx+imgChanSize*chNum] = ch;
}


void GvsChannelImg2D :: correctGamma ( double gamma ) {
    const double maxVal    = UCHAR_MAX;     // 255.0
    const double invMaxVal = 1.0 / maxVal;  // 1.0 / 255.0

    double invGamma;

    if ( gamma == 0.0 || gamma == 1.0 ) {
        return;
    }
    invGamma = 1.0 / gamma;

    for ( int i = 0; i < imgChanSize * imgNumChannels; i++ ) {
        if ( imgData[i] != 0U ) {
            imgData[i] = uchar( maxVal *
                                pow( double(imgData[i]) * invMaxVal, invGamma) +
                                0.5 );
        }
    }
}


void GvsChannelImg2D :: copyFrom ( const GvsChannelImg2D& T ) {
    int minWidth = (imgWidth < T.imgWidth) ? imgWidth : T.imgWidth;
    int minHeight = (imgHeight < T.imgHeight) ? imgHeight : T.imgHeight;

    for ( int y = 0; y < minHeight; y++ ) {
        for ( int x = 0; x < minWidth; x++ ) {
            setColor( x, y, T.sampleColor( x, y ) );
        }
    }
}


void GvsChannelImg2D :: clear() {
    clear( 0, 0, imgWidth-1, imgHeight-1 );
}


void GvsChannelImg2D :: clear ( int left, int bottom, int right, int top ) {
    for (int x=left; x<=right; x++) {
        for (int y=bottom; y<top; y++) {
            setColor(x,y,GvsColor(0.0));
        }
    }
}


int GvsChannelImg2D::width () const {
    return imgWidth;
}

int GvsChannelImg2D::height () const {
    return imgHeight;
}

int GvsChannelImg2D::numChannels () const {
    return imgNumChannels;
}


void GvsChannelImg2D :: setFileName ( const char *filename ) {    
    if (imgFileName != NULL) {
        delete[] imgFileName;
    }

    imgFileName = new char [ strlen( filename ) + 1 ];
    assert( imgFileName != NULL );

    char *newImgFileName = imgFileName;
    while ((*newImgFileName++ = *filename++));    // copy string
}


void GvsChannelImg2D :: blockToRow() {
    std::cerr << "GvsChannelImg2D :: blockToRow()...\n";

    if (imgDataOrder == gvsIMOrow) {
        std::cerr << "Image is already in row order!\n";
    }
    else {
        uchar* col = new uchar[imgNumChannels];

        uchar* newImgData = new uchar[imgWidth*imgHeight*imgNumChannels];
        for (int i=0; i<imgHeight; i++) {
            for (int j=0; j<imgWidth; j++) {
                for (int k=0; k<imgNumChannels; k++) {
                    sampleChannels(j,i,col);
                    newImgData[imgNumChannels*(i*imgWidth+j)+k] = col[k];
                }
            }
        }
        delete [] imgData;
        imgData = newImgData;
        delete [] col;

        imgDataOrder = gvsIMOrow;
    }
}


void GvsChannelImg2D :: rowToBlock() {
    std::cerr << "GvsChannelImg2D :: rowToBlock()...\n";
    if (imgDataOrder == gvsIMOblock) {
        std::cerr << "Image is already in block order!\n";
    }
    else {
        uchar* newImgData = new uchar[imgWidth*imgHeight*imgNumChannels];
        for (int i=0; i<imgHeight; i++) {
            for (int j=0; j<imgWidth; j++) {
                for (int k=0; k<imgNumChannels; k++) {
                    newImgData[k*imgWidth*imgHeight+(i*imgWidth+j)] = imgData[imgNumChannels*(i*imgWidth+j)+k];
                }
            }
        }
        delete [] imgData;
        imgData = newImgData;
        imgDataOrder = gvsIMOblock;
    }
}


GvsImgOrder GvsChannelImg2D :: getImgOrder() const {
    return imgDataOrder;
}


char* GvsChannelImg2D :: getImgFilename  () const {
    return imgFileName;
}


void GvsChannelImg2D :: setWrappingPolicy ( GvsWrapPolicy x_wrap, GvsWrapPolicy y_wrap ) {
    imgWrapPolicyX = x_wrap;
    imgWrapPolicyY = y_wrap;
}


void GvsChannelImg2D :: setWrappingConstColor ( const GvsColor &col ) {
    imgWrapConstColor = col;
    imgWrapConstValue = col.luminance();
}


void GvsChannelImg2D :: setWrappingConstValue ( double val ) {
    imgWrapConstValue = val;
    imgWrapConstColor = GvsColor(val);
}

void GvsChannelImg2D :: setWrapConstValue( uchar *col ) const {
    uchar ucharval = uchar( imgWrapConstValue * 255.0 );

    for (int i=0; i<imgNumChannels; i++)
        col[i] = ucharval;
}

/**
 * The first 12 bytes give the image resolution (2 int)
 * and the number of entries per pixel (1 int).
 *
 * The intersection data is composed of
 *  position      (4 doubles)
 *  light vector  (4 doubles)
 *  object ID     (1 double)
 *  freqshift     (1 double)
 *
 *  jacobi parameters (5 doubles)
 *
 * @param filename   Filename for data.
 * @param filter     Camera filter.
 */
void GvsChannelImg2D::writeIntersecData( const char* filename , GvsCamFilter filter ) {
    std::string baseFilename = std::string(filename);
    size_t pos = baseFilename.find_last_of(".");
    baseFilename = baseFilename.substr(0,pos);

    FILE* fptr = NULL;

    // position and direction;
    int nc = 0;
    switch(filter) {
        case gvsCamFilterRGBpdz: {
            nc = NUM_PDZ_CHANNELS;
#ifdef _WIN32
            fopen_s(&fptr, (baseFilename + ".pdz").c_str(), "wb");
#else
            fptr = fopen((baseFilename+".pdz").c_str(),"wb");
#endif
            break;
        }
        case gvsCamFilterRGBjac: {
            nc = NUM_JAC_CHANNELS;
#ifdef _WIN32
            fopen_s(&fptr,(baseFilename + ".jac").c_str(), "wb");
#else
            fptr = fopen((baseFilename+".jac").c_str(),"wb");
#endif
            break;
        }
        case gvsCamFilterRGBpt: {
            nc = NUM_PT_CHANNELS;
#ifdef _WIN32
            fopen_s(&fptr, (baseFilename + ".pt").c_str(), "wb");
#else
            fptr = fopen((baseFilename+".pt").c_str(),"wb");
#endif
            break;
        }

        default:
        case gvsCamFilterRGB: {
            break;
        }
    }

    fprintf(stderr,"Write intersec data: %s (%d x %d, %d, %s)\n",baseFilename.c_str(),imgWidth,imgHeight,nc,GvsCamFilterNames[(int)filter].c_str());

    if (fptr!=NULL && imgIntersecData!=NULL) {
        fwrite((void*)&imgWidth,sizeof(int),1,fptr);
        fwrite((void*)&imgHeight,sizeof(int),1,fptr);
        fwrite((void*)&nc,sizeof(int),1,fptr);
        int pos;        
        for(int y=imgHeight-1; y>=0; y--) {
            for(int x=0; x<imgWidth; x++) {
                pos = y*imgWidth+x;
                fwrite((void*)imgIntersecData[pos].pos,sizeof(double),4,fptr);

                if (filter==gvsCamFilterRGBpdz || filter==gvsCamFilterRGBjac) {
                    fwrite((void*)imgIntersecData[pos].dir,sizeof(double),4,fptr);
                    fwrite((void*)&imgIntersecData[pos].objID,sizeof(double),1,fptr);
                    fwrite((void*)&imgIntersecData[pos].freqshift,sizeof(double),1,fptr);
                    if (filter==gvsCamFilterRGBjac) {
                        fwrite((void*)imgIntersecData[pos].jacobi,sizeof(double),5,fptr);
                    }
                } else if (filter==gvsCamFilterRGBpt) {
                    fwrite((void*)imgIntersecData[pos].uv, sizeof(double), 2, fptr);
                    fwrite((void*)&imgIntersecData[pos].objID,sizeof(double),1,fptr);
                }
            }
        }
        fclose(fptr);
    }
}


void GvsChannelImg2D::Print( FILE* fptr ) {
    fprintf(fptr,"ChannelImg2D {\n");
    fprintf(fptr,"\tFileName    : %s\n",imgFileName);
    fprintf(fptr,"\twidth       : %d\n",imgWidth);
    fprintf(fptr,"\theight      : %d\n",imgHeight);
    fprintf(fptr,"\tnumChannels : %d\n",imgNumChannels);
    fprintf(fptr,"\tdataOrder   : %d\n",imgDataOrder);
    fprintf(fptr,"\twrapPolicyX : %s\n",GvsWrapPolicyName[imgWrapPolicyX].c_str());
    fprintf(fptr,"\twrapPolicyY : %s\n",GvsWrapPolicyName[imgWrapPolicyY].c_str());
    fprintf(fptr,"\twrapColor   : ");imgWrapConstColor.Print(fptr);
    fprintf(fptr,"\twrapValue   : %f\n",imgWrapConstValue);
    fprintf(fptr,"}\n");
}


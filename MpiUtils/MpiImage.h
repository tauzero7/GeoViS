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
#ifndef MPI_IMAGE
#define MPI_IMAGE

#include <iostream>

#include <GvsGlobalDefs.h>
#include <MpiUtils/GvsMpiDefs.h>

class GvsChannelImg2D;

class GvsMpiImage
{
  public:
    GvsMpiImage();
    virtual ~GvsMpiImage ();

    void    setWithData( bool withData );

    void         setOutfilename( const std::string filename, const std::string fileExt, const int num = 0 );
    std::string  getOutfilename() const;

    void    setImageResolution ( int x, int y );
    void    setNumChannels     ( int num );
    int     getNumChannels     ( void ) const;

    void    insertRegion       ( int x1, int y1, int x2, int y2, uchar* p, gvsData* data );

    void    setNumTasks        ( int num );
    int     getNumTasksLeft    ( void ) const;

    void    writePicture       ( GvsCamFilter filter = gvsCamFilterRGB, double gamma = 1.0 );    
    bool    writeImageFileIfPossible ( GvsCamFilter filter = gvsCamFilterRGB, double gamma = 1.0 );

    void    Print ( FILE* fptr = stderr ) const;


    void    correctGamma          ( double gamma );
    void    activate              ( void );
    void    decreaseNumTasksLeft  ( void );
    void    stop                  ( void );

  protected:
    std::string   mOutfilename;

    int      mNumChannels;
    int      mImageWidth;
    int      mImageHeight;

    uchar*   mBuffer;
    gvsData* mData;
    bool     mWithData;

    int      mNumTasksLeft;
    bool     mActive;
};


#endif

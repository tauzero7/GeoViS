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
#ifndef MPI_TASK_MANAGER
#define MPI_TASK_MANAGER


#include "Dev/GvsDevice.h"
#include "Parser/GvsParser.h"
#include "MpiUtils/GvsMpiDefs.h"
#include "MpiUtils/MpiImage.h"


class GvsMpiTaskManager
{
  public:
    GvsMpiTaskManager(const std::string infile, const std::string outfile);
    virtual ~GvsMpiTaskManager();

    bool  setStartDevice  ( int startdev );
    int   getStartDevNr   ( ) const;

    void  setRenderDevice ( int renderdev );

    virtual bool   initialize ( int numNodes, int numNodesImage );

    virtual void   getDevice  ( GvsDevice *device, unsigned int k = 0 );

    void  createScene ( int task, GvsDevice *device );

    int   getNumTasks          ( ) const;
    void  activateTask         ( int task );
    int   getNextAvailableTask ( int task ) const;
    void  getViewPort          ( int task, int &x1, int &y1, int &x2, int &y2) const;
    int   getImageNr           ( int task ) const;

    void  insertRegion         ( int task, uchar* p, gvsData* data );

    bool  writeImageFileIfPossible( int task, double gamma = 1.0 );
    void  Print ( FILE* fptr = stderr ) const;


  protected:

    GvsParser*   parser;
    std::string  inFileName;
    std::string  outFileName;

    GvsDevice    mDevice;

    int          mNumDevices;
    int          mNumNodesImage;
    int          mStartDevice;
    int          mRenderDevice;

    int          mNumTasks;
    GvsMpiTask*  mTasks;


    GvsMpiImage* mImage;
    int          mImageHeight;
    int          mImageWidth;

    uchar*       mRenderImage;
};


#endif

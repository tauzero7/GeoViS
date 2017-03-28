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

#include <vector>
#include <fstream>

#include "MpiTaskManager.h"

#include "Img/GvsPicIOEnvelope.h"
#include "Img/GvsChannelImg2D.h"

#include "m4dGlobalDefs.h"

extern std::vector<GvsDevice*>  gpDevice;


GvsMpiTaskManager :: GvsMpiTaskManager( const std::string infile, const std::string outfile ) {
    inFileName  = infile;
    outFileName = outfile;

    parser = new GvsParser();

    mRenderDevice = -1;  // alle Devices Rendern
    mStartDevice = 0;
}


GvsMpiTaskManager::~GvsMpiTaskManager ( ) {
    delete parser;
}


bool GvsMpiTaskManager :: setStartDevice ( int startdev ) {
    mStartDevice = startdev;
    if (mStartDevice<0)
        mStartDevice = 0;
    return true;
}


int GvsMpiTaskManager :: getStartDevNr() const {
    return mStartDevice;
}


void GvsMpiTaskManager :: setRenderDevice ( int renderdev ) {
    mRenderDevice = renderdev;
    if (mRenderDevice!=-1) {
        mStartDevice  = renderdev;
    }
}


bool GvsMpiTaskManager :: initialize ( int , int numNodesImage ) {
    //fprintf(stderr,"Parse scm file...\n");
    parser->read_scene(inFileName.c_str());
    mNumDevices = parser->getNumDevices();

    parser->getDevice(&mDevice,0);
    m4d::ivec2 res(mDevice.camera->GetResolution());
    bool isStereo = mDevice.camera->isStereoCam();

    if (mRenderDevice!=-1) {
        if (mRenderDevice >= mNumDevices/(isStereo?2:1)) {
            fprintf(stderr,"Device %d does not exist!\n",mRenderDevice);
            return false;
        }
        fprintf(stderr,"Render only device %d of %d\n",mRenderDevice,mNumDevices);        
        if (isStereo) {
            mNumDevices = mRenderDevice*2 + 2;
        } else {
            mNumDevices = mRenderDevice + 1;
        }
    }

    mImage = new GvsMpiImage[mNumDevices - mStartDevice*(isStereo?2:1)];
    mImageWidth  = res.x(0);
    mImageHeight = res.x(1);

    mNumNodesImage = numNodesImage;

    // image size smaller than the number of nodes per image
    if (mImageHeight < mNumNodesImage )
        mNumNodesImage = mImageHeight;

    // step size
    int yStepsize = mImageHeight / mNumNodesImage;
    if (yStepsize < 1)
        yStepsize = 1;

    mNumNodesImage = mImageHeight / yStepsize;
    if (mNumNodesImage*yStepsize < mImageHeight)
        mNumNodesImage++;


    // Anzahl der Tasks ergibt sich aus der Anzahl Teilaufgaben je Bild mal der Anzahl Bilder,
    // wobei die Anzahl der Bilder gleich der Anzahl der Devices ist
    mNumTasks = mNumNodesImage * (mNumDevices - mStartDevice*(isStereo?2:1));
    mTasks = new GvsMpiTask[mNumTasks];
    assert( mTasks != NULL );

    GvsCamFilter  camFilter;

    // Subdivide images into tasks
    int actTask = 0;
    for ( int image = 0; image < (mNumDevices - mStartDevice*(isStereo?2:1)); image++)   {
        mImage[image].setNumTasks(mNumNodesImage);
        mImage[image].setImageResolution(mImageWidth,mImageHeight);

        if (isStereo) {
            if (image % 2 == 0) {
                mImage[image].setOutfilename(outFileName,GvsCamEyeFileExt[gvsCamEyeLeft],image/(isStereo?2:1) + mStartDevice);
            } else {
                mImage[image].setOutfilename(outFileName,GvsCamEyeFileExt[gvsCamEyeRight],image/(isStereo?2:1) + mStartDevice);
            }
        } else {
            mImage[image].setOutfilename(outFileName,GvsCamEyeFileExt[gvsCamEyeStandard],image + mStartDevice);
        }

        camFilter = mDevice.camera->getCamFilter();

        //int numChannels;
        switch (camFilter) {
            default:
            case gvsCamFilterRGB: {
                //numChannels = 3;
                break;
            }
            case gvsCamFilterRGBpdz:
            case gvsCamFilterRGBjac:
            case gvsCamFilterRGBpt:
            case gvsCamFilterRGBIntersec: {
                //numChannels = 3;
                mImage[image].setWithData(true);
                break;
            }
        }

        int startY = 0;
        for ( int imageRegion = 0; imageRegion < mNumNodesImage; imageRegion++ ) {
            mTasks[actTask].x1 = 0;
            mTasks[actTask].y1 = startY;
            mTasks[actTask].x2 = mImageWidth-1;
            startY += yStepsize;
            mTasks[actTask].y2 = startY-1;

            mTasks[actTask].status = TASK_WAITING;

            // assign a device to each task
            parser->getDevice(&(mTasks[actTask].device),image + mStartDevice*(isStereo?2:1));

            // Every task stores in specific image.
            mTasks[actTask].imageNr = image;

            actTask++;
        }
        mTasks[actTask-1].y2 = mImageHeight-1;
    }
    return true;
}


void GvsMpiTaskManager :: getDevice( GvsDevice *device, unsigned int k ) {
    assert ( k < gpDevice.size() );

    device->metric = gpDevice[k]->metric;

    device->camera    = gpDevice[k]->camera;
    device->projector = gpDevice[k]->projector;

    device->lightSrcMgr = gpDevice[k]->lightSrcMgr;
    device->sceneGraph  = gpDevice[k]->sceneGraph;

    if (!device->mChangeObj.empty()) {
        device->mChangeObj.clear();
    }
    device->mChangeObj = gpDevice[k]->mChangeObj;    
    device->camEye = gpDevice[k]->camEye;
}


int GvsMpiTaskManager::getNumTasks ( ) const {
    return mNumTasks;
}


void GvsMpiTaskManager::activateTask ( int task ) {
    mTasks[task].status = TASK_RUNNING;
}


void GvsMpiTaskManager::getViewPort ( int task, int &x1, int &y1, int &x2, int &y2) const {
    x1 = mTasks[task].x1;
    y1 = mTasks[task].y1;
    x2 = mTasks[task].x2;
    y2 = mTasks[task].y2;
}


int GvsMpiTaskManager::getImageNr ( int task ) const {
    return mTasks[task].imageNr;
}


void GvsMpiTaskManager :: insertRegion ( int task, uchar* p, gvsData* data ) {
    //  cerr << "GvsMpiTaskManager :: insertRegion: " << task << endl;
    int x1 = mTasks[task].x1;
    int y1 = mTasks[task].y1;
    int x2 = mTasks[task].x2;
    int y2 = mTasks[task].y2;

    mImage[mTasks[task].imageNr].insertRegion(x1,y1,x2,y2,p,data);
}


bool GvsMpiTaskManager :: writeImageFileIfPossible ( int task, double gamma ) {
    GvsCamFilter filter = mDevice.camera->getCamFilter();
    return mImage[mTasks[task].imageNr].writeImageFileIfPossible( filter, gamma );
}

/**
 * @brief GvsMpiTaskManager::getNextAvailableTask
 * @param task
 * @return
 */
int GvsMpiTaskManager :: getNextAvailableTask ( int task ) const {
    while ((mTasks[task].status != TASK_WAITING) && (task < mNumTasks-1))   {
        task++;
    }

    if (task == mNumTasks-1)   {
        if (mTasks[task].status != TASK_WAITING) {
            task++;
        }
    }
    else if (task >= mNumTasks) {
        task = mNumTasks;
    }

    return task;
}

/**
 * @brief GvsMpiTaskManager::createScene
 * @param task
 * @param device
 */
void GvsMpiTaskManager :: createScene( int task, GvsDevice *device ) {
    device->metric   = (mTasks[task].device).metric;

    device->camera      = (mTasks[task].device).camera;
    device->projector   = (mTasks[task].device).projector;

    device->lightSrcMgr = (mTasks[task].device).lightSrcMgr;
    device->sceneGraph  = (mTasks[task].device).sceneGraph;

    // clear previous list because of copy operation
    if (!device->mChangeObj.empty()) {
        device->mChangeObj.clear();
    }
    device->mChangeObj  = (mTasks[task].device).mChangeObj;

    device->camEye = (mTasks[task].device).camEye;

    // make changes
    device->makeChange();
}


void GvsMpiTaskManager :: Print ( FILE* fptr ) const {
    fprintf(fptr,"\nMpiTaskManager:   \n---------------\n");
    fprintf(fptr,"\tno devices:      %d\n",mNumDevices);
    fprintf(fptr,"\tno nodes/image:  %d\n",mNumNodesImage);
    fprintf(fptr,"\tno tasks:        %d\n",mNumTasks);
    fprintf(fptr,"\tscene file:      %s\n",inFileName.c_str());
    fprintf(fptr,"\timage file:      %s\n",outFileName.c_str());
    fprintf(fptr,"\n");
}

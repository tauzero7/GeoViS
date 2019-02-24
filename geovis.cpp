// ---------------------------------------------------------------------
//  Copyright (c) 2013, Universitaet Stuttgart, VISUS, Thomas Mueller
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

#include "Dev/GvsDevice.h"
#include "Dev/GvsSampleMgr.h"
#include "Img/GvsPicIOEnvelope.h"
#include "Parser/GvsParser.h"
#include "Utils/GvsLog.h"

#ifndef _WIN32
static GvsLog& LOG = GvsLog::instance();
#else
m4d::MetricDatabase* m4d::MetricDatabase::m_instance = nullptr;
#endif

void renderDevice( GvsDevice* dev, char* outFileName ) {   
    GvsSampleMgr* sampleMgr = new GvsSampleMgr(dev,true);
    sampleMgr->setRegionToImage();

    fprintf(stderr,"\nStart rendering...\n");
    sampleMgr->putFirstPixel();
    while (sampleMgr->putNextPixel());

    fprintf(stderr,"\nRendering done... write image...\n");
    sampleMgr->writePicture(outFileName);

    delete sampleMgr;
}

/**
 * @brief Main program for serial ray tracing.
 * @param argc
 * @param argv
 * @return
 * 
 * @file   geovis.cpp
 */
int main(int argc, char* argv[]) {
    if (argc<3) {
        fprintf(stderr,"Usage: ./gvsRender <SDL-file> <img-filename> [deviceNo]\n");
        return -1;
    }

    char* inFileName  = argv[1];
    char* outFileName = argv[2];

    //LOG.setLogFile("log.txt",0);

    // test file ending before rendering
    GvsPicIOEnvelope picIO;
    if (!picIO.testImageExtension(outFileName)) {
        fprintf(stderr,"file ending unknown!\n");
        return -1;
    }

    int   devNum = 0;
    if (argc>3) devNum = atoi(argv[3]);

    // ---- parse SDL file
    GvsParser* parser = new GvsParser();
    parser->read_scene(inFileName);
   // parser->printAll();

    // ---- get device
    GvsDevice device;
    parser->getDevice(&device,0);
    
    if (device.camera->isStereoCam() && parser->getNumDevices() > 1) {
        parser->getDevice(&device, static_cast<unsigned int>(2*devNum+0));
        device.makeChange();
        //device.Print();
        renderDevice(&device,outFileName);
        
        parser->getDevice(&device, static_cast<unsigned int>(2*devNum+1));
        device.makeChange();
        //device.Print();
        renderDevice(&device,outFileName);
    }
    else {
        parser->getDevice(&device, static_cast<unsigned int>(devNum));
        device.makeChange();
        renderDevice(&device,outFileName);
    }
    //device.Print();


    // ---- initialize sample manager
    /*
    GvsSampleMgr* sampleMgr = new GvsSampleMgr(&device,true);
    sampleMgr->setRegionToImage();

    fprintf(stderr,"\nStart rendering...\n");
    sampleMgr->putFirstPixel();
    while (sampleMgr->putNextPixel());

    fprintf(stderr,"\nRendering done... write image...\n");
    sampleMgr->writePicture(outFileName);
*/

    delete parser;
    return 0;
}


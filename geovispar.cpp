#include <mpi.h>

#include <iostream>
#include <unistd.h>

#include "Dev/GvsDevice.h"
#include "Dev/GvsSampleMgr.h"
#include "Img/GvsPicIOEnvelope.h"
#include "Parser/GvsParser.h"
#include "Utils/GvsLog.h"

#include "MpiUtils/GvsMpiDefs.h"
#include "MpiUtils/MpiTaskManager.h"


char* inFileName   = nullptr;
char* outFileName  = nullptr;
char* maskFileName = nullptr;
char* logFileName  = nullptr;
int   numNodesImage = 1;
int   renderDevice  = -1;
int   startDevice   = 0;

GvsDevice     device;
GvsSampleMgr  sampleMgr ( &device );

GvsLog& LOG = GvsLog::instance();


void MpiCreateMsgPtType( MPIMsgPt *c, MPI_Datatype *newType ) {
    MPI_Datatype  type[5]     = {MPI_INT, MPI_INT, MPI_INT, MPI_LONG, MPI_LONG};
    int           blocklen[5] = { 1, 1, 1, 1, 1 };
    MPI_Aint      disp[5];
    long          base,i;

    MPI_Get_address ( c, disp );
    MPI_Get_address ( &(c->node),      disp + 1 );
    MPI_Get_address ( &(c->task),      disp + 2 );
    MPI_Get_address ( &(c->bytes),     disp + 3 );
    MPI_Get_address ( &(c->numPixels), disp + 4 );
    base = disp[0];

    for (i = 0; i < 5; i++) {
        disp[i] -= base;
    }
    MPI_Type_create_struct ( 5, blocklen, disp, type, newType );
    MPI_Type_commit ( newType );
}

/**
 * @brief readCmdLineParams
 * @param argc
 * @param argv
 * @return
 */
int readCmdLineParams( int argc, char** argv ) {
    if ( argc < 3 ) {
        fprintf(stderr,"Usage: ./gvsRenderPar [options] <infilename> <outfilename>\n");
        fprintf(stderr,"\t[-tasks <n>]       tasks per image\n");
        fprintf(stderr,"\t[-renderdev <n>]   render only device <n>\n");
        fprintf(stderr,"\t[-startdev <n>]    start device <n>\n");
        fprintf(stderr,"\t[-mask <filename>] mask image\n");
        fprintf(stderr,"\t[-log <filename>]  log filename\n");
        fprintf(stderr,"\tinfilename         scene description file\n");
        fprintf(stderr,"\toutfilename        output image base file name\n");
        fprintf(stderr,"\n");
        return 0;
    }

    inFileName  = argv[argc-2];
    outFileName = argv[argc-1];

    int i = 0;
    while ( ++i < argc-2 ) {
        if (!strcmp( argv[i], "-tasks")) {
            if (sscanf( argv[++i], "%d", &numNodesImage) != 1) {
                std::cerr << "Error: Integer expected for <n> in '-tasks <n>'\n";
                return 0;
            }
        }
        else if (!strcmp( argv[i], "-renderdev")) {
            if (sscanf( argv[++i], "%d", &renderDevice) != 1) {
                std::cerr << "Error: Integer expected for <n> in '-renderdev <n>'\n";
                return 0;
            }
        }
        else if (!strcmp( argv[i], "-startdev")) {
            if (sscanf( argv[++i], "%d", &startDevice) != 1) {
                std::cerr << "Error: Integer expected for <n> in '-startdev <n>'\n";
                return 0;
            }
        }
        else if (!strcmp( argv[i], "-mask")) {
            maskFileName = argv[++i];
        }
        else if (!strcmp( argv[i], "-log")) {
            logFileName = argv[++i];
        }
    }
    return 1;
}


void RayTraceRegion ( int x1, int y1, int x2, int y2, int image, uchar region[] ) {
    if (device.camEye == gvsCamEyeLeft) {
        fprintf(stderr,"Raytracing-Region: (%4i,%4i) - (%4i,%4i) of image %4i  (left)\n",x1+1,y1+1,x2+1,y2+1,image);
    } else if (device.camEye == gvsCamEyeRight) {
        fprintf(stderr,"Raytracing-Region: (%4i,%4i) - (%4i,%4i) of image %4i  (right)\n",x1+1,y1+1,x2+1,y2+1,image);
    } else {
        fprintf(stderr,"Raytracing-Region: (%4i,%4i) - (%4i,%4i) of image %4i\n",x1+1,y1+1,x2+1,y2+1,image);
    }

    sampleMgr.setRegion(m4d::ivec2(x1,y1),m4d::ivec2(x2,y2));
    sampleMgr.putFirstPixel();
    while (sampleMgr.putNextPixel());

    sampleMgr.extractRegion(x1,y1,x2,y2,region);
}

/**
 * @brief RayTraceRegionData
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param image
 * @param region
 * @param regData
 */
void RayTraceRegionData ( int x1, int y1, int x2, int y2, int image, uchar region[], gvsData* regData ) {
    fprintf(stderr,"Raytracing-RegionData: (%4i,%4i) - (%4i,%4i) of image %4i\n",x1+1,y1+1,x2+1,y2+1,image);

    sampleMgr.setRegion(m4d::ivec2(x1,y1),m4d::ivec2(x2,y2));        
    sampleMgr.putFirstPixel();    
    while (sampleMgr.putNextPixel());

    sampleMgr.extractRegionData(x1,y1,x2,y2,region,regData);
}

/**
 * @brief printMPIStatus
 * @param status
 */
void printMPIStatus( MPI_Status &status ) {
    fprintf(stderr,"MPI_Status: Source=%d  Tag=%d  Error=%d\n",status.MPI_SOURCE,status.MPI_TAG,status.MPI_ERROR);
}


/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]) {    
    if (readCmdLineParams(argc,argv)<1) {
        return -1;
    }

    // test file ending before rendering
    GvsPicIOEnvelope picIO;
    if (!picIO.testImageExtension(outFileName)) {
        fprintf(stderr,"file ending unknown!\n");
        return -1;
    }

    int  myrank;
    int  ranksize;
    MPI_Status  status;

    // Initialize MPI
    MPI_Init(&argc,&argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );     // my place in MPI system
    MPI_Comm_size( MPI_COMM_WORLD, &ranksize );   // size of MPI system

    if (ranksize<2) {
        fprintf(stderr,"Needs at least two nodes!\n");
        MPI_Finalize();
        return -2;
    }

    int   nrNodes       = ranksize-1;   // master node does not render
    int   nrActiveNodes = 0;            // until now, no node is active

    GvsMpiTaskManager* taskManager;
    taskManager = new GvsMpiTaskManager(inFileName,outFileName);
    taskManager->setStartDevice(startDevice);
    taskManager->setRenderDevice(renderDevice);
    if (!taskManager->initialize(nrNodes,numNodesImage)) {
        MPI_Finalize();
        return -1;
    }

    taskManager->getDevice(&device);
    sampleMgr.setRegionToImage();    // Adapt ChannelImg to camera resolution        
    if (maskFileName!=NULL) {
        sampleMgr.setMask(maskFileName);
    }

    int actTask = 0;
    MPI_Barrier ( MPI_COMM_WORLD );

    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processor_name,&namelen);
    fprintf(stderr,"Process %d on %s\n",  myrank, processor_name);

    if (logFileName!=NULL) {
        char mylogFileName[1024];
        sprintf(mylogFileName,"log_%s_%d.txt",logFileName,myrank);
        LOG.setLogFile(mylogFileName,5);
    }

    MPI_Datatype   MPI_MsgPt;
    MPIMsgPt       msgPt;
    MpiCreateMsgPtType( &msgPt, &MPI_MsgPt );

    // --------------------------------------------------------------
    //                        M A S T E R
    // --------------------------------------------------------------
    if (myrank == 0) {
        taskManager->Print();

        for ( int i = 1; i < ranksize; i++ ) {
            actTask = taskManager->getNextAvailableTask(actTask);
            printf("Master: Sending task %d to MPI-Prozess %d \n",actTask,i);

            if (actTask < taskManager->getNumTasks() ) {
                taskManager->activateTask(actTask);

                MPI_Send( &actTask, 1, MPI_INT, i, TAG_START_TASK, MPI_COMM_WORLD);
                nrActiveNodes++;
            }
            else {
                int dummyTask = -1;
                MPI_Send ( &dummyTask, 1, MPI_INT, i, TAG_START_TASK, MPI_COMM_WORLD );
                MPI_Recv ( &msgPt, 1, MPI_MsgPt, i, TAG_RESULT, MPI_COMM_WORLD, &status );
            }
        }
    }

    // --------------------------------------------------------------
    //                        S L A V E
    // --------------------------------------------------------------
    else {
        do {
            MPI_Recv( &actTask, 1, MPI_INT, 0, TAG_START_TASK, MPI_COMM_WORLD, &status);
            if (actTask >= 0) {
                taskManager->createScene(actTask, &device);

                int x1,y1,x2,y2;
                taskManager->getViewPort(actTask,x1,y1,x2,y2);
                long numBytes  = sampleMgr.calcRegionBytes ( x1, y1, x2, y2 );
                long numPixels = sampleMgr.calcRegionPixels( x1, y1, x2, y2 );
                long numData   = sampleMgr.calcRegionData(x1, y1, x2, y2);

                uchar* regionBuffer = new uchar[numBytes];
                assert(regionBuffer!=NULL);

                gvsData* regionData = NULL;
                if (numData > 0) {
                    regionData = new gvsData[numData];
                    assert(regionData != NULL);
                }


                char hostname[1024];
                gethostname(hostname,1024);

                fprintf(stderr,"  Node %3i (%s): ",myrank,hostname);
                GvsCamFilter filter = device.camera->getCamFilter();

                int imgNr = taskManager->getImageNr(actTask) + taskManager->getStartDevNr();
                if (device.camEye == gvsCamEyeLeft || device.camEye == gvsCamEyeRight) {
                    imgNr = taskManager->getImageNr(actTask)/2 + taskManager->getStartDevNr();
                }

                if ((filter == gvsCamFilterRGBpdz) ||
                        (filter == gvsCamFilterRGBjac) ||
                        (filter == gvsCamFilterRGBpt) ||
                        (filter == gvsCamFilterRGBIntersec)) {

                    msgPt.numPixels = numPixels;
                    RayTraceRegionData(x1,y1,x2,y2, imgNr, regionBuffer, regionData);
                } else {
                    msgPt.numPixels = -1;
                    RayTraceRegion(x1,y1,x2,y2, imgNr, regionBuffer);
                }

                // send raytraced region back to MASTER
                msgPt.continueCalc = 1;
                msgPt.node  = myrank;
                msgPt.bytes = numBytes;
                msgPt.task  = actTask;

                MPI_Send ( &msgPt, 1, MPI_MsgPt, 0, TAG_RESULT, MPI_COMM_WORLD);
                MPI_Send ( regionBuffer, numBytes, MPI_UNSIGNED_CHAR, 0, TAG_REGION_BUFFER, MPI_COMM_WORLD );
                delete [] regionBuffer;

                if (msgPt.numPixels > 0) {
                    MPI_Send ( regionData, numPixels*sizeof(gvsData), MPI_BYTE, 0, TAG_DATA_BUFFER, MPI_COMM_WORLD );
                }
                if (regionData!=NULL) {
                    delete [] regionData;
                }
            }
            else {
                fprintf(stderr,"Node %3i: FINISHED\n",myrank);

                msgPt.continueCalc = 0;
                msgPt.node  = myrank;
                msgPt.bytes = -1;
                msgPt.task  = actTask;
                msgPt.numPixels = -1;

                MPI_Send ( &msgPt, 1, MPI_MsgPt, 0, TAG_RESULT, MPI_COMM_WORLD );
            }
        }
        while (actTask >= 0);
    }

    // --------------------------------------------------------------
    //                        M A S T E R
    // --------------------------------------------------------------
    if (myrank == 0) {
        while (nrActiveNodes > 0) {
            //bool written = false;
            MPI_Recv ( &msgPt, 1, MPI_MsgPt, MPI_ANY_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &status );
            nrActiveNodes--;

            if (msgPt.continueCalc) {
                int  currTask  = msgPt.task;
                int  fromNode  = msgPt.node;
                long numBytes  = msgPt.bytes;
                long numPixels = msgPt.numPixels;

                assert ( numBytes >= 0);
                uchar* regionBuffer = new uchar[numBytes];
                gvsData* regionData = NULL;

                MPI_Recv ( regionBuffer, numBytes, MPI_UNSIGNED_CHAR, fromNode, TAG_REGION_BUFFER, MPI_COMM_WORLD, &status );
                if (numPixels > 0) {
                    regionData = new gvsData[numPixels];
                    MPI_Recv ( regionData, numPixels*sizeof(gvsData), MPI_BYTE, fromNode, TAG_DATA_BUFFER, MPI_COMM_WORLD, &status);                    
                }

                taskManager->insertRegion( currTask, regionBuffer, regionData);

                delete [] regionBuffer;
                if (regionData!=NULL) {
                    delete [] regionData;
                }

                //written = taskManager->writeImageFileIfPossible( currTask );
                taskManager->writeImageFileIfPossible( currTask );

                actTask = taskManager->getNextAvailableTask(actTask);
                if (actTask < taskManager->getNumTasks())
                {
                    taskManager->activateTask(actTask);
                    MPI_Send ( &actTask, 1, MPI_INT, fromNode, TAG_START_TASK, MPI_COMM_WORLD );

                    nrActiveNodes++;
                }
                else
                {
                    int dummyTask = -1;
                    MPI_Send ( &dummyTask, 1, MPI_INT, fromNode, TAG_START_TASK, MPI_COMM_WORLD );
                    MPI_Recv ( &msgPt, 1, MPI_MsgPt, fromNode, TAG_RESULT, MPI_COMM_WORLD, &status );
                }
            }
        }
    }

    // Clean up MPI
    MPI_Type_free(&MPI_MsgPt);
    MPI_Finalize();
    return 0;
}


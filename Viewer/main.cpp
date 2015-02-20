// ---------------------------------------------------------------------
//  Copyright (c) 2014, Universitaet Stuttgart, VISUS, Thomas Mueller
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
#include <netdb.h>
#include <netinet/in.h>

extern "C" {
#include "GL3/gl3w.h"
}

#include "gtc/type_ptr.hpp"
#include "gtc/matrix_transform.hpp"
#include <GL/freeglut.h>

#include "AntTweakBar.h"
#include "GLShader.h"

#include "GvsGlobalDefs.h"
#include "Dev/GvsDevice.h"
#include "Img/GvsPicIOEnvelope.h"
#include "Img/GvsChannelImg2D.h"
#include "Parser/GvsParser.h"
#include "Utils/GvsLog.h"
#include "Utils.h"

GvsLog& LOG = GvsLog::instance();

int winWidth  = 700;
int winHeight = 700;

// vertices of window-filling quad
const static float quadVerts[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
};

enum eDataType {
    dt_none = -1,
    dt_image = 0,
    dt_pdz,
    dt_jac
};

// name of the standard vertex shader; you should not change it.
std::string vertShaderName = "Viewer/Shader/standard.vert";

// name of the standard fragment shader
std::string fragShaderName = "Viewer/Shader/standard.frag";


GLuint   va_square  = 0;
GLuint   texImageID = 0;
GLuint   texDataID[3] = {0,0,0};
GLShader shader;

GvsParser* parser = NULL;
GvsDevice device;

TwBar    *bar = NULL;
bool     showTBar = true;
int      showChannel = 0;

int    fileNum = 0;
TwEnumVal*               fileNameEnum;
std::vector<std::string> filePathNames;
std::vector<std::string> filenames;
GvsPicIOEnvelope picIO;
std::string              folderPath = "output";
std::vector<std::string> extensions;
std::string              inFileName = "";

int   shaderNum = 0;
TwEnumVal*               shaderNameEnum;
std::vector<std::string> shaderPathNames;
std::vector<std::string> shaderNames;
std::string              shaderPath = "Viewer/Shader";

int        buttonPressed = -1;
glm::ivec2 lastMousePos;
int resX = 0;
int resY = 0;
int numChannels = 0;
eDataType currDataType = dt_none;
int texFilter = 1;
GLenum gltfilter[2] = {GL_NEAREST,GL_LINEAR};
bool updateMinMax = true;

double *data = NULL;
double currDataValue = 0.0;
float *sdata = NULL;
float  currSDataValue = 0.0f;

float posMin[4], posMax[4];
float dirMin[4], dirMax[4];
float sMin[4], sMax[4];

float timeParam = 0.0f;
float timeStep  = 0.01f;

struct sockaddr_in  clientAddr;
int socketID = -1;
bool haveConnection = false;

// must be identical to entry in gdefs.h of the GeodesicViewer
enum  enum_socket_task {
    enum_stask_initdir = 0,
    enum_stask_initpos
};

typedef struct gvs_socket_data_t {
    int task;
    double vals[4];
} gvs_socket_data;


/** initialize socket connection to GeodesicViewer (if available).
 * @param serverAddress
 * @param port
 * @param id
 * @param addr
 */
bool  init_client_socket( std::string serverAddress, int port, int &id, struct sockaddr_in &addr ) {
    struct hostent*  hostInfo = gethostbyname( serverAddress.c_str() );
    if (hostInfo == NULL) {
        fprintf(stderr,"Can't interpret host: %s\n",serverAddress.c_str() );
        return false;
    }

    id = socket( AF_INET, SOCK_STREAM, 0 );
    if (id<0) {
        fprintf(stderr,"Can't create socket!\n");
        return false;
    }

    addr.sin_family = hostInfo->h_addrtype;
    memcpy( reinterpret_cast<char*>(&addr.sin_addr.s_addr), hostInfo->h_addr_list[0], hostInfo->h_length );
    addr.sin_port = htons( port );

    if (connect( id, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        //fprintf(stderr,"Can't connect to server %s\n",serverAddress.c_str());
        fprintf(stderr,"Can't connect to GeodesicViewer.\n");
        return false;
    }
    haveConnection = true;
    return true;
}

/** Initialize OpenGL...
 *   and generate vertex array for window-filling quad.
 */
void init_OpenGL() {
    if (gl3wInit()) {
        fprintf(stderr,"Error: Failed to initialize gl3w.\n");
        exit(1);
    }  
    glClearColor(0.0f,0.0f,0.0f,0.0f);

    GLuint  vbo_square;
    glGenVertexArrays(1,&va_square);
    glBindVertexArray(va_square);   
        glGenBuffers(1,&vbo_square);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_square);
        glEnableVertexAttribArray(0);
        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*4*2,quadVerts,GL_STATIC_DRAW);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER,0);      
    glBindVertexArray(0);    
}

/** Set minimum and maximum 
 * @param min
 * @param max
 * @param data
 */
void setMinMax( float *min, float *max, const float* data ) {
    for(int i=0; i<4; i++) {
        if (data[i]<min[i]) {
            min[i] = data[i];
        } else if (data[i]>max[i]) {
            max[i] = data[i];
        }
    }
}

/** Get actual pixel coordinates.
 * @param x
 * @param y
 * @param dx
 * @param dy
 */
void GetActualPixelCoords( int x, int y, int &dx, int &dy ) {
    dx = static_cast<int>( x/static_cast<double>(winWidth) * resX );
    dy = static_cast<int>( (winHeight-y)/static_cast<double>(winHeight) * resY );
}

/** Callback function for file setting.
 * @param value
 * @param clientData
 */
void TW_CALL SetFile( const void *value, void *clientData ) {
    fileNum = *static_cast<const int*>(value);

    if (texImageID>0) {
        glDeleteTextures(1,&texImageID);
        texImageID = 0;
    }
    if (data!=NULL) {
        delete [] data;
        data = NULL;
    }
    if (sdata!=NULL) {
        delete [] sdata;
        sdata = NULL;
    }
    if (texDataID[0]>0) {
        glDeleteTextures(3,texDataID);
        texDataID[0] = texDataID[1] = texDataID[2] = 0;
    }

    if (fileNum==0) {        
        currDataType = dt_none;
    } else {
        //std::cerr << filePathNames[fileNum-1] << std::endl;
        // -------------------------------------
        //    read tif,png, or ppm image
        // -------------------------------------
        if (FileEndsWith(filePathNames[fileNum-1],".tif") || 
            FileEndsWith(filePathNames[fileNum-1],".png") ||
            FileEndsWith(filePathNames[fileNum-1],".ppm")) {
            //std::cerr << "tif\n";
            GvsChannelImg2D image;
            if (!picIO.readChannelImg(image,filePathNames[fileNum-1].c_str())) {
                return;
            }

            resX = image.width();
            resY = image.height();
            numChannels = image.numChannels();
            fprintf(stderr,"Image size:  %d x %d (%d)\n",resX,resY,numChannels);

            int chSize = resX*resY;
            unsigned char *buf = new unsigned char[chSize*numChannels];
            unsigned char *ib = image.getImagePtr();            
            for(int i=0; i<chSize; i++) {
                for(int c=0; c<numChannels; c++) {
                    buf[numChannels*i+c] = ib[i+c*chSize];
                }
            }      

            glPixelStorei(GL_UNPACK_ALIGNMENT,1);

            glGenTextures(1,&texImageID);
            glBindTexture(GL_TEXTURE_2D,texImageID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            if (numChannels==3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
            } else if (numChannels==4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
            }
            glBindTexture(GL_TEXTURE_2D,0);
            delete [] buf;
            currDataType = dt_image;
        }
        // -------------------------------------
        //    read pdz file
        //      compare also GvsChannelImg2D::writeIntersecData
        // -------------------------------------
        else if (FileEndsWith(filePathNames[fileNum-1],".pdz")) {
                  // || FileEndsWith(filePathNames[fileNum-1],".jac")) {
            FILE* fptr = fopen(filePathNames[fileNum-1].c_str(),"rb");
            if (fptr==NULL) {
                return;
            }

            bool isOkay = true;
            isOkay &= (fread((char*)&resX,sizeof(int),1,fptr)==1);
            isOkay &= (fread((char*)&resY,sizeof(int),1,fptr)==1);
            isOkay &= (fread((char*)&numChannels,sizeof(int),1,fptr)==1);
            if (isOkay) {
                data = new double[resX*resY*numChannels];
                isOkay &= (fread((char*)&data[0],sizeof(double),resX*resY*numChannels,fptr)==(size_t)(resX*resY*numChannels));
            }
            fclose(fptr);
            
            if (!isOkay) {
                fprintf(stderr,"Error reading file %s.\n",filePathNames[fileNum-1].c_str());
                return;
            }

            if (numChannels==NUM_PDZ_CHANNELS) {                
                currDataType = dt_pdz;
            } else {
                currDataType = dt_jac;
            }

            // update min/max only if wanted
            if (updateMinMax) {
                for(int d=0; d<4; d++) {
                    posMin[d] = FLT_MAX;
                    posMax[d] = -FLT_MAX;
                    dirMin[d] = FLT_MAX;
                    dirMax[d] = -FLT_MAX;
                    sMin[d] = FLT_MAX;
                    sMax[d] = -FLT_MAX;
                }
            }

            float* posData = new float[resX*resY*4];
            float* dirData = new float[resX*resY*4];
            sdata = new float[resX*resY*4];

            // see also GvsChannelImg2D::writeIntersecData
            for(int i=0; i<resX*resY; i++) {
                posData[4*i+0] = static_cast<float>(data[numChannels*i+0]);
                posData[4*i+1] = static_cast<float>(data[numChannels*i+1]);
                posData[4*i+2] = static_cast<float>(data[numChannels*i+2]);
                posData[4*i+3] = static_cast<float>(data[numChannels*i+3]);

                dirData[4*i+0] = static_cast<float>(data[numChannels*i+4]);
                dirData[4*i+1] = static_cast<float>(data[numChannels*i+5]);
                dirData[4*i+2] = static_cast<float>(data[numChannels*i+6]);
                dirData[4*i+3] = static_cast<float>(data[numChannels*i+7]);

                if (numChannels==NUM_PDZ_CHANNELS) {
                    sdata[4*i+0] = static_cast<float>(data[numChannels*i+8]);   // objID
                    sdata[4*i+1] = static_cast<float>(data[numChannels*i+9]);   // freq-shift
                    sdata[4*i+2] = 0.0f; // not in use
                    sdata[4*i+3] = 0.0f; // not in use
                } else {

                }
            
                if (updateMinMax) {
                    setMinMax(posMin,posMax,&posData[4*i]);
                    setMinMax(dirMin,dirMax,&dirData[4*i]);
                    setMinMax(sMin,sMax,&sdata[4*i]);
                }
            }

            glGenTextures(3,texDataID);

            glBindTexture(GL_TEXTURE_2D,texDataID[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resX, resY, 0, GL_RGBA, GL_FLOAT, posData);

            glBindTexture(GL_TEXTURE_2D,texDataID[1]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resX, resY, 0, GL_RGBA, GL_FLOAT, dirData);

            glBindTexture(GL_TEXTURE_2D,texDataID[2]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resX, resY, 0, GL_RGBA, GL_FLOAT, sdata);

            glBindTexture(GL_TEXTURE_2D, 0);
            delete [] posData;
            delete [] dirData;
        }

        int devNum = HaveNumInFilename(filePathNames[fileNum-1]);
        if (devNum>=0 && parser!=NULL) {
            parser->getDevice(&device,devNum);
            device.makeChange();
 
            if (haveConnection) {
                m4d::vec4 pos = device.projector->getPosition();
                gvs_socket_data sdata = { (int)enum_stask_initpos, {pos.x(0), pos.x(1), pos.x(2), pos.x(3)} };
                send( socketID, (char*)&sdata, sizeof(gvs_socket_data), 0 );
            }
        }
    }
}

void TW_CALL GetFile( void *value, void *clientData ) {
    *static_cast<int*>(value) = fileNum;
}

void TW_CALL SetShader( const void *value, void *clientData ) {
    shaderNum = *static_cast<const int*>(value);
    //std::cerr << shaderNum << " "  << shaderPathNames[shaderNum] << std::endl;
    
    fragShaderName = shaderPathNames[shaderNum];
    shader.RemoveAllShaders();
    shader.CreateProgramFromFile(vertShaderName.c_str(),fragShaderName.c_str());
    glutPostRedisplay();
}

void TW_CALL GetShader( void *value, void *clientData ) {
    *static_cast<int*>(value) = shaderNum;
}

/** Callback function for resize button.
 * @param clientData
 */
void TW_CALL WinResize( void *clientData ) {
    if (resX>=50 && resY>=50) {
        glutReshapeWindow(resX,resY);
    }
}

/** Initialize TweakBar
 */
void init_TBar() {    
    if ( !TwInit(TW_OPENGL,NULL) ) {
        fprintf(stderr,"AntTweakBar initialization failed: %s\n", TwGetLastError());
        exit(1);
    }
    bar = TwNewBar("Parameters");
    TwDefine(" GLOBAL help='Tweak example.'"); // Message added to the help bar.
    TwDefine(" Parameters size='200 300' color='96 96 9' alpha=150 "); // change default tweak bar size and color  
    TwWindowSize(winWidth,winHeight);
    TwDefine(" GLOBAL fontsize=2  buttonalign=left");

    // construct file selector
    GetFileNames(folderPath,extensions,filePathNames);
    
    fileNameEnum = new TwEnumVal[filePathNames.size()+1];
    fileNameEnum[0].Value = 0;
    fileNameEnum[0].Label = "none";
    for(unsigned int i=0; i<filePathNames.size(); i++) {
        size_t fileLen = filePathNames[i].length();
        size_t posSep  = filePathNames[i].find_last_of("/");
        std::string name = filePathNames[i].substr(posSep+1,fileLen-posSep);
        filenames.push_back(name);
        fileNameEnum[i+1].Value = i+1;
        fileNameEnum[i+1].Label = filenames[filenames.size()-1].c_str();
    }


    std::vector<std::string> fragExt;
    fragExt.push_back(".frag");    
    GetFileNames(shaderPath,fragExt,shaderPathNames);

    shaderNameEnum = new TwEnumVal[shaderPathNames.size()];
    for(unsigned int i=0; i<shaderPathNames.size(); i++) {
        size_t fileLen = shaderPathNames[i].length();
        size_t posSep  = shaderPathNames[i].find_last_of("/");
        std::string name = shaderPathNames[i].substr(posSep+1,fileLen-posSep);
        //std::cerr << name << std::endl;
        shaderNames.push_back(name);
        shaderNameEnum[i].Value = i;
        shaderNameEnum[i].Label = shaderNames[shaderNames.size()-1].c_str();
        if (shaderNames[shaderNames.size()-1].compare("standard.frag")==0) {
            shaderNum = i;
        }
    }

    TwAddVarRW(bar, "WinWidth", TW_TYPE_INT32, &winWidth, " group='Window' readonly=true");
    TwAddVarRW(bar, "WinHeight", TW_TYPE_INT32, &winHeight, " group='Window' readonly=true");
    TwAddButton(bar, "Resize", WinResize, NULL, " group='Window'");

    TwType fileSelectorType = TwDefineEnum("FileSelectorType", fileNameEnum, filenames.size()+1);
    TwAddVarCB(bar, "Files", fileSelectorType, SetFile, GetFile, NULL, " group='Load'");
   
    TwAddVarRW(bar, "resX", TW_TYPE_INT32, &resX, " group='Load' readonly=true");
    TwAddVarRW(bar, "resY", TW_TYPE_INT32, &resY, " group='Load' readonly=true");
    TwAddVarRW(bar, "update", TW_TYPE_BOOLCPP, &updateMinMax, " group='Load'");

    TwType shaderSelectorType = TwDefineEnum("ShaderSelectorType", shaderNameEnum, shaderNames.size());
    TwAddVarCB(bar, "Shader", shaderSelectorType, SetShader, GetShader, NULL, " group='Load'");

    TwEnumVal chanVals[] = { {0,"none"}, {1,"1:x0"}, {2,"2:x1"}, {3,"3:x2"}, {4,"4:x3"},
                                         {5,"5:d0"}, {6,"6:d2"}, {7,"7:d3"}, {8,"8:d4"},
                                         {9,"9:objID"}, {10,"10:freqShift"} };
                                         //,{11, "11:val 1"}, {12, "12:val 2"} };
    TwType chanType = TwDefineEnum("ChanType", chanVals, 11);
    TwAddVarRW(bar, "Channel", chanType, &showChannel, " group='Value'");

    TwEnumVal filterVals[] = { {0,"nearest"}, {1,"linear"} };
    TwType filterType = TwDefineEnum("FilterType", filterVals, 2);
    TwAddVarRW(bar, "Filter", filterType, &texFilter, " group='Value'");   

    TwAddVarRW(bar, "DValue", TW_TYPE_DOUBLE, &currDataValue, " group='Value' readonly=true");
    TwAddVarRW(bar, "SValue", TW_TYPE_FLOAT,  &currSDataValue, " group='Value' readonly=true");

    TwAddVarRW(bar, "pos min x0", TW_TYPE_FLOAT, &posMin[0], " group='Position'");
    TwAddVarRW(bar, "pos max x0", TW_TYPE_FLOAT, &posMax[0], " group='Position'");    
    TwAddVarRW(bar, "pos min x1", TW_TYPE_FLOAT, &posMin[1], " group='Position'");
    TwAddVarRW(bar, "pos max x1", TW_TYPE_FLOAT, &posMax[1], " group='Position'");
    TwAddVarRW(bar, "pos min x2", TW_TYPE_FLOAT, &posMin[2], " group='Position'");
    TwAddVarRW(bar, "pos max x2", TW_TYPE_FLOAT, &posMax[2], " group='Position'");
    TwAddVarRW(bar, "pos min x3", TW_TYPE_FLOAT, &posMin[3], " group='Position'");
    TwAddVarRW(bar, "pos max x3", TW_TYPE_FLOAT, &posMax[3], " group='Position'");

    TwAddVarRW(bar, "dir min x0", TW_TYPE_FLOAT, &dirMin[0], " group='Direction'");
    TwAddVarRW(bar, "dir max x0", TW_TYPE_FLOAT, &dirMax[0], " group='Direction'");    
    TwAddVarRW(bar, "dir min x1", TW_TYPE_FLOAT, &dirMin[1], " group='Direction'");
    TwAddVarRW(bar, "dir max x1", TW_TYPE_FLOAT, &dirMax[1], " group='Direction'");
    TwAddVarRW(bar, "dir min x2", TW_TYPE_FLOAT, &dirMin[2], " group='Direction'");
    TwAddVarRW(bar, "dir max x2", TW_TYPE_FLOAT, &dirMax[2], " group='Direction'");
    TwAddVarRW(bar, "dir min x3", TW_TYPE_FLOAT, &dirMin[3], " group='Direction'");
    TwAddVarRW(bar, "dir max x3", TW_TYPE_FLOAT, &dirMax[3], " group='Direction'");

    TwAddVarRW(bar, "fs min", TW_TYPE_FLOAT, &sMin[1], " group='Supplement'");
    TwAddVarRW(bar, "fs max", TW_TYPE_FLOAT, &sMax[1], " group='Supplement'");
/*
    TwAddVarRW(bar, "val1 min", TW_TYPE_FLOAT, &sMin[2], " group='Supplement'");
    TwAddVarRW(bar, "val1 max", TW_TYPE_FLOAT, &sMax[2], " group='Supplement'");
    TwAddVarRW(bar, "val2 min", TW_TYPE_FLOAT, &sMin[3], " group='Supplement'");
    TwAddVarRW(bar, "val2 max", TW_TYPE_FLOAT, &sMax[3], " group='Supplement'");        
*/
    TwDefine("Parameters/Position group=MinMax");
    TwDefine("Parameters/Direction group=MinMax");
    TwDefine("Parameters/Supplement group=MinMax");
}

/** close callback function
 */
void close(){  
    TwTerminate();
    fprintf(stderr,"Bye bye...\n");
    glDeleteVertexArrays(1,&va_square);
    shader.Release();

    if (data!=NULL) {
        delete [] data;
    }   
    if (sdata!=NULL) {
        delete [] sdata;
    };
    exit( EXIT_SUCCESS );
}

/** display callback function
 */
void display() {
    glClear( GL_COLOR_BUFFER_BIT );   
    glm::mat4 pvMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f);
    
    shader.Bind();
    glUniformMatrix4fv(shader.GetUniformLocation("mvp"),1,GL_FALSE,glm::value_ptr(pvMatrix));
    glUniform1i(shader.GetUniformLocation("channel"),showChannel);
    glUniform1i(shader.GetUniformLocation("dataType"),static_cast<int>(currDataType));

    glUniform4fv(shader.GetUniformLocation("minPos"),1,posMin);
    glUniform4fv(shader.GetUniformLocation("maxPos"),1,posMax);
    glUniform4fv(shader.GetUniformLocation("minDir"),1,dirMin);
    glUniform4fv(shader.GetUniformLocation("maxDir"),1,dirMax);
    glUniform4fv(shader.GetUniformLocation("minS"),1,sMin);
    glUniform4fv(shader.GetUniformLocation("maxS"),1,sMax);

    glUniform1f(shader.GetUniformLocation("timeParam"),timeParam);

    // image texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texImageID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gltfilter[texFilter] );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gltfilter[texFilter] );        
    glUniform1i(shader.GetUniformLocation("texImageID"),0);

    // data texture with positions
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,texDataID[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gltfilter[texFilter] );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gltfilter[texFilter] );
    glUniform1i(shader.GetUniformLocation("texDataPos"),1);

    // data texture with directions
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,texDataID[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gltfilter[texFilter] );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gltfilter[texFilter] );
    glUniform1i(shader.GetUniformLocation("texDataDir"),2);

    // data texture with supplements
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,texDataID[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gltfilter[texFilter] );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gltfilter[texFilter] );
    glUniform1i(shader.GetUniformLocation("texDataS"),3);

    glBindVertexArray(va_square);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);    
    glBindTexture(GL_TEXTURE_2D,0);

    shader.Release();
    
    if (showTBar) {
        TwDraw();
    }
    glutSwapBuffers();
}

/**
 */
void idle(void) {
}

/** keyboard callback function
 * \param key : key stroke
 * \param x : 
 * \param y :
 */
void keyboard( unsigned char key, int x, int y ) {
    if (TwEventKeyboardGLUT(key,x,y)) {
        glutPostRedisplay();
        return;    
    }
    
    switch(key) {
        case 27: {   //!< Escape key
            glutLeaveMainLoop();
            break; 
        } 
        case 'r': {
            shader.RemoveAllShaders();
            shader.CreateProgramFromFile(vertShaderName.c_str(),fragShaderName.c_str());
            glutPostRedisplay();
            break;
        }
    }    
}

/** mouse motion callback function
 * @param x
 * @param y
 */
void motion(int x, int y) {
    if (showTBar && TwEventMouseMotionGLUT(x,y)) {
        glutPostRedisplay();
        return;
    }

    if (device.camera !=NULL) {
        int sx = static_cast<int>( x/static_cast<double>(winWidth)*resX );
        int sy = static_cast<int>( y/static_cast<double>(winHeight)*resY );
        double ksi,chi;
        device.camera->PixelToAngle(sx,sy,ksi,chi);
        //fprintf(stderr,"ksi: %f  chi: %f\n",ksi*RAD_TO_DEG,chi*RAD_TO_DEG);
        if (haveConnection) {
            gvs_socket_data sdata = { (int)enum_stask_initdir, {-1.0, 1.0, ksi*RAD_TO_DEG, chi*RAD_TO_DEG} };
            send( socketID, (char*)&sdata, sizeof(gvs_socket_data), 0 );
        }
    }    
    else if (buttonPressed == GLUT_RIGHT_BUTTON) {
        timeParam += timeStep * (y - lastMousePos.y);
    }
    lastMousePos = glm::ivec2(x,y);
    glutPostRedisplay();
}

/** mouse callback function
 * @param button
 * @param state
 * @param x
 * @param y
 */
void mouse(int button, int state, int x, int y) {
    if (showTBar && TwEventMouseButtonGLUT(button,state,x,y)) {
        glutPostRedisplay();
        return;
    }
    if (state == GLUT_DOWN) {
        buttonPressed = button;
    } else {
        buttonPressed = -1;
    }

    if (button == GLUT_LEFT_BUTTON && state==GLUT_DOWN && device.camera !=NULL) {
        int sx = static_cast<int>( x/static_cast<double>(winWidth)*resX );
        int sy = static_cast<int>( y/static_cast<double>(winHeight)*resY );
        double ksi,chi;
        device.camera->PixelToAngle(sx,sy,ksi,chi);
        //fprintf(stderr,"ksi: %f  chi: %f\n",ksi*RAD_TO_DEG,chi*RAD_TO_DEG);
        if (haveConnection) {
            gvs_socket_data sdata = { (int)enum_stask_initdir, {-1.0, 1.0, ksi*RAD_TO_DEG, chi*RAD_TO_DEG} };
            send( socketID, (char*)&sdata, sizeof(gvs_socket_data), 0 );
        }
    }
    lastMousePos = glm::ivec2(x,y);
}


/** Callback for passive mouse motion.
 * @param x
 * @param y
 */
void passiveMotion(int x, int y) {
    TwEventMouseMotionGLUT(x,y);
    if (data!=NULL && showChannel>0) {
        int dX,dY;
        GetActualPixelCoords(x,y,dX,dY);
        currDataValue = 0.0;
        currSDataValue = 0.0f;
        if (dX>=0 && dX<resX && dY>=0 && dY<resY) {
            if (showChannel<=numChannels) {
                currDataValue = data[numChannels*(dY*resX+dX) + showChannel-1];
            }
            if (showChannel>9) {
                currSDataValue = sdata[4*(dY*resX+dX) + showChannel-9];
            }
            //fprintf(stderr,"%4d %4d %f\n",dX,dY,currDataValue);               
           TwRefreshBar(bar);   
        }
    }
    glutPostRedisplay();
}

/** mouse wheel callback function
 * @param button
 * @param dir
 * @param x
 * @param y
 */
void mouseWheel(int button, int dir, int x, int y) {
    //std::cerr << button << " " << dir << std::endl;
    TwRefreshBar(bar);
    glutPostRedisplay();
}


/** reshape callback function
 * \param w : new window width
 * \param h : new window height
 */
void reshape( int w, int h ) {
    glViewport(0,0,w,h);
    TwWindowSize(w,h);
    winWidth = w;
    winHeight = h;
}

/** special key callback function
 * @param key
 * @param x
 * @param y
 */
void specialKey(int key, int x, int y) {
    if (TwEventSpecialGLUT(key,x,y)) {
        glutPostRedisplay();
        return;
    }
    if (key==GLUT_KEY_F12) {
        showTBar = !showTBar;
        glutPostRedisplay();
    }
}

/** read commandline parameters
 * @param argc
 * @param argv
 */
void readCmdLine( int argc, char* argv[] ) {
    if (argc<=1) {
        return;
    }

    bool extSet = false;
    for(int nArg=1; nArg<argc; nArg++) {
        if (strcmp(argv[nArg],"-h")==0) {
            fprintf(stderr,"==================================\n");
            fprintf(stderr,"\t -h    : this help\n");
            fprintf(stderr,"\t -f    : file folder\n");
            fprintf(stderr,"\t -e    : add file extension\n");
            fprintf(stderr,"\t -s    : scene file\n");
            fprintf(stderr,"==================================\n");
            exit(1);
        }
        else if (strcmp(argv[nArg],"-f")==0 && nArg+1<argc) {
            folderPath = argv[nArg+1];
        }
        else if (strcmp(argv[nArg],"-e")==0 && nArg+1<argc) {
            extensions.push_back(argv[nArg+1]);
            extSet = true;
        }
        else if (strcmp(argv[nArg],"-s")==0 && nArg+1<argc) {
            inFileName = argv[nArg+1];
        }
    }

    if (!extSet) {
        extensions.push_back(".tif");
        extensions.push_back(".png");
        extensions.push_back(".ppm");
        extensions.push_back(".pdz");
        //extensions.push_back(".jac");
    }

    fprintf(stderr,"Consider file extensions: ");
    for(unsigned int i=0; i<extensions.size(); i++) {
        fprintf(stderr,"%s ",extensions[i].c_str());
    }
    fprintf(stderr,"\n");
}


/** That is the main function of the program. 
 * \param argc : number of command line arguments
 * \param argv : pointer to command line arguments
 */
int main( int argc, char* argv[] ) { 
    readCmdLine(argc,argv);

    if (inFileName!=std::string()) {   
        parser = new GvsParser();
        parser->read_scene(inFileName.c_str());
        //parser->printAll();
        parser->getDevice(&device);
        device.makeChange();
        //device.Print();
    }

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    //glutInitContextVersion(3,3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitWindowSize(winWidth,winHeight);
    glutCreateWindow("GvsViewer");
    
    glutDisplayFunc ( display );
    glutReshapeFunc ( reshape );
    glutCloseFunc   ( close );
    glutKeyboardFunc( keyboard ); 
    glutSpecialFunc ( specialKey );
    glutMouseFunc   ( mouse );
    glutMotionFunc  ( motion );
    glutPassiveMotionFunc ( passiveMotion );
    //glutMouseWheelFunc ( mouseWheel );
    glutIdleFunc    ( idle );
    
    init_OpenGL();
    init_TBar();
    if (!shader.CreateProgramFromFile(vertShaderName.c_str(),fragShaderName.c_str())) {
        fprintf(stderr,"Cannot create shaders.\n");
        return -1;
    }

    std::string serverAddr = "127.0.0.1";
    init_client_socket( serverAddr, 9900, socketID, clientAddr );
    
    glutMainLoop();
    return 0;
}

#ifndef GVS_SHADER_H
#define GVS_SHADER_H

#include "GvsGlobalDefs.h"
#include "Img/GvsColor.h"

class GvsDevice;
class GvsRayVisual;
class GvsShader;


enum GvsShaderType
{
    surfaceShader = 1
};


class GvsShader
{
public:
    GvsShader ( );
    virtual ~GvsShader() {}

    virtual GvsColor getIncidentLight ( ) = 0;
    virtual GvsColor getIncidentLight ( GvsDevice* device, GvsRayVisual &ray ) = 0;

    virtual void Print ( FILE* fptr = stderr ) = 0;
};

#endif

#ifndef GVS_SURFACE_SHADER_H
#define GVS_SURFACE_SHADER_H

#include <iostream>

#include "Shader/GvsShader.h"

class GvsSurfIntersec;
class GvsDevice;
class GvsColor;
class GvsRay;
class GvsRayVisual;
class GvsTexture;

/**
 * @brief The GvsSurfaceShader class
 */
class GvsSurfaceShader : public GvsShader
{
public:
    GvsSurfaceShader();

    void setPrimitiveColor( GvsTexture* tex );
    void setNormalMap( GvsTexture* tex );
    void setKambient( double ka );
    void setKdiffuse( double kl );

    virtual GvsColor ambientColor( GvsDevice* device, GvsRayVisual &ray );

    virtual GvsColor getIncidentLight( void ) { return RgbBlack; }
    virtual GvsColor getIncidentLight( GvsDevice* device, GvsRayVisual &ray );

    virtual GvsColor reflArtifColor( GvsDevice* device, GvsRayVisual &ray );
    virtual GvsColor reflLightColor( GvsDevice* device, GvsRayVisual &ray );

    virtual void Print ( FILE* fptr = stderr );

private:
    GvsColor primitiveColor( GvsSurfIntersec& ) const;

protected:
    GvsSurfIntersec  *intersection;

    GvsTexture *matTexColor;
    GvsTexture *matNormalMap;
    double matAmbient;
    double matDiffuse;
};

#endif

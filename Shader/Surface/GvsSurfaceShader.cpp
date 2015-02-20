
#include "Shader/Surface/GvsSurfaceShader.h"
#include "Dev/GvsDevice.h"
#include "Img/GvsColor.h"
#include "Light/GvsLightSrc.h"
#include "Ray/GvsRay.h"
#include "Ray/GvsRayVisual.h"
#include "Texture/GvsTexture.h"


GvsSurfaceShader::GvsSurfaceShader() :
    intersection(NULL),
    matTexColor(NULL),
    matNormalMap(NULL),
    matAmbient(1.0),
    matDiffuse(0.0) {
}

void GvsSurfaceShader::setPrimitiveColor( GvsTexture* tex ) {
    matTexColor = tex;
}

void GvsSurfaceShader::setNormalMap( GvsTexture* tex ) {
    matNormalMap = tex;
}

void GvsSurfaceShader::setKambient( double ka ) {
    matAmbient = ka;
}

void GvsSurfaceShader::setKdiffuse( double kl ) {
    matDiffuse = kl;
}


GvsColor GvsSurfaceShader::ambientColor ( GvsDevice* device, GvsRayVisual & ) {
    return device->lightSrcMgr->ambientLight();
}


GvsColor GvsSurfaceShader :: getIncidentLight( GvsDevice* device, GvsRayVisual& ray ) {
    if ( (ray.surfIntersec().getLocalDirection() | ray.surfIntersec().normNormal()) > 0.0 ) {
        ray.surfIntersec().invertNormal();
    }

    //GvsColor ambColor  = ambientColor( device, ray );
    GvsColor primColor = primitiveColor( ray.surfIntersec() );

    return primColor*matAmbient  +
            primColor*matDiffuse*reflArtifColor( device, ray ) +
            primColor*reflLightColor( device, ray );
}


GvsColor  GvsSurfaceShader::reflArtifColor( GvsDevice* device, GvsRayVisual &ray ) {    
    // normal and direction are given in standard object system
    m4d::vec3 intersecNormal;
    if (matNormalMap!=NULL) {
        GvsColor nc = matNormalMap->sampleColor( ray.surfIntersec() );
        intersecNormal = -m4d::vec3( nc.red, nc.green, nc.blue ).getNormalized();
        //std::cerr << intersecNormal << std::endl;
    } else {
        intersecNormal = ray.surfIntersec().normNormal();
    }
    m4d::vec3 inDirection = -ray.surfIntersec().getLocalDirection();

    double cosNL = GVS_MAX( 0.0, intersecNormal | inDirection );
    //if (cosNL <= 0.0) return RgbBlack;

    GvsColor ambLight = device->lightSrcMgr->ambientLight();
    GvsColor outLight = cosNL * ambLight;
    return outLight;
}


GvsColor GvsSurfaceShader::reflLightColor( GvsDevice* device, GvsRayVisual &ray ) {
    GvsColor outLight = GvsColor(0.0);
    if (device->metric->getMetricName()!="Minkowski" ||
            !device->lightSrcMgr->withShadowRays()) {
        return outLight;
    }

    if (ray.surfIntersec().object()!=NULL) {
        if (ray.surfIntersec().object()->getObjType()!=inCoords ||
                ray.surfIntersec().object()->getMotion()!=NULL) {
            return outLight;
        }
    }

    // only valid for Minkowski spacetime in Cartesian coordinates !!!
    m4d::vec4 isecPoint = ray.surfIntersec().point();


    GvsLightSrc* light = NULL;
    bool lightFound = false;

    for(int i=0; i<(device->lightSrcMgr->length()); i++) {
        light = device->lightSrcMgr->getLightSrc(i,lightFound);

        if (lightFound) {
            m4d::vec4 lightPos = light->getPosition();
            m4d::vec3 lightDir = (lightPos.getAsV3D() - isecPoint.getAsV3D()).getNormalized();
            m4d::vec4 rayDir = m4d::vec4(-1.0,lightDir.x(0),lightDir.x(1),lightDir.x(2));

            // calculate a shadow ray
            GvsRayVisual* eyeRay = new GvsRayVisual(device->projector->getRayGen());
            eyeRay->recalc( isecPoint, rayDir );

            if (device->sceneGraph != NULL) {
                // find closest intersection of the shadow ray with a scene object
                if (!device->sceneGraph->testIntersection(*eyeRay)) {
                    outLight += light->color();
                }
            }

            delete eyeRay;
        }
    }
    return outLight;
}


GvsColor GvsSurfaceShader::primitiveColor( GvsSurfIntersec& intersec ) const {
    if (matTexColor != NULL) {
        return matTexColor->sampleColor( intersec );
    }
    return RgbBlack;
}


void GvsSurfaceShader::Print ( FILE* fptr ) {
    fprintf(fptr,"SurfaceShader {\n");
    fprintf(fptr,"\tobjcolor:  "); matTexColor->Print(fptr);
    if (matNormalMap!=NULL) {
        fprintf(fptr,"\tnormalmap: "); matNormalMap->Print(fptr);
    }
    fprintf(fptr,"\tambient:   %f\n",matAmbient);
    fprintf(fptr,"\tdiffuse:   %f\n",matDiffuse);
    fprintf(fptr,"}\n");
}


#include "GvsTexture.h"
#include "Ray/GvsSurfIntersec.h"


double GvsTexture :: sampleValue ( GvsSurfIntersec &intersec ) const {
    return (sampleColor(intersec)).luminance();
}


GvsColor GvsTexture :: sampleColor ( GvsSurfIntersec & ) const {
    return RgbBlack;
}


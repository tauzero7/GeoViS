#ifndef GVS_TEXTURE_H
#define GVS_TEXTURE_H

#include <iostream>
#include <Obj/GvsBase.h>

class GvsSurfIntersec;
class GvsColor;

enum GvsTextureType
{
    texture2D
};

class GvsTexture : public GvsBase
{
public:
    GvsTexture () {}

    virtual  double   sampleValue ( GvsSurfIntersec &intersec ) const;
    virtual  GvsColor sampleColor ( GvsSurfIntersec &intersec ) const;

    virtual  void     Print( FILE* fptr = stderr ) = 0;
};

#endif

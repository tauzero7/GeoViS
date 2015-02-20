#ifndef GVS_UNI_TEX_H
#define GVS_UNI_TEX_H

#include "GvsTexture.h"
#include "Img/GvsColor.h"


class GvsUniTex : public GvsTexture
{
  public:
    GvsUniTex ( const GvsColor &unicol );
    GvsUniTex ( double          unival );

    virtual double   sampleValue  ( GvsSurfIntersec &surfIntersec ) const;
    virtual GvsColor sampleColor  ( GvsSurfIntersec &surfIntersec ) const;

    GvsColor color    ( void                ) const;
    void     setColor ( const GvsColor &col );

    int     SetParam ( std::string pName, m4d::vec2 pt );

    virtual void Print( FILE* fptr = stderr );

  private:
    GvsColor uniColor;
    double   uniValue;
};

#endif

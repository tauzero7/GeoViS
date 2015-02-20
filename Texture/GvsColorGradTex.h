#ifndef GVS_COLORGRAD_TEX_H
#define GVS_COLORGRAD_TEX_H

#include "GvsTexture2D.h"
#include "Img/GvsColor.h"

enum gvsColorGradType {
    gvsColorGrad_linear = 0,
    gvsColorGrad_radial,
    gvsColorGrad_hsv,
    gvsColorGrad_texCoords
};


class GvsColorGradTex : public GvsTexture2D
{
public:
    GvsColorGradTex();
    GvsColorGradTex( const GvsColor &unicol1, const GvsColor &unicol2 );
    GvsColorGradTex( const GvsColor &unicol1, const GvsColor &unicol2,
                     const m4d::vec2 startPoint, const m4d::vec2 endPoint, std::string gType = "linear" );

    void SetColor1( const GvsColor &col );
    void SetColor2( const GvsColor &col );
    void SetColors( const GvsColor &col1, const GvsColor &col2 );

    void SetStartPoint( const m4d::vec2 point );
    void SetEndPoint( const m4d::vec2 point );
    void SetGradType( std::string gType );
   // void SetMinMaxRadii( const double rmin, const double rmax );

    virtual double   sampleValue  ( GvsSurfIntersec &surfIntersec ) const;
    virtual GvsColor sampleColor  ( GvsSurfIntersec &surfIntersec ) const;

    int     SetParam ( std::string pName, m4d::vec2 pt );
    int     SetParam ( std::string pName, m4d::vec3 pt );
    int     SetParam ( std::string pName, std::string gType );

    virtual void Print( FILE* fptr = stderr );

private:
    GvsColor uniColor1;
    GvsColor uniColor2;
    m4d::vec2 startPoint;
    m4d::vec2 endPoint;
    m4d::vec2 gradDir;
    double    gradDirLength;
   // double    rMin,rMax;
    gvsColorGradType gradType;
};

#endif

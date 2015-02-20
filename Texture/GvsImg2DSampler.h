#ifndef GVS_IMG_2D_SAMPLER_H
#define GVS_IMG_2D_SAMPLER_H


#include "GvsTexture2D.h"


class GvsChannelImg2D;

class GvsImg2DSampler : public GvsTexture2D
{
public:
    GvsImg2DSampler ();
    GvsImg2DSampler ( GvsChannelImg2D *img );
    GvsImg2DSampler ( GvsChannelImg2D *img, const m4d::Matrix<double,2,3>& tmat );

    GvsChannelImg2D* image () const;

    virtual double sampleValue ( double u, double v ) const;
    virtual double sampleValue ( GvsSurfIntersec  &intersec   ) const;

    virtual GvsColor sampleColor ( double u, double v ) const;
    virtual GvsColor sampleColor ( GvsSurfIntersec &intersec  ) const;

    virtual void Print( FILE* fptr = stderr );

private:
    GvsChannelImg2D *texImg;
};


#endif

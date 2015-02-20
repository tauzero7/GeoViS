
#include "GvsImg2DSampler.h"
#include "Img/GvsChannelImg2D.h"
#include "Ray/GvsSurfIntersec.h"


GvsImg2DSampler :: GvsImg2DSampler ()
{
    GvsChannelImg2D *img = new GvsChannelImg2D( 10, 10, 1 );
    uchar color = 255;
    for ( int i = 0; i < 10; i++ )
        for ( int j = 0; j < 10; j++ )
            img->setChannels( i, j, &color );

    texImg = img;
    assert( texImg != NULL );
}

GvsImg2DSampler :: GvsImg2DSampler ( GvsChannelImg2D *img )
{
    texImg = img;
    assert( texImg != NULL );
}

GvsImg2DSampler :: GvsImg2DSampler ( GvsChannelImg2D *img,
                                     const m4d::Matrix<double,2,3>& tmat       )
    : GvsTexture2D( tmat )
{
    texImg = img;
    assert( texImg != NULL );
}


void GvsImg2DSampler ::Print( FILE* fptr ) {
    fprintf(fptr,"Img2DSampler {\n");
    fprintf(fptr,"\ttransform:\n"); texTransformation.printS(fptr);
    if (texImg!=NULL) {
        texImg->Print(fptr);
    }
    fprintf(fptr,"}\n\n");
}


inline GvsChannelImg2D* GvsImg2DSampler ::image () const {
    return texImg;
}


double GvsImg2DSampler :: sampleValue( GvsSurfIntersec &intersec ) const {
    m4d::vec2 uv = intersec.texUVParam();
    return sampleValue ( uv.x(0) , uv.x(1) );
}


double GvsImg2DSampler :: sampleValue ( double u, double v ) const {
    m4d::vec2 uv = texTransformation * m4d::vec2(u,v);
    return texImg->sampleValue( uv.x(0), uv.x(1) );
}


GvsColor GvsImg2DSampler :: sampleColor ( GvsSurfIntersec &intersec ) const {
    m4d::vec2 uv = intersec.texUVParam();
    //uv.printS();
    return sampleColor ( uv.x(0), uv.x(1) );
}


GvsColor GvsImg2DSampler :: sampleColor ( double u, double v ) const {
    m4d::vec2 uv = texTransformation * m4d::vec2(u,v);
    return texImg->sampleColor( uv.x(0), uv.x(1) );
}

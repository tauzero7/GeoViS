
#include "GvsChequeredT2D.h"
#include <Ray/GvsSurfIntersec.h>
#include <Texture/GvsUniTex.h>
#include <cmath>
#include <cassert>


GvsChequeredT2D::GvsChequeredT2D () {
    tex0 = new GvsUniTex( 0 );
    tex1 = new GvsUniTex( 1 );
    assert( tex0 != NULL );
    assert( tex1 != NULL );
    borderWidth = 0.25;
}

GvsChequeredT2D::GvsChequeredT2D( double width ) {
    tex0 = new GvsUniTex( 0 );
    tex1 = new GvsUniTex( 1 );
    assert( tex0 != NULL );
    assert( tex1 != NULL );
    borderWidth = width;
    assert( borderWidth > 0.0 && borderWidth < 0.5 );
}

GvsChequeredT2D::GvsChequeredT2D ( GvsTexture *t0, GvsTexture *t1, double width ) {
    tex0 = t0;
    tex1 = t1;
    assert( tex0 != NULL );
    assert( tex1 != NULL );
    borderWidth = width;
    assert( borderWidth > 0.0 && borderWidth < 0.5 );
}

GvsChequeredT2D::GvsChequeredT2D (GvsTexture *t0, GvsTexture *t1,
                                   const m4d::Matrix<double,2,3> &mat, double width )
    : GvsTexture2D( mat ) {
    tex0 = t0;
    tex1 = t1;
    assert( tex0 != NULL );
    assert( tex1 != NULL );
    borderWidth = width;
    assert( borderWidth > 0.0 && borderWidth < 0.5 );
}

GvsChequeredT2D::~GvsChequeredT2D() {
    // Do not delete the textures here.
    tex0 = tex1 = NULL;
}

void GvsChequeredT2D :: Print( FILE* fptr ) {
    fprintf(fptr,"ChequeredT2D {\n");
    fprintf(fptr,"\ttransform:\n");
    texTransformation.printS(fptr);
    fprintf(fptr,"\tTexture 1:\n");
    tex0->Print(fptr);
    fprintf(fptr,"\tTexture 2:\n");
    tex1->Print(fptr);
    fprintf(fptr,"}\n");
}

double GvsChequeredT2D::sampleValue ( GvsSurfIntersec& intersec ) const {
    return (sampleColor(intersec)).luminance();
}

GvsColor GvsChequeredT2D::sampleColor ( GvsSurfIntersec& intersec ) const {
    m4d::vec2 q = texTransformation * intersec.texUVParam();

    double qU = fabs(q.x(0))-floor(fabs(q.x(0)));
    double qV = fabs(q.x(1))-floor(fabs(q.x(1)));

    if ( (qU>borderWidth) && (qU<(1.0-borderWidth)) && (qV>borderWidth) && (qV<(1.0-borderWidth)) ) {
        return tex0->sampleColor( intersec );
    } else {
        return tex1->sampleColor( intersec );
    }
}

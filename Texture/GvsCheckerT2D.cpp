
#include "GvsCheckerT2D.h"
#include <Ray/GvsSurfIntersec.h>
#include <Texture/GvsUniTex.h>


GvsCheckerT2D::GvsCheckerT2D ()
{
    tex0 = NULL;
    tex1 = NULL;
}

GvsCheckerT2D::GvsCheckerT2D( GvsTexture *t0, GvsTexture *t1 )
{
    tex0 = t0;
    assert( tex0 != NULL );
    tex1 = t1;
    assert( tex1 != NULL );
}

GvsCheckerT2D::GvsCheckerT2D( GvsTexture *t0, GvsTexture *t1,
                              const m4d::Matrix<double,2,3> &mat )
    : GvsTexture2D( mat )
{
    tex0 = t0;
    assert( tex0 != NULL );
    tex1 = t1;
    assert( tex1 != NULL );
}

GvsCheckerT2D::~GvsCheckerT2D ( ) {
    // Do not delete the textures here.
    tex0 = tex1 = NULL;
}


double GvsCheckerT2D::sampleValue ( GvsSurfIntersec &insec ) const {
    return (sampleColor(insec)).luminance();
}


GvsColor GvsCheckerT2D::sampleColor ( GvsSurfIntersec &insec ) const {
    m4d::vec2 q = texTransformation * insec.texUVParam();

    int c1 = (int)floor(q.x(0));
    int c2 = (int)floor(q.x(1));
    int checker = ((c1%2) == (c2%2));
    //int checker = ((Gvs_base(q.x(0))%2) == (Gvs_base(q.x(1))%2));
    if ( checker )
        return tex0->sampleColor( insec );
    else
        return tex1->sampleColor( insec );
}


void GvsCheckerT2D::Print( FILE* fptr ) {
    fprintf(fptr,"CheckerT2D {\n");
    fprintf(fptr,"\ttransform:\n");
    texTransformation.printS(fptr);
    fprintf(fptr,"\tTexture 1:\n");
    tex0->Print(fptr);
    fprintf(fptr,"\tTexture 2:\n");
    tex1->Print(fptr);
    fprintf(fptr,"}\n");
}

//
//

#include "GvsDieT2D.h"
#include <Ray/GvsSurfIntersec.h>
#include <Texture/GvsUniTex.h>

const double eyeRadius=0.1;

#if 0
const double eyePointDie[][2] = {
    {0.5,0.5},
    {0.2,0.2}, {0.8,0.8},
    {0.2,0.2}, {0.5,0.5}, {0.8,0.8},
    {0.2,0.2}, {0.8,0.2}, {0.2,0.8}, {0.8,0.8},
    {0.2,0.2}, {0.8,0.2}, {0.2,0.8}, {0.8,0.8}, {0.5,0.5},
    {0.2,0.2}, {0.2,0.5}, {0.2,0.8}, {0.8,0.2}, {0.8,0.5}, {0.8,0.8}
};
#else
const double eyePointDie[][2] = {
    {0.5,0.5},
    {0.25,0.25}, {0.75,0.75},
    {0.25,0.75}, {0.5,0.5},   {0.75,0.25},
    {0.25,0.25}, {0.75,0.25}, {0.25,0.75}, {0.75,0.75},
    {0.25,0.25}, {0.75,0.25}, {0.25,0.75}, {0.75,0.75}, {0.5,0.5},
    {0.25,0.25}, {0.25,0.5},  {0.25,0.75}, {0.75,0.27}, {0.75,0.5}, {0.75,0.75}
};
#endif

GvsDieT2D::GvsDieT2D() :
    tex0(NULL), tex1(NULL), numEyes(0) {
}

GvsDieT2D::GvsDieT2D( GvsTexture *t0, GvsTexture *t1, int eyes ) :
    tex0(t0), tex1(t1), numEyes(eyes) {
}

GvsDieT2D::GvsDieT2D( GvsTexture *t0, GvsTexture *t1,
                      const m4d::Matrix<double,2,3> &mat, int eyes )
    : GvsTexture2D( mat ),
      tex0(t0), tex1(t1), numEyes(eyes) {
}

GvsDieT2D::~GvsDieT2D() {
    tex0 = tex1 = NULL;
}

double GvsDieT2D::sampleValue ( GvsSurfIntersec &insec ) const {
    return (sampleColor(insec)).luminance();
}

GvsColor GvsDieT2D::sampleColor ( GvsSurfIntersec &insec ) const {
    //m4d::vec2 q = texTransformation * insec.texUVParam();
    m4d::vec2 q = insec.texUVParam();

    GvsSceneObj* obj = insec.object();
    if (obj==NULL) {
        return GvsColor(0);
    }

#if 0
    switch (insec.partIndex) {
        case 0: return GvsColor(1,0,0);
        case 1: return GvsColor(0,1,0);
        case 2: return GvsColor(0,0,1);
        case 3: return GvsColor(1,1,0);
        case 4: return GvsColor(0,1,1);
        case 5: return GvsColor(1,0,1);
    }
#endif

    if (isUVinEye(q, insec.partIndex) ) {
        return tex0->sampleColor( insec );
    } else {
        return tex1->sampleColor( insec );
    }
}

void GvsDieT2D::Print( FILE* fptr ) {
    fprintf(fptr,"CheckerT2D {\n");
    fprintf(fptr,"\ttransform:\n");
    texTransformation.printS(fptr);
    fprintf(fptr,"\tTexture 1:\n");
    tex0->Print(fptr);
    fprintf(fptr,"\tTexture 2:\n");
    tex1->Print(fptr);
    fprintf(fptr,"}\n");
}

bool GvsDieT2D::isUVinEye(m4d::vec2 u, int face) const {
    int n = 0;
    switch (face) {
        case 0: { n = 5; break; }
        case 1: { n = 0; break; }
        case 2: { n = 3; break; }
        case 3: { n = 2; break; }
        case 4: { n = 1; break; }
        case 5: { n = 4; break; }
        default:
            return false;
    }

    int noff = n*(n+1)/2;
    for (int i=0;i<=n;i++) {
        double x = eyePointDie[noff+i][0]-u.x(0);
        double y = eyePointDie[noff+i][1]-u.x(1);
        if (x*x+y*y <= eyeRadius*eyeRadius) return true;
    }
    return false;
}

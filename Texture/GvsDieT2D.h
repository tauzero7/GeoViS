#ifndef GVS_DIE_T2D_H
#define GVS_DIE_T2D_H

#include "GvsTexture2D.h"

class GvsDieT2D : public GvsTexture2D
{
public:
    GvsDieT2D ( void );
    GvsDieT2D ( GvsTexture *t0, GvsTexture *t1, int eyes = 6 );
    GvsDieT2D ( GvsTexture *t0, GvsTexture *t1, const m4d::Matrix<double,2,3> &mat, int eyes = 6) ;
    ~GvsDieT2D ( void );

    virtual void Print( FILE* fptr = stderr );

    virtual double   sampleValue ( GvsSurfIntersec & ) const;
    virtual GvsColor sampleColor ( GvsSurfIntersec & ) const;

    GvsTexture *texture0() {
        return tex0;
    }

    GvsTexture *texture1() {
        return tex1;
    }

    void setTexture0( GvsTexture *tex ) {
        tex0 = tex;
        assert( tex0 != NULL );
    }

    void setTexture1( GvsTexture *tex ) {
        tex1 = tex;
        assert( tex1 != NULL );
    }

protected:
    bool  isUVinEye(m4d::vec2 u, int face) const;

private:
    GvsTexture *tex0;
    GvsTexture *tex1;
    int numEyes;
};

#endif

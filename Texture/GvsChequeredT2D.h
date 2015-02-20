#ifndef GVS_CHEQUERED_T2D_H
#define GVS_CHEQUERED_T2D_H

#include "GvsTexture2D.h"

class GvsChequeredT2D : public GvsTexture2D
{
public:
    GvsChequeredT2D ();
    GvsChequeredT2D ( double width );
    GvsChequeredT2D ( GvsTexture *t0, GvsTexture *t1, double width );
    GvsChequeredT2D ( GvsTexture *t0, GvsTexture *t1, const m4d::Matrix<double,2,3>&, double width );
    ~GvsChequeredT2D ();

    virtual void  Print( FILE* fptr = stderr );

    virtual double   sampleValue ( GvsSurfIntersec& intersec ) const;
    virtual GvsColor sampleColor ( GvsSurfIntersec& intersec ) const;

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

private:
    GvsTexture *tex0;
    GvsTexture *tex1;
    double     borderWidth;
};


#endif

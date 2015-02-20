#ifndef GVS_TEXTURE_2D_H
#define GVS_TEXTURE_2D_H

#include <Texture/GvsTexture.h>

#include "math/Mat.h"


class GvsTexture2D : public GvsTexture
{
public:
    GvsTexture2D ( );
    GvsTexture2D ( const m4d::Matrix<double,2,3>& mat );

    void   setTransformation ( const m4d::Matrix<double,2,3>& mat );
    m4d::Matrix<double,2,3> transformation () const;

    void  translate         ( double tu, double tv );
    void  scale             ( double su, double sv );
    void  rotate            ( double angle         );
    void  rotate            ( double rotCenterX,
                              double rotCenterY,
                              double angle         );

    virtual  void     Print( FILE* fptr = stderr ) = 0;
protected:
    m4d::Matrix<double,2,3> texTransformation;
};

#endif

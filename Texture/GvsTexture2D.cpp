
#include "Texture/GvsTexture2D.h"

#include "math/TransfMat.h"


GvsTexture2D :: GvsTexture2D ( )
{
  texTransformation.setIdent();
}

GvsTexture2D :: GvsTexture2D ( const m4d::Matrix<double,2,3>& mat )
{
  texTransformation = mat;
}


void
GvsTexture2D :: setTransformation ( const m4d::Matrix<double,2,3>& mat )
{
  texTransformation = mat;
}

m4d::Matrix<double,2,3>
GvsTexture2D :: transformation ( ) const
{
  return texTransformation;
}

void
GvsTexture2D :: translate ( double tu, double tv )
{
  texTransformation = m4d::TranslateMat2D(-tu,-tv) * texTransformation;
}

void
GvsTexture2D :: scale ( double su, double sv )
{
  double suInv, svInv;

  if (fabs(su) < GVS_EPS)
  {
    std::cerr << "Error in GvsTexture2D::scale() : su = 0" << std::endl;
    suInv = 1.0;
  }
  else
    suInv = 1.0 / su;

  if (fabs(sv) < GVS_EPS)
  {
    std::cerr << "Error in RvsTexture2D::scale() : sv = 0" << std::endl;
    svInv = 1.0;
  }
  else
    svInv = 1.0 / sv;

  texTransformation = m4d::ScaleMat2D(suInv,svInv) * texTransformation;

}

void
GvsTexture2D :: rotate ( double angle )
{
  texTransformation = m4d::RotateMat2D(-angle) * texTransformation;
}

void
GvsTexture2D :: rotate ( double rotCenterX, double rotCenterY, double angle )
{
  texTransformation = m4d::RotateMat2D( rotCenterX, rotCenterY, -angle ) *
                      texTransformation;
}


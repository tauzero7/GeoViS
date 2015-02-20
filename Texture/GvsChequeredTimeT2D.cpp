/////////////////////////////////////////////////////////////////////////////
//
// .NAME    GvsChequeredTimeT2D.cxx
// .AUTHOR  Frank Grave, VIS, Stuttgart
// .DATE    27.10.2007
// .DESCRIPTION chequered paper texture, based on checker-texture
//
//             (source: RayViS, GvsChequeredTimeT2D, Thmoas Mueller)
// .CHANGED
//
//
/////////////////////////////////////////////////////////////////////////////

#include "GvsChequeredTimeT2D.h"
#include <Ray/GvsSurfIntersec.h>
#include <Texture/GvsUniTex.h>

#include <cmath>


using namespace std;

//------------------------------------------------------------------------
//   constructor/destructor
//------------------------------------------------------------------------

GvsChequeredTimeT2D :: GvsChequeredTimeT2D ( GvsProjectorBase *proj )
{
  tex0 = new GvsUniTex( 0 );
  assert( tex0 != NULL );
  tex1 = new GvsUniTex( 1 );
  assert( tex1 != NULL );
  deltaT = 1.0;
  mProjector = proj;
  assert ( mProjector != NULL );

  colorBehaviour  = (int)useTextures;
  deltaTBehaviour = (int)useDeltaT;
  timeBehaviour   = (int)coordTime;
  mStretch        = 1.0;
}

GvsChequeredTimeT2D :: GvsChequeredTimeT2D ( double dt, GvsProjectorBase *proj )
{
  tex0 = new GvsUniTex( 0 );
  assert( tex0 != NULL );
  tex1 = new GvsUniTex( 1 );
  assert( tex1 != NULL );
  deltaT = dt;
  mProjector = proj;
  assert ( mProjector != NULL );

  colorBehaviour  = (int)useTextures;
  deltaTBehaviour = (int)useDeltaT;
  timeBehaviour   = (int)coordTime;
  mStretch        = 1.0;
}

GvsChequeredTimeT2D :: GvsChequeredTimeT2D ( GvsTexture *t0, GvsTexture *t1, double dt,  GvsProjectorBase *proj )
{
  tex0 = t0;
  assert( tex0 != NULL );
  tex1 = t1;
  assert( tex1 != NULL );
  deltaT = dt;
  mProjector = proj;
  assert ( mProjector != NULL );

  colorBehaviour  = (int)useTextures;
  deltaTBehaviour = (int)useDeltaT;
  timeBehaviour   = (int)coordTime;
  mStretch        = 1.0;
}

GvsChequeredTimeT2D :: GvsChequeredTimeT2D ( GvsTexture *t0, GvsTexture *t1,
    const TMat2D& mat, double dt,  GvsProjectorBase *proj )
    : GvsTexture2D( mat )
{
  tex0 = t0;
  assert( tex0 != NULL );
  tex1 = t1;
  assert( tex1 != NULL );
  deltaT = dt;
  mProjector = proj;
  assert ( mProjector != NULL );

  colorBehaviour  = (int)useTextures;
  deltaTBehaviour = (int)useDeltaT;
  timeBehaviour   = (int)coordTime;
  mStretch        = 1.0;
}

GvsChequeredTimeT2D :: ~GvsChequeredTimeT2D ()
{
  //delete tex0,tex1;
  tex0 = tex1 = NULL;
}
//------------------------------------------------------------------------
//   inline     GvsChequeredTimeT2D :: s e t / g e t
//------------------------------------------------------------------------
inline GvsTexture* GvsChequeredTimeT2D :: texture0()
{
  return tex0;
}
inline GvsTexture* GvsChequeredTimeT2D :: texture1()
{
  return tex1;
}


inline void GvsChequeredTimeT2D :: setTexture0( GvsTexture *tex )
{
  tex0 = tex;
  assert( tex0 != NULL );
}

inline void GvsChequeredTimeT2D :: setTexture1( GvsTexture *tex )
{
  tex1 = tex;
  assert( tex1 != NULL );
}


//------------------------------------------------------------------------
//   method     GvsChequeredTimeT2D :: p r i n t
//------------------------------------------------------------------------

void GvsChequeredTimeT2D :: print( ostream& os )
{
  os << "ChequeredT2D {" << endl;
  os << "\ttransform: " << endl;
  texTransformation.print(os);
  os << "\tTexture 1:" << endl;
  tex0->print(os);
  os << "\tTexture 2:" << endl;
  tex1->print(os);
  os << "\tdeltaT: " << deltaT << endl;
  os << "}" << endl;
}

//------------------------------------------------------------------------
//   method     GvsChequeredTimeT2D :: s e t B e h a v i o u r
//------------------------------------------------------------------------
void GvsChequeredTimeT2D :: setBehaviour( chqTimeBhv bhv )
{
  switch ( bhv )
  {
    case useSpecrend:
      colorBehaviour   = (int)bhv;
      break;
    case useTextures:
      colorBehaviour   = (int)bhv;
      break;
    case useDeltaT:
      deltaTBehaviour  = (int)bhv;
      break;
    case useDeltaTLin:
      deltaTBehaviour  = (int)bhv;
      break;
    case useScale:
      deltaTBehaviour  = (int)bhv;
      break;
    case coordTime:
      timeBehaviour    = (int)bhv;
      break;
    case localObsTime:
      timeBehaviour    = (int)bhv;
      break;
    case localIntersecTime:
      timeBehaviour    = (int)bhv;
      break;
    default:
      break;
  }
}

//------------------------------------------------------------------------
//   method     GvsChequeredTimeT2D :: s e t S t r e t c h
//------------------------------------------------------------------------
void GvsChequeredTimeT2D :: setStretch( double stretch )
{
  mStretch = stretch;
}



//------------------------------------------------------------------------
//   method     GvsChequeredTimeT2D :: s a m p l e V a l u e
//------------------------------------------------------------------------

double GvsChequeredTimeT2D :: sampleValue ( GvsSurfIntersec* intersec ) const
{
  return (sampleColor(intersec)).luminance();
}

//------------------------------------------------------------------------
//   method     GvsChequeredTimeT2D :: s a m p l e C o l o r
//------------------------------------------------------------------------

GvsColor GvsChequeredTimeT2D :: sampleColor ( GvsSurfIntersec* intersec ) const
{
  //V2D q = texTransformation * intersec->texUVParam();
  double currProjCoordTime      = mProjector -> getPosition().x(0);
  double currIntersecCoordTime  = intersec   -> point().x(0);

  double timeDiff = 0.0;
  if ( timeBehaviour == (int)coordTime )
  {
//     timeDiff = currProjCoordTime - currIntersecCoordTime;
     timeDiff = currIntersecCoordTime;    
  }
  else if ( timeBehaviour == (int)localObsTime )
  {
    cerr << "timeBehaviour == (int)localObsTime missing" << endl;
  }
  else if ( timeBehaviour == (int)localIntersecTime )
  {
    cerr << "timeBehaviour == (int)localIntersecTime missing" << endl;
  }
  cout << currProjCoordTime << " " << currIntersecCoordTime << endl;
//   assert ( timeDiff >= 0.0 );
  cout << "timeDiff, deltaT : " << timeDiff << " " << deltaT << endl;


  double fac = 0.0;
  if ( deltaTBehaviour == (int)useDeltaT )
  {
    fac = floor( timeDiff/(deltaT) - floor(timeDiff/(deltaT)) + 0.5 );
  }
  else if ( deltaTBehaviour == (int)useDeltaTLin )
  {
    fac =  fabs(2.0*(timeDiff/(deltaT) - floor(timeDiff/(deltaT)))-1.0);
  }
  else if ( deltaTBehaviour == (int)useScale )
  {
    fac = atan(timeDiff*mStretch)/M_PI_2;
  }
//   cout << "fac : " << fac << endl;
  assert( fac >= 0.0 );


  GvsColor col;
  if ( colorBehaviour == (int)useTextures )
  {
    col = fac*tex0->sampleColor( intersec ) + (1.0-fac)*tex1->sampleColor( intersec );
    return col;
  }
  else if ( colorBehaviour == (int)useSpecrend )
  {
    cout << "colorBehaviour == (int)useSpecrend missing" << endl;
    return GvsColor(0.0,0.0,0.0);
  }

  return GvsColor(0.0,0.0,0.0);
}

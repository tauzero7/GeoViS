#ifndef GVS_CHEQUERED_TIME_T2D_H
#define GVS_CHEQUERED_TIME_T2D_H
/////////////////////////////////////////////////////////////////////////////
//
// .NAME    GvsChequeredTimeT2D.h
// .AUTHOR  Frank Grave, VIS Stuttgart
// .DATE    25.10.2007
// .DESCRIPTION
//
//             (source: RayViS, GvsChequeredTimeT2D, Thomas Mueller)
// .CHANGED
//
//
/////////////////////////////////////////////////////////////////////////////

#include "GvsTexture2D.h"
#include <Dev/GvsProjectorBase.h>

enum chqTimeBhv
{
  useSpecrend = 0,        //einfärben (nur Farbe)
  useTextures,            //einfärben (Mittelwert aus 2 Texturen)
  useDeltaT=10,           //verwende deltaT, Farbwert "springt"
  useDeltaTLin,           //verwende deltaT, Farbwert "pendelt"
  useScale,               //verwende kein deltaT, Farbverlauf quasilogarithmisch
  coordTime=20,           //Koordinatenzeit des Schnittpunktes wird verwendet
  localObsTime,           //lokale Zeit des Beobachters wird verwendet
  localIntersecTime       //lokale Zeit am Scnittpunkt wird verwendet
};

class GvsChequeredTimeT2D : public GvsTexture2D
{
  public:
    GvsChequeredTimeT2D ( GvsProjectorBase *proj);
    GvsChequeredTimeT2D ( double dt, GvsProjectorBase *proj );
    GvsChequeredTimeT2D ( GvsTexture *t0, GvsTexture *t1, double dt, GvsProjectorBase *proj );
    GvsChequeredTimeT2D ( GvsTexture *t0, GvsTexture *t1, const TMat2D& mat, double dt, GvsProjectorBase *proj );
    ~GvsChequeredTimeT2D ();

    virtual void print( ostream& os );

    void setBehaviour( chqTimeBhv bhv );
    void setStretch( double stretch );

    virtual double   sampleValue ( GvsSurfIntersec* intersec ) const;
    virtual GvsColor sampleColor ( GvsSurfIntersec* intersec ) const;

    GvsTexture *texture0();
    GvsTexture *texture1();

    void setTexture0( GvsTexture *tex );
    void setTexture1( GvsTexture *tex );

  private:
    GvsTexture *tex0;
    GvsTexture *tex1;
    double     deltaT;

    GvsProjectorBase *mProjector;

    int colorBehaviour;   //0=specrend, 1=textures
    int deltaTBehaviour;  //10=deltaT, 11=deltaTLin, 12=scale
    int timeBehaviour;    //20=coordTime, 21=localObsTime, 22=localIntersecTime
    double mStretch;      //Skalierungsfaktor (zieht arctan in die l�nge)
};


#endif

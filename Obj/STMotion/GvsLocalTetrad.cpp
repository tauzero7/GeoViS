// ---------------------------------------------------------------------
//  Copyright (c) 2013-2014, Universitaet Stuttgart, VISUS, Thomas Mueller
//
//  This file is part of GeoViS.
//
//  GeoViS is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  GeoViS is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with GeoViS.  If not, see <http://www.gnu.org/licenses/>.
// ---------------------------------------------------------------------

#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Utils/GvsGramSchmidt.h"
#include "math/TransfMat.h"

GvsLocalTetrad :: GvsLocalTetrad ( ) {
    for (int i=0;i<4;i++) {
        e[i].setX(i,1.0);
        base[i].setX(i,1.0);
    }
    inCoords = false;

    mRightHanded = true;
    mTau = 0.0;

    pos = m4d::vec4();
    vel = m4d::vec4(1.0,0.0,0.0,0.0);

    locTetradMetric = NULL;
    stBoundBox = NULL;

    AddParam("pos",gvsDT_VEC4);
    AddParam("e0",gvsDT_VEC4);
    AddParam("e1",gvsDT_VEC4);
    AddParam("e2",gvsDT_VEC4);
    AddParam("e3",gvsDT_VEC4);
    AddParam("calc",gvsDT_INT);
    AddParam("localvel",gvsDT_VEC3);
    AddParam("time",gvsDT_DOUBLE);
}

GvsLocalTetrad :: GvsLocalTetrad ( m4d::Metric* metric ) {
    //cout << "GvsLocalTetrad :: GvsLocalTetrad(GvsMetric* metric)" << std::endl;
    for (int i=0;i<4;i++)
    {
        e[i].setX(i,1.0);
        base[i].setX(i,1.0);
    }
    inCoords = false;
    lfType   = m4d::enum_nat_tetrad_default;

    mRightHanded = true;

    mTau = 0.0;
    pos = m4d::vec4();
    vel = m4d::vec4(1.0,0.0,0.0,0.0);

    locTetradMetric = metric;

    stBoundBox = NULL;

    AddParam("pos",gvsDT_VEC4);
    AddParam("e0",gvsDT_VEC4);
    AddParam("e1",gvsDT_VEC4);
    AddParam("e2",gvsDT_VEC4);
    AddParam("e3",gvsDT_VEC4);
    AddParam("calc",gvsDT_INT);
    AddParam("localvel",gvsDT_VEC3);
    AddParam("time",gvsDT_DOUBLE);
}

GvsLocalTetrad :: GvsLocalTetrad(const GvsLocalTetrad* lt) {
    //cout << "GvsLocalTetrad :: GvsLocalTetrad(const GvsLocalTetrad* lt)" << std::endl;
    mTau = lt->getLocalTime();
    pos = lt->getPosition();
    vel = lt->getVelocity();
    acc = lt->getAccel();
    for (int i=0;i<4;i++)
    {
        e[i]    = lt->getE(i);
        base[i] = lt->getBase(i);
    }
    inCoords = lt->getInCoords();
    lfType   = lt->getLFType();

    mRightHanded = lt->isRightHanded();

    locTetradMetric = lt->getMetric();

    if (locTetradMetric->breakCondition(pos)) {
        fprintf(stderr,"GvsLocalTetrad: invalid position, metric breaks down!\n");
        exit(1);
    }

    stBoundBox = new GvsBoundBox4D(*(lt->getSTBoundBox()));

    AddParam("pos",gvsDT_VEC4);
    AddParam("e0",gvsDT_VEC4);
    AddParam("e1",gvsDT_VEC4);
    AddParam("e2",gvsDT_VEC4);
    AddParam("e3",gvsDT_VEC4);
    AddParam("calc",gvsDT_INT);
    AddParam("localvel",gvsDT_VEC3);
    AddParam("time",gvsDT_DOUBLE);
}

GvsLocalTetrad :: GvsLocalTetrad(m4d::Metric* metric, const m4d::vec4 &p, const m4d::vec4 &v) {    
    mTau = 0.0;
    pos = p;
    vel = v;
    acc = m4d::vec4();
    for (int i=0;i<4;i++) {
        e[i].setX(i,1.0);
        base[i].setX(i,1.0);
    }
    inCoords = false;
    lfType   = m4d::enum_nat_tetrad_default;

    mRightHanded = true;

    //assert (metric!=NULL);
    locTetradMetric = metric;

    /*
    if (locTetradMetric->breakCondition(pos)) {
        fprintf(stderr,"GvsLocalTetrad: invalid position, metric breaks down!\n");
        exit(1);
    }
    */
    // test outside !!

    stBoundBox = NULL;

    AddParam("pos",gvsDT_VEC4);
    AddParam("e0",gvsDT_VEC4);
    AddParam("e1",gvsDT_VEC4);
    AddParam("e2",gvsDT_VEC4);
    AddParam("e3",gvsDT_VEC4);
    AddParam("calc",gvsDT_INT);
    AddParam("localvel",gvsDT_VEC3);
    AddParam("time",gvsDT_DOUBLE);
}

GvsLocalTetrad :: GvsLocalTetrad( m4d::Metric* metric,
                                  const m4d::vec4 &e_0, const m4d::vec4 &e_1, const m4d::vec4 &e_2, const m4d::vec4 &e_3,
                                  const m4d::vec4 &p, const bool coords)
{
    //cout << "GvsLocalTetrad :: GvsLocalTetrad( GvsMetric* m, const m4d::vec4 &e_i, const m4d::vec4 &p, const bool inCoords)" << std::endl;
    assert (metric!=NULL);
    locTetradMetric = metric;

    mTau = 0.0;
    pos = p;
    vel = m4d::vec4(1.0,0.0,0.0,0.0);
    acc = m4d::vec4();

    if (locTetradMetric->breakCondition(pos)) {
        fprintf(stderr,"GvsLocalTetrad: invalid position, metric breaks down!\n");
        exit(1);
    }

    setTetrad(e_0,e_1,e_2,e_3,false);
    //Print();
    //GvsGramSchmidt* gs = new GvsGramSchmidt(metric,pos,e_0,e_1,e_2,e_3);
    //mRightHanded = gs->isRightHanded();
    //delete gs;

    inCoords = coords;
    lfType   = m4d::enum_nat_tetrad_default;

    stBoundBox = NULL;

    AddParam("pos",gvsDT_VEC4);
    AddParam("e0",gvsDT_VEC4);
    AddParam("e1",gvsDT_VEC4);
    AddParam("e2",gvsDT_VEC4);
    AddParam("e3",gvsDT_VEC4);
    AddParam("calc",gvsDT_INT);
    AddParam("localvel",gvsDT_VEC3);
    AddParam("time",gvsDT_DOUBLE);
}

GvsLocalTetrad :: ~GvsLocalTetrad()
{
    DelAllParam();

    locTetradMetric = NULL;
    if (stBoundBox!=NULL) {
        delete stBoundBox;
        stBoundBox = NULL;
    }
}


//----------------------------------------------------------------------------
//         set/get Metric
//----------------------------------------------------------------------------
void
GvsLocalTetrad :: setMetric ( m4d::Metric* metric )
{
    locTetradMetric = metric;
}

m4d::Metric*
GvsLocalTetrad :: getMetric ( ) const
{
    assert (locTetradMetric!=NULL);
    return locTetradMetric;
}

//----------------------------------------------------------------------------
//         setLocalTetrad
//----------------------------------------------------------------------------
void
GvsLocalTetrad :: setLocalTetrad ( const GvsLocalTetrad &lt)
{
    // std::cerr << "GvsLocalTetrad :: setLocalTetrad ( const GvsLocalTetrad &lt)\n";
    mTau = lt.getLocalTime();
    pos = lt.getPosition();
    vel = lt.getVelocity();
    acc = lt.getAccel();
    for (int i=0;i<4;i++) {
        e[i]    = lt.getE(i);
        base[i] = lt.getBase(i);
    }
    inCoords = lt.getInCoords();
    locTetradMetric = lt.getMetric();

    GvsGramSchmidt* gs = new GvsGramSchmidt(lt.getMetric(),pos,e[0],e[1],e[2],e[3]);
    mRightHanded = gs->isRightHanded();
    delete gs;
}
//----------------------------------------------------------------------------
//         setLocalTime
//----------------------------------------------------------------------------
void GvsLocalTetrad::setLocalTime( double tau ) {
    mTau = tau;
}

//----------------------------------------------------------------------------
//         set/getTetrad, Pos,Vel,Acc
//----------------------------------------------------------------------------
void GvsLocalTetrad::setE( int k, const m4d::vec4 &e_k ) {
    e[k] = e_k;
}

void GvsLocalTetrad::setTriad( const m4d::vec4 &e_1, const m4d::vec4 &e_2, const m4d::vec4 &e_3 ) {
    e[1] = e_1;
    e[2] = e_2;
    e[3] = e_3;
}

void
GvsLocalTetrad :: setTetrad(const m4d::vec4 &e_0, const m4d::vec4 &e_1, const m4d::vec4 &e_2, const m4d::vec4 &e_3, bool gramSchmidt)
{
    e[0] = e_0;
    e[1] = e_1;
    e[2] = e_2;
    e[3] = e_3;

    calcInvert(e[0],e[1],e[2],e[3],base[0],base[1],base[2],base[3]);

    if (gramSchmidt)
    {
        GvsGramSchmidt* gs = new GvsGramSchmidt(locTetradMetric,pos,e[0],e[1],e[2],e[3]);
        mRightHanded = gs->isRightHanded();
        delete gs;
    }
}

void
GvsLocalTetrad :: getTetrad ( m4d::vec4 &e_0, m4d::vec4 &e_1, m4d::vec4 &e_2, m4d::vec4 &e_3 )
{
    e_0 = e[0];
    e_1 = e[1];
    e_2 = e[2];
    e_3 = e[3];
}

void
GvsLocalTetrad :: getInvTetrad ( m4d::vec4 &b_0, m4d::vec4 &b_1, m4d::vec4 &b_2, m4d::vec4 &b_3 )
{
    b_0 = base[0];
    b_1 = base[1];
    b_2 = base[2];
    b_3 = base[3];
}

void
GvsLocalTetrad :: setPosition(const m4d::vec4 &p)
{
    pos = p;
}

void
GvsLocalTetrad :: setPositionX( int coord, double val )
{
    pos.setX( coord, val );
}

void
GvsLocalTetrad :: setVelocity(const m4d::vec4 &v)
{
    vel = v;
}

void
GvsLocalTetrad :: setAccel(const m4d::vec4 &a)
{
    // noch nicht korrekt implementiert !!
    acc = a;
}

//----------------------------------------------------------------------------
//         calcInvert
//----------------------------------------------------------------------------
void
GvsLocalTetrad :: calcInvert ( void )
{
    register int i;
    m4d::mat4 mat;
    for (i=0;i<4;i++)
        mat.setRow(i,e[i]);

    mat.invert();

    for (i=0;i<4;i++)
        base[i] = mat.getRow(i);
}

void
GvsLocalTetrad :: calcInvert ( const m4d::vec4 &a0, const m4d::vec4 &a1, const m4d::vec4 &a2, const m4d::vec4 &a3,
                               m4d::vec4 &b0, m4d::vec4 &b1, m4d::vec4 &b2, m4d::vec4 &b3)
{
    m4d::mat4 mat;
    mat.setRow(0,a0);
    mat.setRow(1,a1);
    mat.setRow(2,a2);
    mat.setRow(3,a3);

    mat.invert();

    b0 = mat.getRow(0);
    b1 = mat.getRow(1);
    b2 = mat.getRow(2);
    b3 = mat.getRow(3);
}

//----------------------------------------------------------------------------
//         inCoords
//----------------------------------------------------------------------------
void
GvsLocalTetrad :: setInCoords ( const bool coords, const m4d::enum_nat_tetrad_type lft )
{
    inCoords = coords;
    lfType = lft;
}

bool
GvsLocalTetrad :: getInCoords ( void   ) const
{
    return inCoords;
}

void
GvsLocalTetrad :: setLFType   ( const m4d::enum_nat_tetrad_type lftype )
{
    lfType = lftype;
}

m4d::enum_nat_tetrad_type
GvsLocalTetrad :: getLFType   ( void ) const
{
    return lfType;
}


void GvsLocalTetrad::setLocalVel( const m4d::vec3 &v, const m4d::enum_nat_tetrad_type lft ) {
#if 0
    double vq = v.getNorm() * v.getNorm();    
    assert(vq <= 1.0);

    double gamma = 1.0/sqrt(1.0-vq);
    m4d::vec4 locVel(gamma,gamma*v.x(0),gamma*v.x(1),gamma*v.x(2));

    locTetradMetric->localToCoord(pos,locVel,vel,lft);
    adjustTetrad();
#else
    m4d::vec4 en[4];
    m4d::LorentzTransf LT(v.getNorm(),v);
    //LT.print();

    // reset initial tetrad
    for (int i=0;i<4;i++) {
        e[i] = m4d::vec4();
        e[i].setX(i,1.0);
    }

    for(int i=0; i<4; i++) {
        en[i] = m4d::vec4();
        for(int j=0; j<4; j++) {
            en[i] += LT.getElem(i,j)*e[j];
        }
    }

    for(int i=0; i<4; i++) {
        e[i] = en[i];
    }
    //calcInvert();
    calcInvert(e[0],e[1],e[2],e[3],base[0],base[1],base[2],base[3]);
#endif
}

/*double coordDiff(m4d::Metric* metric, const int c,  double x,  double y )
{
    bool periodic=false;
    double cmax;
    switch (metric->getCoordType())
    {
      case m4d::enum_coordinate_cartesian: {}
      case m4d::enum_coordinate_spherical: {
         if (c==2) {periodic= true; cmax=M_PI;}
         if (c==3) {periodic= true; cmax=2.0*M_PI;}
         break;
      }
      case m4d::enum_coordinate_cylinder: {
        if (c==2) {periodic= true; cmax=2.0*M_PI;}
        break;
      }
      default:{
        std::cerr<< "Achtung: Für 'coordinate_custom' kann eventuelle Periodizität von Koordinaten nicht berücksichtigt werden.\n";
      }
    }
    double diff;
    if (periodic)  // !!!!!!!!!!!! richtige Periode muss noch eingebaut werden !!!
      {

        diff = fmod(y-x,cmax);
        if (diff>0.5*cmax) diff = diff-cmax;
        else if (diff<-0.5*cmax) diff = diff+cmax;
      }
      else   // false
      {
        diff = y-x;
      }
    return diff;
}*/

//----------------------------------------------------------------------------
//         transform  coords <--> local Tetrad
//----------------------------------------------------------------------------
m4d::vec4 GvsLocalTetrad::transToLocTetrad ( const m4d::vec4 &point ) const {
    assert(locTetradMetric!=NULL);

    m4d::vec4 pLocal;

    if (inCoords) {
        // The local tetrad is given in coordinate representation. Hence, a point can be
        // directly transformed into the tetrade.        
        for (int i=0;i<4;i++) {
            pLocal[i] = 0.0;
            for (int m=0;m<4;m++) {
                pLocal[i] += locTetradMetric->coordDiff(m,pos.x(m),point.x(m)) * base[m].x(i);
            }
        }
    }
    else {
        // The local tetrad is given with respect to a natural local tetrad. Hence, a point
        // has to be transformed in the local tetrad first.
        m4d::vec4 newPoint;
            for (int i=0;i<4;i++) {
            pLocal[i] = 0.0;
            for (int m=0;m<4;m++) {
                pLocal[i] += newPoint.x(m) * base[m].x(i);
            }
        }
    }
    return pLocal;
}

m4d::vec4
GvsLocalTetrad :: transToCoords    ( const m4d::vec4 &point ) const
{
    m4d::vec4 pCoords;

    if (inCoords) {
        // The local tetrad is given in coordinate representation. Hence, a point within
        // the tetrad can be directly transformed into coordinates.
        for (int m=0;m<4;m++) {
            pCoords[m] = 0.0;
            for (int i=0;i<4;i++) {
                pCoords[m] += point.x(i)*e[i].x(m);  // ist das genau genug ????
            }
            pCoords[m] += pos.x(m);
        }
    }
    else {
        // Lokale Tetrade ist bzgl natuerlicher lokaler Tetrade an ihrem Ort gegeben. Ein Punkt
        // muss daher zunaechst auf die natuerliche Tetrade und anschliessend auf Koordinaten
        // transformiert werden.
        m4d::vec4 newPoint;
        std::cerr << "GvsLocalTetrad::transToCoords()... for inCoords=false not implemented yet!" << std::endl;
    }
    // pCoords[4] = 0;// point.x(4);
    return pCoords;
}

//----------------------------------------------------------------------------
//         adjustTetrad
//----------------------------------------------------------------------------
//  Zunaechst wird die Position und die Geschwindigkeit auf ihre Gueltigkeit
//  hin ueberprueft. Anschliessend wird die bereits grob vorgegebene Tetrade
//  so an die Bewegung angepasst, dass e0 in Richtung der Geschwindigkeit v
//  zeigt.

void GvsLocalTetrad :: adjustTetrad() {
    // Is the position within the valid domain?
    bool allowedPos = false;
    allowedPos = !locTetradMetric->breakCondition(pos);
    assert(allowedPos==true);

   // e[0].print();

    // ganz miieser haeck
    if (locTetradMetric->getMetricName()=="AlcubierreWarp")
    {
        m4d::vec4 lp = m4d::vec4(pos[0],pos[1],pos[2],pos[3]);
        m4d::vec4 b[4];
        locTetradMetric->getNatTetrad(pos,b[0],b[1],b[2],b[3],lfType);
        e[0] = m4d::vec4(b[0][0],b[0][1],b[0][2],b[0][3]);
        e[1] = m4d::vec4(b[1][0],b[1][1],b[1][2],b[1][3]);
        e[2] = m4d::vec4(b[2][0],b[2][1],b[2][2],b[2][3]);
        e[3] = m4d::vec4(b[3][0],b[3][1],b[3][2],b[3][3]);
        calcInvert(e[0],e[1],e[2],e[3],base[0],base[1],base[2],base[3]);
        return;
    }

    // ---------------------------------------------------------
    //  Determine the null-component of the coordinate velocity
    //  by meands of the initial condition g_{ab}v^a v^b = -1
    //  with v^a = dx^a/dlambda =
    //
    //  g_{00}(v^0)^2 + 2(g_{0j}v^j)v^0 + (g_{ij}v^iv^j + 1) = 0
    //    a   (v^0)^2 +      b     v^0 +       c
    // ---------------------------------------------------------

    locTetradMetric->calculateMetric(pos);
    double  a = locTetradMetric->getMetricCoeff(0,0);
    double  b = 0.0;
    double  c = 0.0;

    for (int i = 1; i < 4; i++) {
        b += locTetradMetric->getMetricCoeff(0,i)*vel.x(i);
        for (int j = 1; j < 4; j++) {
            c += locTetradMetric->getMetricCoeff(i,j)*vel.x(i)*vel.x(j);
        }
    }
    b *= 2.0;
    c += locTetradMetric->sign();

    double  d = b*b - 4.0*a*c;
    if (d < 0.0) {
        //std::cerr << "d: " << d << std::endl;
        //std::cerr << "dir: " << vel.x(1) << "  "  << vel.x(2) << "  " << vel.x(3) << std::endl;
        std::cerr << "Metric: ";
        std::cerr << locTetradMetric->getMetricName();
        std::cerr << ":  Error!! Velocity is not timelike!" << std::endl;
        return;
    }
    //assert(d>=0.0);
    d = sqrt(d);
    a *= 2.0;
    double u1 = (-b + d) / a;
    double u2 = (-b - d) / a;
    double w;

    if (u1 > u2)
        w = u1;
    else
        w = u2;

    vel.setX(0,w);

    // Null-Richtung der Tetrade wird tangentiell an die Bewegungrichtung angepasst.
    // Die restlichen Richtungen werden orthonormiert.
    e[0] = m4d::vec4(vel[0],vel[1],vel[2],vel[3]);

    // Ist die lokale Tetrade nicht in Koordinatendarstellung gegeben, so werden die
    // anderen drei Tetraden-Vektoren auf Koordinaten transformiert.
    if (inCoords==false) {
        for (int i=1;i<4;i++) {
            locTetradMetric->localToCoord(pos,e[i],e[i],lfType);
        }
    }

    // orthonormiere lokale Tetrade
    GvsGramSchmidt* gs = new GvsGramSchmidt(locTetradMetric,pos);
    gs->calculateTetrad(e[0],e[1],e[2],e[3]);
    delete gs;

    // e[mu] ist nun in Koordinatendarstellung. Wenn inCoords==false, dann wird jetzt
    // die Tetrade wieder in die natuerliche Tetrade vom Typ lfType transformiert.
    if (inCoords==false) {
        for (int i=0;i<4;i++) {
            locTetradMetric->coordToLocal(pos,e[i],e[i],lfType);
        }
    }

    // Zuletzt wird noch die Inverse Basis bestimmt.
    // Determine inverse basis vectors
    calcInvert(e[0],e[1],e[2],e[3],base[0],base[1],base[2],base[3]);    

    //e[0].print();
    //std::cerr << "___\n";
}


void GvsLocalTetrad::transformTetrad( const bool coords, const m4d::enum_nat_tetrad_type lft ) {
    //std::cerr << coords << " " << inCoords << std::endl;
    if ((coords != inCoords) || (lft != lfType)) {

        if ((coords==true) && (inCoords==false)) {
            // local tetrad is given wrt. natural local tetrad lft and has
            // to be transformed into coordinate representation
            for (int i=0;i<4;i++) {
                locTetradMetric->localToCoord(pos,e[i],e[i],lfType);
            }
            inCoords = true;
        }
        else if ((coords==false) && (inCoords==true)) {
            // local tetrad is given wrt. to coordinates and has
            // to be transformed into local representation
            for (int i=0;i<4;i++) {
                locTetradMetric->coordToLocal(pos,e[i],e[i],lft);
            }
            lfType = lft;
            inCoords = false;
        }
        else if ((coords==inCoords) && (inCoords==false))
        {
            //cerr << "lokal -> lokal\n";
            // lokale Tetrade ist bereits bzgl einer natuerlichen Tetrade dargestellt
            // soll aber jetzt bzgl einer anderen natuerlichen Tetrade dargestellt werden
            for (int i=0;i<4;i++)
            {
                locTetradMetric->localToCoord(pos,e[i],e[i],lfType);
                locTetradMetric->coordToLocal(pos,e[i],e[i],lft);
            }
            lfType = lft;
        }
    }

    // determine the inverse tetrad
    calcInvert(e[0],e[1],e[2],e[3],base[0],base[1],base[2],base[3]);
}

void
GvsLocalTetrad :: setSTBoundBox  ( GvsBoundBox4D* box )
{
    //cout << "setSTBoundBox" << std::endl;
    if (stBoundBox != NULL)
        delete stBoundBox;

    stBoundBox = new GvsBoundBox4D(*box);
}

GvsBoundBox4D*
GvsLocalTetrad :: getSTBoundBox  ( ) const
{
    return stBoundBox;
}


GvsLocalTetrad*
GvsLocalTetrad :: getInterpolatedTetrad ( GvsLocalTetrad* lt0, GvsLocalTetrad* lt1, double frak )
{
    GvsLocalTetrad* lt;
    if (frak<=0.0) {
        lt = new GvsLocalTetrad(lt0);
    } else if (frak>=1.0) {
        lt = new GvsLocalTetrad(lt1);
    } else {
        lt = new GvsLocalTetrad(lt0->getMetric());
        lt->setPosition((1.0-frak)*(lt0->getPosition()) + frak*(lt1->getPosition()));
        lt->setVelocity((1.0-frak)*(lt0->getVelocity()) + frak*(lt1->getVelocity()));
        lt->setAccel((1.0-frak)*(lt0->getAccel()) + frak*(lt1->getAccel()));

        lt->setInCoords(lt0->getInCoords());
        lt->setLFType(lt0->getLFType());
        lt->setLocalTime((1.0-frak)*(lt0->getLocalTime()) + frak*(lt1->getLocalTime()));

        for (int k=0;k<4;k++) {
            lt->setE(k,((1.0-frak)*(lt0->getE(k)) + frak*(lt1->getE(k))));
        }
        lt->calcInvert();
        
        GvsBoundBox4D* nbox = new GvsBoundBox4D();
        *nbox = *(lt0->getSTBoundBox()) + *(lt1->getSTBoundBox());
        lt->setSTBoundBox(nbox);
        delete nbox;
    }
    return lt;
}

int GvsLocalTetrad::SetParam ( std::string pName, int val ) {
    int isOkay = GvsBase::SetParam(pName,val);
    if (isOkay >= gvsSetParamNone && pName=="calc") {
        if (val==0) {
            adjustTetrad();
        }
        else if (val==1) {
            calcInvert();
            Print();
        }
    }
    return isOkay;
}

int GvsLocalTetrad::SetParam ( std::string pName, double val ) {
    int isOkay = GvsBase::SetParam(pName,val);
    if (isOkay >= gvsSetParamNone && pName=="time") {
        pos.setX(0,val);
        adjustTetrad();
    }
    return isOkay;
}

int GvsLocalTetrad::SetParam( std::string pName, m4d::vec4 pt ) {
    int isOkay = GvsBase::SetParam(pName,pt);
    if (isOkay >= gvsSetParamNone) {
        if (pName=="pos") {
            setPosition(pt);
            adjustTetrad();   // ???CHECK if allowed
        }
        else if (pName=="e0") {
            setE(0,pt);
        }
        else if (pName=="e1") {
            setE(1,pt);
        }
        else if (pName=="e2") {
            setE(2,pt);
        }
        else if (pName=="e3") {
            setE(3,pt);
        }
    }
    return isOkay;
}

int GvsLocalTetrad::SetParam ( std::string pName, m4d::vec3 vt ) {
    int isOkay = GvsBase::SetParam(pName,vt);
    if (isOkay >= gvsSetParamNone && pName=="localvel") {
        setLocalVel(vt);
    }
    return isOkay;
}

//----------------------------------------------------------------------------
//        getFourVelocity / getThreeVelocity
//----------------------------------------------------------------------------
//  Vierer- und Dreiergeschwindigkeit haengen ueber
//      u = gamma ( e_0 + v^1e_1 + v^2e_2 + v^3e_3)
//  miteinander zusammen.
//
m4d::vec4
GvsLocalTetrad :: getFourVelocity  ( const m4d::vec3 &v ) const
{
    double vel = v.x(0)*v.x(0) + v.x(1)*v.x(1) + v.x(2)*v.x(2);
    assert (vel <= 1.0);

    double gamma = 1.0/sqrt(1-vel);

    return m4d::vec4 ( gamma, gamma*v.x(0), gamma*v.x(1), gamma*v.x(2) );
}

// 3er-Geschwindigkeit ist gegeben durch:
//   v[0]   = Betrag der Geschwindigkeit
//   v[1-3] = Richtung der Geschwindigkeit
m4d::vec4
GvsLocalTetrad :: getFourVelocity  ( const m4d::vec4 &v ) const
{
    m4d::vec3 dir = m4d::vec3(v[1],v[2],v[3]);
    assert (dir != m4d::vec3(0,0,0));
    assert (fabs(v[0])<1.0);

    dir.normalize();

    dir *= v[0];

    return getFourVelocity(dir);
}

m4d::vec3
GvsLocalTetrad :: getThreeVelocity ( const m4d::vec4 &u ) const
{
    assert ( u.x(0) != 0.0 );

    return m4d::vec3 ( u.x(1)/u.x(0), u.x(2)/u.x(0), u.x(3)/u.x(0) );
}

//----------------------------------------------------------------------------
//         localToCoord / coordToLocal
//----------------------------------------------------------------------------
m4d::vec4
GvsLocalTetrad :: localToCoord ( const m4d::vec4 &vec ) const
{
    m4d::vec4 newVec;

    if (inCoords) {
        // If the tetrad is given in coordinate representation, then a vector
        // can be transformed to coordinates immediately:
        //
        //   y = y^n e_n = y^n e_n^mu d_mu = ty^n d_mu
        //
        for (int mu=0; mu<4; mu++) {
            newVec[mu]=0.0;
            for (int i=0; i<4; i++ ) {
                newVec[mu] += vec[i]*e[i][mu];
            }
        }
        return newVec;
    }
    else {
        // otherwise, you have to transform to the natural local tetrad and afterwards
        // into coordinates
        m4d::vec4 newnewVec;
        for (int mu=0; mu<4; mu++) {
            newVec[mu]=0.0;
            for (int i=0; i<4; i++ ) {
                newVec[mu] += vec[i]*e[i][mu];
            }
        }
        locTetradMetric->localToCoord(pos,newVec,newnewVec,lfType);
        return newnewVec;
    }
}

m4d::vec4
GvsLocalTetrad :: coordToLocal ( const m4d::vec4 &vec ) const
{
    m4d::vec4 newVec;

    if (inCoords)
    {
        // Liegt die Tetrade in Koordinatendarstellung vor, so kann man einen Vektor
        // direkt von Koordinaten auf die LT transformieren:
        for (int mu=0; mu<4; mu++)
        {
            newVec[mu]=0.0;
            for (int i=0; i<4; i++ )
                newVec[mu] += vec[i]*base[i][mu];
        }
        return newVec;
    }
    else
    {
        // ansonsten transformiert man zunaechst auf die natuerliche Tetrade und anschliessend
        // auf die eigentliche Tetrade
        m4d::vec4 oldVec;
        locTetradMetric->coordToLocal(pos,vec,oldVec,lfType);
        for (int mu=0; mu<4; mu++)
        {
            newVec[mu]=0.0;
            for (int i=0; i<4; i++ )
                newVec[mu] += oldVec[i]*base[i][mu];
        }
        return newVec;
    }
}

//----------------------------------------------------------------------------
//         print
//----------------------------------------------------------------------------

void
GvsLocalTetrad :: printP( ) const
{
    printf("-------\n");
    printf("Metrik: \n");
    printf("Pos: %10f %10f %10f %10f \n",pos.x(0),pos.x(1),pos.x(2),pos.x(3));
    printf("Vel: %10f %10f %10f %10f\n",vel.x(0),vel.x(1),vel.x(2),vel.x(3));
    printf("----\n");
    std::string inc = std::string("no");
    if (inCoords) inc = std::string("yes");
    std::string lfTypeName = m4d::stl_nat_tetrad_types[(int)lfType];
    printf("tetrad in coords: %s -> %s\n",inc.c_str(),lfTypeName.c_str());
    printf("e0:  %10f %10f %10f %10f\n",e[0].x(0),e[0].x(1),e[0].x(2),e[0].x(3));
    printf("e1:  %10f %10f %10f %10f\n",e[1].x(0),e[1].x(1),e[1].x(2),e[1].x(3));
    printf("e2:  %10f %10f %10f %10f\n",e[2].x(0),e[2].x(1),e[2].x(2),e[2].x(3));
    printf("e3:  %10f %10f %10f %10f\n",e[3].x(0),e[3].x(1),e[3].x(2),e[3].x(3));
    printf("right-handed: ");
    if (mRightHanded) printf("yes\n");
    else printf("no\n");
    printf("----\n");
    printf("b0:  %10f %10f %10f %10f\n",base[0].x(0),base[0].x(1),base[0].x(2),base[0].x(3));
    printf("b1:  %10f %10f %10f %10f\n",base[1].x(0),base[1].x(1),base[1].x(2),base[1].x(3));
    printf("b2:  %10f %10f %10f %10f\n",base[2].x(0),base[2].x(1),base[2].x(2),base[2].x(3));
    printf("b3:  %10f %10f %10f %10f\n",base[3].x(0),base[3].x(1),base[3].x(2),base[3].x(3));
    printf("-------\n");
}


void GvsLocalTetrad::Print( FILE* fptr  ) {
    fprintf(fptr,"LocalTetrad {\n");
    fprintf(fptr,"\tmetric:   %s\n",locTetradMetric->getMetricName().c_str());
    fprintf(fptr,"\tpropTime: %f\n",mTau);
    fprintf(fptr,"\tpos:      ");pos.printS(fptr);
    fprintf(fptr,"\tvel:      ");vel.printS(fptr);
    fprintf(fptr,"\tinCoords: %s\n",(inCoords?"yes":"no"));
    fprintf(fptr,"\t      e0: ");e[0].printS(fptr);
    fprintf(fptr,"\t      e1: ");e[1].printS(fptr);
    fprintf(fptr,"\t      e2: ");e[2].printS(fptr);
    fprintf(fptr,"\t      e3: ");e[3].printS(fptr);
#if 0
    fprintf(fptr,"\t   base0: ");base[0].printS(fptr);
    fprintf(fptr,"\t   base1: ");base[1].printS(fptr);
    fprintf(fptr,"\t   base2: ");base[2].printS(fptr);
    fprintf(fptr,"\t   base3: ");base[3].printS(fptr);
#endif
    fprintf(fptr,"}\n\n");
}

void GvsLocalTetrad::printS( std::ostream &os ) {
    os << mTau << " \t";
    os << pos.x(0)  << " " << pos.x(1)  << " " << pos.x(2)  << " " << pos.x(3)  << " \t";
    os << vel.x(0)  << " " << vel.x(1)  << " " << vel.x(2)  << " " << vel.x(3)  << " \t";
    os << e[0].x(0) << " " << e[0].x(1) << " " << e[0].x(2) << " " << e[0].x(3) << " \t";
    os << e[1].x(0) << " " << e[1].x(1) << " " << e[1].x(2) << " " << e[1].x(3) << " \t";
    os << e[2].x(0) << " " << e[2].x(1) << " " << e[2].x(2) << " " << e[2].x(3) << " \t";
    os << e[3].x(0) << " " << e[3].x(1) << " " << e[3].x(2) << " " << e[3].x(3) << " \t";
    os << base[0].x(0) << " " << base[0].x(1) << " " << base[0].x(2) << " " << base[0].x(3) << " \t";
    os << base[1].x(0) << " " << base[1].x(1) << " " << base[1].x(2) << " " << base[1].x(3) << " \t";
    os << base[2].x(0) << " " << base[2].x(1) << " " << base[2].x(2) << " " << base[2].x(3) << " \t";
    os << base[3].x(0) << " " << base[3].x(1) << " " << base[3].x(2) << " " << base[3].x(3) << std::endl;
}

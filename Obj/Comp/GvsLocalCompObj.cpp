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

#include "Obj/Comp/GvsLocalCompObj.h"
#include "Obj/STMotion/GvsStMotionGeodesic.h"

#include "metric/m4dMetric.h"


GvsLocalCompObj :: GvsLocalCompObj() :
    GvsSceneObj(),
    staticTetrad(NULL) {
    objList  = new GvsObjPtrList();

    mNumObjects = 0;
    mPointsWarning = true;
    mTimeBoxSize = 1e6;
}


GvsLocalCompObj :: GvsLocalCompObj ( GvsStMotion* motion ) : GvsSceneObj(),
    staticTetrad(NULL) {

    if (stMotion!=NULL)
        delete stMotion;
    stMotion = motion;
    haveMotion = true;

    objList  = new GvsObjPtrList();

    mNumObjects = 0;
    mPointsWarning = true;
    mTimeBoxSize = 4.0;
}

GvsLocalCompObj :: ~GvsLocalCompObj() {
    if (objList != NULL) {
        objList->clear();
        delete objList;

        objList = NULL;
    }

    staticTetrad = NULL;

    mNumObjects = 0;
    mPointsWarning = true;
}

void
GvsLocalCompObj :: setLocalTetrad ( GvsLocalTetrad* locT )
{
    if (haveMotion)
        stMotion->setLocalTetrad(locT);
    else
    {
        staticTetrad = locT;
        staticTetrad->transformTetrad(true); // transform to coordinates
        //staticTetrad->Print();
    }
}

GvsLocalTetrad*
GvsLocalCompObj :: getLocalTetrad ( int k ) const
{
    std::cerr << "GvsLocalCompObj :: getLocalTetrad\n";
    if (haveMotion)
    {
        assert ( k < numPositions );
        return stMotion->getLocalTetrad(k);
    }
    else
        return staticTetrad;
}


//----------------------------------------------------------------------------
//         set/get motion
//----------------------------------------------------------------------------
void
GvsLocalCompObj :: setMotion ( GvsStMotion *motion )
{
    if (stMotion!=NULL) delete stMotion;
    stMotion = motion;
    haveMotion = true;

    numPositions = stMotion->getNumPositions();

    if (objList->length() != 0) calcSTBoundBoxComplete();
}

GvsStMotion*
GvsLocalCompObj :: getMotion ( void ) const
{
    return stMotion;
}

void GvsLocalCompObj::setTimeBoxSize( double timeBoxSize ) {
    mTimeBoxSize = timeBoxSize;
}

void GvsLocalCompObj :: Add ( GvsSceneObj *obj ) {
    assert ( obj != NULL );

    objList->Add(obj);
    compBoundBox += obj->boundingBox();
    mNumObjects = objList->length();
    calcSTBoundBox(0);
}


GvsSceneObj* GvsLocalCompObj::getObj ( int nr ) const {
    assert (nr < mNumObjects);
    return objList->getObj(nr);
}

int GvsLocalCompObj::getNumObjs ( void ) const {
    return mNumObjects;
}

/*void
        calcDist  (m4d::Metric* cMetric, const m4d::vec4 &p1, const m4d::vec4 &p2, double &spaceDist, double &timeDist )
        {
          //  p1.print();
          //  p2.print();
          //assert(p1.x(4)==p2.x(4));


          bool cr[4];
          cr[0] =true;
          cr[1] = cr[2] = cr[3] = false;
          cMetric->calculateMetric(p1);

          spaceDist = 0.0;
          timeDist  = 0.0;
          for (int i=0; i<4; i++)
          {
            for (int j=0; j<4; j++)
            {
              if ( !(cr[i]) && !(cr[j]) ) // CoordRange??
               // spaceDist += cMetric->getMetricCoeff(i,j)*(p2.x(i)-p1.x(i))*(p2.x(j)-p1.x(j));
                spaceDist += cMetric->getMetricCoeff(i,j)*cMetric->coordDiff(i,p1.x(i),p2.x(i))*cMetric->coordDiff(j,p1.x(j),p2.x(j));
              if (  (cr[i]) &&  (cr[j]) )
                //timeDist  -= cMetric->getMetricCoeff(i,j)*(p2.x(i)-p1.x(i))*(p2.x(j)-p1.x(j));
                timeDist  -= cMetric->getMetricCoeff(i,j)*cMetric->coordDiff(i,p1.x(i),p2.x(i))*cMetric->coordDiff(j,p1.x(j),p2.x(j));
            }
          }
          if (spaceDist>=0.0) spaceDist = sqrt(spaceDist);
          //else { std::cerr << "SpaceDist < 0 \n"; }
          if (timeDist >=0.0) timeDist  = sqrt(timeDist );
          //else { std::cerr << "TimeDist < 0 \n"; }
        }
*/


bool GvsLocalCompObj :: testIntersection( GvsRay &ray ) {
    // std::cerr << "GvsLocalCompObj :: testIntersection()\n";
    assert (mNumObjects > 0);

    GvsSceneObj* obj = NULL;

    m4d::Metric* stMetric = NULL;
    GvsLocalTetrad* locT0 = NULL;
    GvsLocalTetrad* locT1 = NULL;
    //double          localTime0, localTime1;
    m4d::vec4 pos;
    m4d::vec4 box0,box1;

    double t0,t1;
    double spaceDist0,timeDist0,spaceDist1,timeDist1;

    bool result;
    bool intersecFound = false;

    if (!haveMotion) {
        // locT0    = stMotion->getLocalTetrad(0);
        locT0    = staticTetrad;
        stMetric = locT0->getMetric();       // metric at position of local tetrad
        pos      = locT0->getPosition();     // position of the local tetrad

        box0 = (locT0->getSTBoundBox())->uppBounds();
        //box0.print();

        locT1 = locT0;

        int maxSeg   = ray.getNumPoints()-2;
        int startSeg = int(0);
        int endSeg   = maxSeg;

        for (int seg = startSeg; seg < endSeg; seg++) {
            m4d::vec4 p0 = ray.getPoint(seg);
            m4d::vec4 p1 = ray.getPoint(seg+1);

            stMetric->calcSepDist(pos,p0,spaceDist0,timeDist0);
            stMetric->calcSepDist(pos,p1,spaceDist1,timeDist1);

            if ( ((fabs(spaceDist0)<box0.x(1)) && (fabs(timeDist0)<box0.x(0))) ||
                 ((fabs(spaceDist1)<box0.x(1)) && (fabs(timeDist1)<box0.x(0))) )
            {
                // std::cerr << "Hit STBoundBox\n";

                m4d::vec4 p0loc = locT0->transToLocTetrad(p0);
                m4d::vec4 p1loc = locT0->transToLocTetrad(p1);

                for(int i = 0; i < (objList->length()); i++ ) {
                    obj = objList->getObj(i);
                    result = obj->testLocalIntersection(ray,seg,locT0,locT1,p0loc,p1loc);
                    intersecFound = intersecFound || result;
                }
            }
        }
    }
    else {
        // local object in motion
        assert(stMotion!=NULL);

        int maxSeg   = ray.getNumPoints()-2;
        int startSeg = int(0);
        int endSeg   = maxSeg;

        int numMotionPos = stMotion->getNumPositions();

        for(int seg = startSeg; seg < endSeg; seg++) {
            m4d::vec4 p0 = ray.getPoint(seg);
            m4d::vec4 p1 = ray.getPoint(seg+1);

            t0 = p0.x(0);
            t1 = p1.x(0);

            //fprintf(stderr,"%8.4f %8.4f %8.4f  %8.4f %8.4f.. ",t0,t1,t0*t1,p0.x(1),ray.getTangente(seg).x(0));

            // local tetrad which is closest to the light ray segment at t0
            int num0;
            locT0 = stMotion->getClosestLT(t0,num0);
            if (locT0 == NULL) {
                continue;
            }
            //localTime0 = stMotion->getLocalTime(num0);

            stMetric = locT0->getMetric();
            pos      = locT0->getPosition();

            if (!stMetric->calcSepDist( pos,p0,spaceDist0,timeDist0)) {
                continue;
            }
            box0  = (locT0->getSTBoundBox())->uppBounds();

            
            // local tetrad which is closest to the light ray segment at t1
            int num1;
            locT1 = stMotion->getClosestLT(t1,num1);
            if (locT1 == NULL) {
                continue;
            }
            //localTime1 = stMotion->getLocalTime(num1);

            if ((num0==num1) && (num0!=0))
            {
                if ((num0+1) >= numMotionPos)
                {
                    if ( mPointsWarning )
                    {
                        std::cerr << "Motion has not enough points !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                        mPointsWarning = false;
                        continue;
                    }
                   // localTime1 = localTime0;
                }
                else
                {
                    locT1 = stMotion->getLocalTetrad(num0-1);
                   // localTime1 = stMotion->getLocalTime(num0-1);
                    //std::cerr << "......................................\n";
                }
            }

            stMetric = locT1->getMetric();
            pos      = locT1->getPosition();

            if (!stMetric->calcSepDist( pos,p1,spaceDist1,timeDist1)) {
                continue;
            }
            box1  = (locT1->getSTBoundBox())->uppBounds();

            //cerr << "box: " << box0.x(1) << " " << box1.x(1) << "  " << box0.x(0) << " " << box1.x(0) << std::endl;
            //cerr << spaceDist0 << " " << spaceDist1 << "  " << timeDist0 << " " << timeDist1 << std::endl;
            //fprintf(stderr,"%8.4f %8.4f %8.4f %8.4f\n",spaceDist0,spaceDist1,timeDist0,timeDist1);

            //printf("--------------  localTime0 localTime1 %f %f\n",localTime0,localTime1);

            // Is one of the points inside the space-time bubble ?
            // Note, only the first component is necessary for spaceDist comparison, see calcSTBoundBox
            if ( ((fabs(spaceDist0)<box0.x(1)) && (fabs(timeDist0)<box0.x(0))) ||
                 ((fabs(spaceDist1)<box1.x(1)) && (fabs(timeDist1)<box1.x(0))) )       
            {
                // transform points into local tetrad
                m4d::vec4 p0loc = locT0->transToLocTetrad(p0);
                m4d::vec4 p1loc = locT1->transToLocTetrad(p1);

                for(int i = 0; i < (objList->length()); i++ ) {
                    obj = objList->getObj(i);
                    result = obj->testLocalIntersection(ray,seg,locT0,locT1,p0loc,p1loc);
                    intersecFound = intersecFound || result;
                }
                if (intersecFound) return true;
            }
        }
    }
    return intersecFound;
}


GvsBoundBox GvsLocalCompObj::boundingBox( ) const {
    return compBoundBox;
}


void GvsLocalCompObj::calcSTBoundBox ( int k ) {
    m4d::vec3 low = compBoundBox.lowBounds();
    m4d::vec3 upp = compBoundBox.uppBounds();

    double m1 = GVS_MAX(fabs(low.x(0)),fabs(upp.x(0)));
    double m2 = GVS_MAX(fabs(low.x(1)),fabs(upp.x(1)));
    double m3 = GVS_MAX(fabs(low.x(2)),fabs(upp.x(2)));
    //fprintf(stderr,"GvsLocalCompObj::calcSTBoundBox... %f %f %f\n",m1,m2,m3);

    GvsLocalTetrad* locT;
    if (haveMotion)
        locT = stMotion->getLocalTetrad(k);
    else
        locT = staticTetrad;

    // mTimeBoxSize should be quite large for static objects: initial value = 1e6
    // for moving objects, however, the value should be small.
    m4d::vec4 maxBound;
    maxBound = m4d::vec4(mTimeBoxSize,m1,m2,m3);

    // Transform position vector into coordinate system
    m4d::vec4 maxBoundCoords = locT->transToCoords(maxBound);

    // Determine maximum spacelike and timelike distance of the bounding box
    m4d::Metric* metric = locT->getMetric();
    double spaceDist,timeDist;
    metric->calcSepDist(locT->getPosition(),maxBoundCoords,spaceDist,timeDist);

    GvsBoundBox4D* stBoundBox = new GvsBoundBox4D(m4d::vec4(-timeDist,-spaceDist,0,0),m4d::vec4(timeDist,spaceDist,0,0));
    if (haveMotion)
        stMotion->setSTBoundBox(stBoundBox,k);
    else
        staticTetrad->setSTBoundBox(stBoundBox);
    delete stBoundBox;
}


void GvsLocalCompObj::calcSTBoundBoxComplete() {
    // Berechne zu jedem Ort der Bewegung eine stBoundBox und uebergib diese an die zugehoerige
    // lokale Tetrade in der Bewegungsklasse
    //  std::cerr << "GvsLocalCompObj::calcStBoundBoxComplete()...";

    if (haveMotion) {
        int num = stMotion->getNumPositions();
        //  std::cerr << "GvsLocalCompObj :: calcSTBoundBoxComplete: " << num << std::endl;
        for ( int i=0; i < num; i++) {
            calcSTBoundBox(i);
        }
    }
    else {
        calcSTBoundBox();
    }
}


void GvsLocalCompObj::calcSTBoundBoxPartial( int fromPos, int toPos ) {
    if (haveMotion)
    {
        int num = stMotion->getNumPositions();
        assert ( (fromPos < num) && (toPos < num) );

        if (fromPos==toPos) calcSTBoundBox(fromPos);
        else
        {
            int i,von,bis;
            if ( fromPos < toPos )
            {
                von = fromPos;
                bis = toPos;
            }
            else
            {
                von = toPos;
                bis = fromPos;
            }

            i = von;
            do
            {
                calcSTBoundBox(i++);
            }
            while (i<bis);
        }
    }
    else
        calcSTBoundBox();
}


void GvsLocalCompObj :: Print( FILE* fptr ) {
    fprintf(fptr,"LocalCompObject {\n");

    //GvsSceneObj* obj;
    if (!haveMotion) {
        staticTetrad->Print(fptr);
    } else {
        if (stMotion->getMotionType() == gvsMotionGeodesic ) {

        }
        fprintf(fptr,"first point: "); stMotion->getFirstPos().printS(fptr);
        fprintf(fptr,"last point:  "); stMotion->getLastPos().printS(fptr);
    }
    fprintf(fptr,"movTimeBoxSize: %f\n",mTimeBoxSize);
    fprintf(fptr,"#objects: %d\n",mNumObjects);
    fprintf(fptr,"}\n");
    /*

  os << "-----------------------------------------\n";
  os << "            LocalCompObject   :" << std::endl;
  os << "-----------------------------------------\n";
  if (!haveMotion)
  {
    os << "Lokale Tetrade (ruhend):" << std::endl;
    staticTetrad->print(os);
  }
  else
  {
    if (stMotion->getMotionType() == gvsMotionGeodesic )
    {
      os << "Geodaetische Bewegung:" << std::endl;
      GvsStMotionGeodesic* gmo = dynamic_cast<GvsStMotionGeodesic*>(stMotion);
      (gmo->getSolver())->print(os);
    }
    os << "Bewegung laeuft vom Punkt: ";
    (stMotion->getFirstPos()).print(os);
    os << "            bis zum Punkt: ";
    (stMotion->getLastPos()).print(os);
  }
  os << "\n\tAnzahl Objekte: " << mNumObjects << std::endl;
  for (int i=0; i<mNumObjects; i++)
  {
    obj = getObj(i);
    obj->print(os);
  }
  compBoundBox.print(os);
  os << "-----------------------------------------\n";
  */
}


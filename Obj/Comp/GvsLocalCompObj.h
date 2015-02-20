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
#ifndef GVS_LOCAL_COMPOUND_OBJ_H
#define GVS_LOCAL_COMPOUND_OBJ_H


#include <Obj/STMotion/GvsLocalTetrad.h>
#include <Obj/GvsBoundBox.h>
#include <Obj/GvsBoundBox4D.h>
#include <Obj/GvsObjPtrList.h>
#include <Obj/GvsSceneObj.h>
#include <Obj/STMotion/GvsStMotion.h>
#include <Ray/GvsRay.h>
#include <iostream>


class GvsLocalCompObj : public GvsSceneObj
{
public:
    GvsLocalCompObj ( );
    GvsLocalCompObj ( GvsStMotion* motion );

    virtual ~GvsLocalCompObj();


    void            setLocalTetrad ( GvsLocalTetrad* locT );
    GvsLocalTetrad* getLocalTetrad ( int k=0 ) const;
    
    virtual void          setMotion ( GvsStMotion *motion );
    virtual GvsStMotion*  getMotion ( void ) const;

    void   setTimeBoxSize( double timeBoxSize );

    virtual void Add        ( GvsSceneObj *obj );

    GvsSceneObj* getObj     ( int nr ) const;
    int          getNumObjs ( void )   const;


    virtual bool testIntersection( GvsRay &ray );

    virtual GvsBoundBox  boundingBox() const;

    virtual void  calcSTBoundBox ( int k = 0 );                       // Berechne stBoundBox am Ort der Nr. k
    virtual void  calcSTBoundBoxComplete ( );                         // Berechne StBoundBox fuer alle Orte
    virtual void  calcSTBoundBoxPartial  ( int fromPos, int toPos );  // Berechne stBoundBox fuer einen Bereich

    virtual void  Print( FILE* fptr = stderr );


protected:
    GvsLocalTetrad*  staticTetrad;

    GvsBoundBox      compBoundBox;  // Bounding Box with respect to local frame

    GvsObjPtrList*   objList;
    int              mNumObjects;

    bool             mPointsWarning;
    
    // (Moving) time box size
    // ... can be very crucial for particular spacetimes and motion (e.g. Kerr)
    // ... is used in calcSTBoundBox
    // ... set in scm via timeBoxSize
    double           mTimeBoxSize;
};


#endif

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
#ifndef GVS_COMPOUND_OBJ_H
#define GVS_COMPOUND_OBJ_H


#include "Obj/GvsBoundBox.h"
#include "Obj/GvsBoundBox4D.h"
#include "Obj/GvsObjPtrList.h"
#include "Obj/GvsSceneObj.h"
#include "Obj/STMotion/GvsStMotion.h"
#include "Ray/GvsRay.h"


class GvsCompoundObj : public GvsSceneObj
{
public:
    GvsCompoundObj();
    virtual ~GvsCompoundObj();

    virtual void Add        ( GvsSceneObj *obj );

    GvsSceneObj* getObj     ( unsigned int nr ) const;
    unsigned int getNumObjs () const;


    virtual bool testIntersection(GvsRay &ray);

    virtual GvsBoundBox    boundingBox    ( ) const;

    virtual void transform (const m4d::Matrix<double,3,4>& );

    virtual void Print( FILE* fptr = stderr );


protected:
    GvsBoundBox     compBoundBox;  // Bounding Box with respect to local frame
    GvsObjPtrList*  objList;
    unsigned int    mNumObjects;
};


#endif

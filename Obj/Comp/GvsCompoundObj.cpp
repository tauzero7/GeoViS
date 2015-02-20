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

#include <iostream>
#include "Obj/Comp/GvsCompoundObj.h"

GvsCompoundObj :: GvsCompoundObj() : GvsSceneObj() {
    objList  = new GvsObjPtrList();
    mNumObjects = 0;
}

GvsCompoundObj::~GvsCompoundObj() {
    if (objList != NULL) {
        objList->clear();
        delete objList;
        objList = NULL;
    }
    mNumObjects = 0;
}


void GvsCompoundObj::Add( GvsSceneObj *obj ) {
    objList->Add(obj);
    compBoundBox += obj->boundingBox();
    mNumObjects = objList->length();
}


GvsSceneObj* GvsCompoundObj::getObj( unsigned int nr ) const {
    if (nr<mNumObjects) {
        return objList->getObj(nr);
    }
    return NULL;
}

unsigned int GvsCompoundObj::getNumObjs() const {
    return mNumObjects;
}

bool GvsCompoundObj::testIntersection(GvsRay &ray) {
    register int i;
    GvsSceneObj* obj;
    bool intersecFound = false;
    for (i = 0; i < (objList->length()); i++ ) {
        obj = objList->getObj(i);
        if (obj!=NULL) {
            bool result = obj->testIntersection(ray);
            intersecFound = intersecFound || result;
        }
    }
    return intersecFound;
}


GvsBoundBox GvsCompoundObj::boundingBox() const {
    return compBoundBox;
}

void GvsCompoundObj::Print( FILE* fptr ) {
    GvsSceneObj* obj;

    fprintf(fptr,"CompoundObject {\n");
    fprintf(fptr,"\t# objects: %d\n",mNumObjects);
    for(unsigned int i=0; i<mNumObjects; i++) {
        obj = getObj(i);
        if (obj!=NULL) {
            obj->Print(fptr);
        }
    }
    fprintf(fptr,"}\n\n");
}


void GvsCompoundObj::transform (const m4d::Matrix<double,3,4> &mat ) {
    GvsSceneObj* obj;
    for(int i = 0; i < (objList->length()); i++ ) {
        //std::cerr << i << std::endl;
        //objList->getObj(i)->Print();
        obj = objList->getObj(i);
        if (obj!=NULL) {
            obj->transform(mat);
        }
    }
}


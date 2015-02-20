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

#include "Obj/GvsObjPtrList.h"


GvsObjPtrList::GvsObjPtrList() {
    if (objList.size()!=0) {
        clear();
    }
    listLength = 0;
}

GvsObjPtrList::~GvsObjPtrList() {
    clear();
}


void GvsObjPtrList::clear() {
    if (!objList.empty())
        objList.clear();

    listLength = 0;
}

void GvsObjPtrList::Add( GvsSceneObj *obj ) {
    objList.push_back(obj);
    listLength = objList.size();
}

void
GvsObjPtrList::append( GvsSceneObj *obj )  // hinten anhaengen
{
    objList.push_back(obj);
    listLength = objList.size();
    // cout << "Object appended." << std::endl;
}

void GvsObjPtrList::print() {
    for (int i=0;i<listLength;i++) {
        printf("Objekt %2i: ",i);
        objList[i]->Print();
    }
}

int GvsObjPtrList::length() const {
    return listLength;
}


GvsSceneObj* GvsObjPtrList :: getObj ( int index ) {
    return objList[index];
}

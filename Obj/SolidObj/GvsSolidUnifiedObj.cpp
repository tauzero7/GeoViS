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

#include <Obj/SolidObj/GvsSolObjSpanList.h>
#include "GvsSolidUnifiedObj.h"


GvsSolidUnifiedObj :: GvsSolidUnifiedObj( GvsSolidObj *child0, GvsSolidObj *child1 )
    : GvsSolidCSGObj(child0,child1 ) {
    setCSGType(CSG_unifiedObj);
}


void GvsSolidUnifiedObj::Print( FILE* fptr ) {
    fprintf(fptr,"SolidUnifiedObj {\n");
    /*
  os << "csgType gpCSGUnifiedObj" << endl;
  childSolObj[0] -> print( os );
  childSolObj[1] -> print( os );
  os << "}" << endl;
  */
}


GvsBoundBox GvsSolidUnifiedObj :: boundingBox() const {
    return childSolObj[1]->boundingBox() + childSolObj[0]->boundingBox();
}


bool GvsSolidUnifiedObj :: getRaySpanList ( GvsRay& ray, GvsSolObjSpanList& spanList )
{
    GvsSolObjSpanList spanList0, spanList1;

    bool insec0 = childSolObj[0]->getRaySpanList( ray, spanList0 );
    bool insec1 = childSolObj[1]->getRaySpanList( ray, spanList1 );

    if ( insec0 || insec1 )
    {
        spanList = spanList0 + spanList1;
        return true;
    }

    return false;
}

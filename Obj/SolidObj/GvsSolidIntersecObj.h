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
#ifndef GVS_SOLID_INTERSEC_OBJ_H
#define GVS_SOLID_INTERSEC_OBJ_H

#include "GvsSolidObj.h"
#include "GvsSolidCSGObj.h"


class GvsSolidIntersecObj : public GvsSolidCSGObj
{
public:
    GvsSolidIntersecObj ( GvsSolidObj *child0, GvsSolidObj *child1  );
    virtual ~GvsSolidIntersecObj (){}

    virtual GvsBoundBox boundingBox    ( void                          ) const;
    virtual bool        getRaySpanList ( GvsRay& ray, GvsSolObjSpanList& spanList );

    virtual void  Print ( FILE* fptr = stderr );
};
#endif

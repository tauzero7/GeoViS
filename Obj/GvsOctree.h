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
#ifndef GVS_OCTREE_H
#define GVS_OCTREE_H

#include <iostream>
#include <float.h>

#include "GvsGlobalDefs.h"
#include "GvsBoundBox.h"


class GvsOctree {

public:
    GvsOctree();

    void createSons(GvsOctree* t, int maxSubDivs);
    void createSons(int maxSubDivs);

    static long super(long id);
    static long lower(long id);

    GvsOctree*  findLeaf(GvsOctree *t, double x, double y, double z, int lod);
    GvsOctree*  findLeaf(double x, double y, double z, int lod);

    void Print ( FILE* fptr = stderr );

public:
    int  lod;
    long  id;
    GvsBoundBox box;
    GvsOctree* son[8];
};


#endif // GVS_OCTREE_H

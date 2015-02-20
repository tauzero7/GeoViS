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

#include "Ray/GvsRayVisual.h"


double GvsRayVisual :: rayContribThreshold = 0.01;
ushort GvsRayVisual :: rayMaxTreeDepth = 0;


static int getRayTreeDepth ( int rayIndex )
{
    if (rayIndex == 0)
        return 0;

    if (rayIndex < 0 )
        return -rayIndex;

    int rayTreeDepth = 1;

    while ( (rayIndex >>= 1) > 0 )
        rayTreeDepth += 1;

    return rayTreeDepth;
}

GvsRayVisual :: GvsRayVisual ( ) : GvsRayClosestIS () {
}

GvsRayVisual :: GvsRayVisual ( GvsRayGen* gen )
    : GvsRayClosestIS ( gen )
{
}

GvsRayVisual :: GvsRayVisual ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen, int index )
    : GvsRayClosestIS ( orig, dir, gen )
{
    rayIndex = index;
    rayTreeDepth = getRayTreeDepth ( index );
    rayMaxContrib = 1.0;
}

GvsRayVisual :: GvsRayVisual ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen,
                               double minSearchDist, double maxSearchDist, int index )
    : GvsRayClosestIS ( orig, dir, gen, minSearchDist, maxSearchDist )
{
    rayIndex = index;
    rayTreeDepth = getRayTreeDepth ( index );
    rayMaxContrib = 1.0;
}

GvsRayVisual :: GvsRayVisual ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad,
                               GvsRayGen* gen, int index )
    : GvsRayClosestIS ( orig, dir, tetrad, gen )
{
    rayIndex = index;
    rayTreeDepth = getRayTreeDepth ( index );
    rayMaxContrib = 1.0;
}

GvsRayVisual :: GvsRayVisual ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad,
                               GvsRayGen* gen, double minSearchDist, double maxSearchDist, int index )
    : GvsRayClosestIS ( orig, dir, tetrad, gen, minSearchDist, maxSearchDist )
{
    rayIndex = index;
    rayTreeDepth = getRayTreeDepth ( index );
    rayMaxContrib = 1.0;
}

GvsRayVisual :: GvsRayVisual ( m4d::vec4* geodPoints, m4d::vec4* geodTangents, int numPoints, int index )
    : GvsRayClosestIS ( )
{
    setPoints( geodPoints );
    setDirs( geodTangents );
    setNumPoints( numPoints );

    rayIndex = index;
    rayTreeDepth = getRayTreeDepth ( index );
    rayMaxContrib = 1.0;
    //cout << minSearchDist() << " " << maxSearchDist() << std::endl;
}

GvsRayVisual::~GvsRayVisual() {
}

m4d::vec3 GvsRayVisual :: getReflectedRayDir() {
    m4d::vec3 rayDirection = raySurfIntersec.getLocalDirection();
    //rayDirection.print(cerr);
    assert ( rayDirection!=m4d::vec3() );

    m4d::vec3 reflecRayDir = rayDirection - 2.0* (raySurfIntersec.normal() | rayDirection) * raySurfIntersec.normal();
    reflecRayDir.normalize();

    return reflecRayDir;
}


GvsRayVisual GvsRayVisual :: getReflectedRay() {
    m4d::vec3 reflecRayDir = getReflectedRayDir();

    GvsRayVisual reflecRay(raySurfIntersec.point(),reflecRayDir.get3As4(),getRayGen());

    reflecRay.rayTreeDepth = rayTreeDepth + 1;
    reflecRay.rayIndex = rayIndex + rayIndex + 1;

    return reflecRay;
}


GvsRayVisual GvsRayVisual :: getTransmittedRay ( double  ) {
    std::cerr << "GvsRayVisual :: getTransmittedRay() ... not implemented yet!\n";
    return GvsRayVisual();
}

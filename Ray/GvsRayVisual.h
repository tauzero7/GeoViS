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
#ifndef GVS_RAY_VISUAL_IS_H
#define GVS_RAY_VISUAL_IS_H

#include "GvsGlobalDefs.h"
#include "Ray/GvsRayClosestIS.h"

class GvsRayVisual : public GvsRayClosestIS
{
public:
    GvsRayVisual ( );
    GvsRayVisual ( GvsRayGen* gen );
    GvsRayVisual ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen, int index = 1);
    GvsRayVisual ( const m4d::vec4 &orig, const m4d::vec4 &dir, GvsRayGen* gen,
                   double minSearchDist, double maxSearchDist, int index = 1 );
    GvsRayVisual( m4d::vec4* geodPoints, m4d::vec4* geodTangents, int numPoints, int index = 1 );

    GvsRayVisual ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad,
                   GvsRayGen* gen, int index = 1);

    GvsRayVisual ( const m4d::vec4 &orig, const m4d::vec4 &dir, const GvsLocalTetrad *tetrad,
                   GvsRayGen* gen, double minSearchDist, double maxSearchDist, int index = 1);

    virtual ~GvsRayVisual();

    int           index             ( void ) const;
    ushort        treeDepth         ( void ) const;

    m4d::vec3     getReflectedRayDir ( void );
    GvsRayVisual  getReflectedRay   ( void );
    GvsRayVisual  getTransmittedRay ( double refracIndex );

    static void   setMaxRayTreeDepth     ( ushort maxDepth  );
    static void   setRayContribThreshold ( double threshold );
    static ushort maxRayTreeDepth        ( void  );
    static double contribThreshold       ( void  );

private:
    ushort rayTreeDepth;
    int    rayIndex;
    double rayMaxContrib;

    static double rayContribThreshold;
    static ushort rayMaxTreeDepth;

};


//----------------------------------------------------------------------------
//            i n l i n e
//----------------------------------------------------------------------------

inline int GvsRayVisual :: index ( void ) const
{
    return rayIndex;
}

inline ushort GvsRayVisual :: treeDepth ( void ) const
{
    return rayTreeDepth;
}

inline void GvsRayVisual :: setMaxRayTreeDepth ( ushort maxDepth  )
{
    rayMaxTreeDepth = maxDepth;
}

inline void GvsRayVisual :: setRayContribThreshold ( double threshold )
{
    rayContribThreshold = threshold;
}

inline ushort GvsRayVisual :: maxRayTreeDepth ()
{
    return rayMaxTreeDepth;
}

inline double GvsRayVisual :: contribThreshold ()
{
    return rayContribThreshold;
}

#endif

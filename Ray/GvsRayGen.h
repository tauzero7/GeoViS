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
#ifndef GVS_RAY_GEN_H
#define GVS_RAY_GEN_H

#include "GvsGlobalDefs.h"

#include "Obj/GvsBase.h"
#include "Obj/GvsBoundBox4D.h"
#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Utils/GvsGeodSolver.h"

#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>

/**
 * @brief The GvsRayGen class
 */
class GvsRayGen : public GvsBase
{
public:
    /**
     * C'tor
     */
    GvsRayGen();

    /**
     * Standard constructor to generate a backward directed light ray.
     * @param solver  pointer to geodesic solver
     */
    GvsRayGen( GvsGeodSolver *solver );

    /**
     * Standard constructor to generate a geodesic.
     * @param solver  pointer to geodesic solver
     * @param type    type of geodesic (timelike, lightlike, lightlike-sachs, spacelike)
     * @param dir     time direction of geodesic (forward or backward in time)
     */
    GvsRayGen( GvsGeodSolver *solver, m4d::enum_geodesic_type type, m4d::enum_time_direction dir );
    virtual ~GvsRayGen();

    void           setBoundBox ( const GvsBoundBox4D &box );
    GvsBoundBox4D  getBoundBox ( ) const;

    /**
     * Set maximum number of points to be integrated for the geodesic
     * @param maxPoints
     */
    void setMaxNumPoints ( const int maxPoints );

    /**
     * Get currently set maximum number of points to be integrated
     * @return
     */
    int  getMaxNumPoints ( ) const;

    void           setActualSolver ( GvsGeodSolver* solver );
    GvsGeodSolver* getActualSolver ( ) const;

    /**
     * Calculate simple light ray only.
     * @param startOrig   Initial position of the ray in coordinates.
     * @param startDir    Initial direction of the ray in coordinates.
     * @param numPoints   Reference to number of points calculated.
     * @param pointer to ray points (memory will be allocated inside)
     * @return break condition
     */
    m4d::enum_break_condition calcPolyline( const m4d::vec4 &startOrig, const m4d::vec4 &startDir,
                                            m4d::vec4*& points, int &numPoints);

    //! Calculate simple light ray and return also directions.
    m4d::enum_break_condition calcPolyline( const m4d::vec4 &startOrig, const m4d::vec4 &startDir,
                                            m4d::vec4*& points, m4d::vec4*& dirs, int &numPoints);

    //! Calculate light ray and parallel transported local tetrad.
    GvsLocalTetrad* calcParTransport( const m4d::vec4 &startOrig, const m4d::vec4 &startDir, const GvsLocalTetrad *lt,
                                      int &numPoints, m4d::enum_break_condition &bc );

    //! Calculate light ray, parallel transport the Sachs basis vectors, and integrate the Jacobi equation.
    bool calcSachsJacobi( const m4d::vec4 &startOrig, const m4d::vec4 &startDir, const m4d::vec3 &localDir,
                          const GvsLocalTetrad* lt,
                          m4d::vec4 *&points, m4d::vec4 *&dirs, double *&lambda,
                          m4d::vec4 *&sachs1, m4d::vec4 *&sachs2,
                          m4d::vec5 *&rayJacobi, m4d::vec5 &rayMaxJacobi,
                          int &numPoints, m4d::enum_break_condition &bc );

    void Print ( FILE* fptr = stderr );


protected:    
    GvsGeodSolver  *actualSolver;
    GvsBoundBox4D  boundBox;        //!< Bounding box for ray tracing

    int    maxNumPoints;    //!< Maximum number of points to calculate    
};

#endif

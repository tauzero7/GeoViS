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
#ifndef GVS_PROJECTOR_H
#define GVS_PROJECTOR_H

#include "GvsGlobalDefs.h"

#include "Obj/GvsBase.h"
#include "Cam/GvsCamera.h"
#include "Obj/STMotion/GvsStMotion.h"
#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Ray/GvsRayVisual.h"

/**
 * @brief The projector represents the observer within a scene given
 *        with respect to a local reference frame (LRF). This LRF can
 *        be either defined as simple local tetrad or as a motion.
 *
 */
class GvsProjector : public GvsBase
{
public:
    GvsProjector();
    GvsProjector( GvsRayGen* gen );
    GvsProjector( GvsRayGen* gen, GvsLocalTetrad* lT );
    virtual ~GvsProjector();

    void       setRayGen( GvsRayGen* gen );
    GvsRayGen* getRayGen() const;

    GvsColor  getBackgroundColor ( ) const;
    void      setBackgroundColor ( const GvsColor &backCol );
    
    //! If unknown error occured, use this color
    GvsColor  getErrorColor() const;
    void      setErrorColor( const GvsColor &col );

    //! If the constraint equation is violated, use this color
    GvsColor  getConstraintColor() const;
    void      setConstraintColor( const GvsColor &col );

    GvsColor  getBreakDownColor() const;
    void      setBreakDownColor( const GvsColor &col );

    //! Get the sample color for pixel (x,y)
    GvsColor  getSampleColor ( GvsDevice* device, double x, double y ) const;

    //! Get the sample color from the calculated light ray.
    GvsColor  getSampleColor ( GvsRayVisual*& eyeRay, GvsDevice* device  ) const;

    void            setLocalTetrad ( GvsLocalTetrad* lT );
    GvsLocalTetrad* getLocalTetrad ( );
    void            setTetrad      ( const m4d::vec4 &e0, const m4d::vec4 &e1, const m4d::vec4 &e2, const m4d::vec4 &e3, bool inCoord=false );

    void       setPosition( const m4d::vec4 &pos );
    m4d::vec4  getPosition() const;

    virtual void         setMotion( GvsStMotion* motion );
    virtual GvsStMotion* getMotion( ) const;

    virtual void setActualPos ( int nr );   //!< Select a position from the motion.

    virtual int SetParam ( const std::string pName, m4d::vec4 pt );
    virtual int SetParam ( const std::string pName, int nr );

    /** Get ray direction
     * The initial light ray direction is determined with respect to the camera model
     * and the local tetrad of the observer.
     * @param device   Pointer to the actual device.
     * @param x    Horizontal image pixel [0,resH-1]
     * @param y    Vertical image pixel [0,resV-1]
     * @param rayDirection   Initial direction of light ray given as four-vector in coordinate representation.
     * @param localRayDir    Initial direction of light ray with respect to local tetrad.
     */
    virtual void getRayDir( GvsDevice* device,
                            const double x, const double y,
                            m4d::vec4&   rayDirection,
                            m4d::vec3&   localRayDir ) const;

    virtual void Print ( FILE* fptr = stderr );

protected:
    GvsColor         backgroundColor;
    GvsColor         errorColor;
    GvsColor         constraintColor;  //!< If the constraint equation is violated then use this color
    GvsColor         breakDownColor;   //!< If the geodesic integration breaks down (black hole horizon)
    GvsRayGen*       rayGen;
    GvsLocalTetrad*  locTetrad;   //!< If the projector is static.
    GvsStMotion*     stMotion;    //!< If the projector is in motion.
};

#endif

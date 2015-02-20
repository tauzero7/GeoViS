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
#ifndef GVS_SCENE_OBJ_h
#define GVS_SCENE_OBJ_h

#include <iostream>

#include "Obj/GvsBoundBox.h"
#include "Obj/GvsTransformObj.h"
#include "Obj/STMotion/GvsStMotion.h"
#include "Ray/GvsRay.h"

#include <metric/m4dMetric.h>

class GvsLocalTetrad;

enum GvsObjType{
    // Object is given with respect to a local frame or is given in coordinate forumulation
    local=0,
    inCoords
};

/**
 * @brief The GvsSceneObj class
 */
class GvsSceneObj : public GvsTransformObj
{
public:
    GvsSceneObj();
    GvsSceneObj(GvsObjType oTyp);
    virtual ~GvsSceneObj();

    void          setObjType ( GvsObjType oTyp );
    GvsObjType    getObjType () const;

    void          setChart  ( int chart );
    int           getChart  () const;

    void          setMetric ( m4d::Metric* metric );  //???
    m4d::Metric*  getMetric () const;

    // Motion class
    virtual void         setMotion ( GvsStMotion *motion );
    virtual GvsStMotion* getMotion ( void ) const;

    virtual GvsBoundBox  boundingBox ( ) const = 0;

    virtual bool testIntersection      ( GvsRay &ray );
    virtual bool testLocalIntersection ( GvsRay &ray, const int seg,
                                         GvsLocalTetrad* lt0, GvsLocalTetrad* lt1,
                                         const m4d::vec4 p0, const m4d::vec4 p1 );

protected:
    GvsObjType    mObjType;         // Objekt wird in Koordinaten oder bzgl lokaler Tetrade beschrieben
    m4d::Metric*  mMetric;          // Pointer auf die komplette Raumzeit
    int           mChart;

    GvsStMotion*  stMotion;         // Bewegung des Objekts
    bool          haveMotion;
    int           numPositions;
};

#endif

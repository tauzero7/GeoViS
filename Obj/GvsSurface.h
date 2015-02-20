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
#ifndef GVS_SURFACE_H
#define GVS_SURFACE_H

#include "GvsGlobalDefs.h"
#include "Obj/GvsSceneObj.h"

class GvsSurfIntersec;
class GvsSurfaceShader;
class GvsShader;
class GvsBoundBox;

/**
 * @brief The GvsSurface class
 */
class GvsSurface : public GvsSceneObj
{
public:
    GvsSurface();
    GvsSurface(GvsSurfaceShader *shader);
    virtual ~GvsSurface();

    void                setShader ( GvsSurfaceShader* shader );
    GvsSurfaceShader*   shader ( ) const;
    virtual GvsShader*  shader ( const GvsSurfIntersec & intersec ) const;

    virtual void  calcNormal      ( GvsSurfIntersec &  intersec ) const = 0;
    virtual void  calcTexUVParam  ( GvsSurfIntersec &  intersec ) const;
    virtual void  calcDerivatives ( GvsSurfIntersec &  intersec ) const;

    virtual void  Print ( FILE *fptr = stderr );

protected:
    GvsSurfaceShader *surfShader;

};

#endif

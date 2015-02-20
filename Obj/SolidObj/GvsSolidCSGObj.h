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
#ifndef GVS_SOLID_CSG_OBJ_H
#define GVS_SOLID_CSG_OBJ_H

#include <Obj/SolidObj/GvsSolidObj.h>
#include <Ray/GvsRay.h>
#include <Ray/GvsSurfIntersec.h>

#include "m4dGlobalDefs.h"

class GvsRay;
class GvsSurfIntersec;
class GvsSurfaceShader;
class GvsStMotion;
class GvsSolObjSpanList;


enum GvsCSGType{CSG_Obj = 0, CSG_differObj, CSG_unifiedObj, CSG_intersecObj};


class GvsSolidCSGObj : public GvsSolidObj
{
public:
    GvsSolidCSGObj( GvsSolidObj *child01,
                    GvsSolidObj *child02 );
    virtual ~GvsSolidCSGObj(){}

    virtual bool testIntersection ( GvsRay&      ray      );

    virtual void scale            ( const m4d::vec3&   scaleVec );
    virtual void translate        ( const m4d::vec3&   transVec );
    virtual void rotate           ( const m4d::vec3&   rotAxis, double rotAngle  );

    virtual void transform        ( const m4d::Matrix<double,3,4>& mat     );

    virtual void calcNormal       ( GvsSurfIntersec & ) const;
    virtual void calcTexUVParam   ( GvsSurfIntersec & ) const;
    virtual void calcDerivatives  ( GvsSurfIntersec & ) const;
    virtual void calcTex3dPoint   ( GvsSurfIntersec & ) const;

    virtual bool isValidObjIntersec ();

    virtual int SetParam ( std::string pName, m4d::Matrix<double,3,4> mat );

    virtual void Print( FILE* fptr = stderr );

    virtual GvsCSGType getCSGType () const;
    virtual void setCSGType (GvsCSGType csgType);

protected:
    GvsSolidObj* childSolObj[2];
    GvsCSGType   mCSGType;
    bool haveObjIntersec;
};



#endif

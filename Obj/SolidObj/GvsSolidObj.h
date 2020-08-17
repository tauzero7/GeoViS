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
#ifndef GVS_SOLID_OBJ_h
#define GVS_SOLID_OBJ_h

#include <Obj/GvsSurface.h>
#include <Ray/GvsRay.h>
#include <iostream>

class GvsLocalTetrad;
class GvsSolObjSpanList;

class GvsSolidObj : public GvsSurface
{
public:
    GvsSolidObj(GvsSurfaceShader* shader);
    virtual ~GvsSolidObj();

    virtual bool testIntersection(GvsRay& ray);

    virtual bool testLocalIntersection(
        GvsRay& ray, const int seg, GvsLocalTetrad* lt0, GvsLocalTetrad* lt1, const m4d::vec4 p0, const m4d::vec4 p1);

    virtual bool getRaySpanList(GvsRay& ray, GvsSolObjSpanList& isl);

    virtual int SetParam(std::string pName, m4d::Matrix<double, 3, 4> mat);

    virtual bool haveSetParamTransfMat() const;

protected:
    m4d::Matrix<double, 3, 4> volTransfMat;
    m4d::Matrix<double, 3, 4> volInvTransfMat;

    m4d::Matrix<double, 3, 4> volParamTransfMat;
    m4d::Matrix<double, 3, 4> volParamInvTransfMat;
    bool mHaveSetParamTransfMat;
};

#endif

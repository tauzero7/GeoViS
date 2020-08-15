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
#ifndef GVS_TRIANGLE_H
#define GVS_TRIANGLE_H

#include "Obj/PlanarObj/GvsPlanarSurf.h"

class GvsRay;
class GvsRayAllIS;
class GvsStMotion;

class GvsTriangle : public GvsPlanarSurf
{
public:
    GvsTriangle(void);
    GvsTriangle(const m4d::vec3& p0, const m4d::vec3& p1, const m4d::vec3& p2, GvsSurfaceShader* shader,
        m4d::Metric* metric, GvsObjType objType = local);

    GvsTriangle(const m4d::vec3& p0, const m4d::vec3& p1, const m4d::vec3& p2, GvsSurfaceShader* shader,
        m4d::Metric* metric, GvsStMotion* motion, GvsObjType objType = local);

    GvsTriangle(const m4d::vec3& p0, const m4d::vec3& p1, const m4d::vec3& p2, const m4d::vec2& uv0,
        const m4d::vec2& uv1, const m4d::vec2& uv2, GvsSurfaceShader* shader, m4d::Metric* metric,
        GvsObjType objType = local);

    GvsTriangle(const m4d::vec3& p0, const m4d::vec3& p1, const m4d::vec3& p2, const m4d::vec2& uv0,
        const m4d::vec2& uv1, const m4d::vec2& uv2, GvsSurfaceShader* shader, m4d::Metric* metric, GvsStMotion* motion,
        GvsObjType objType = local);

    GvsTriangle(const GvsTriangle& triangle);

    ~GvsTriangle(void);

    virtual GvsPlanarSurf* getClone(void) const;

    virtual bool isValidHit(m4d::vec3 rp);

    virtual void calcNormal(GvsSurfIntersec& intersec) const;
    virtual void calcTexUVParam(GvsSurfIntersec& intersec) const;

    virtual void scale(const m4d::vec3& scaleVec);
    virtual void translate(const m4d::vec3& transVec);
    virtual void rotate(const m4d::vec3& rotAxis, double rotAngle);
    virtual void transform(const m4d::Matrix<double, 3, 4>& mat);

    virtual void Print(FILE* fptr = stderr);

protected:
    m4d::vec3 calcTriNormal();

    virtual void calcProjEdges(int projPlane, const m4d::vec3& intersecPt, m4d::vec2& projEdge20, m4d::vec2& projEdge21,
        m4d::vec2& projEdge2p) const;

    virtual void calcBoundBox(void);

protected:
    m4d::vec3 trianVertex[3];
    m4d::vec2 trianUV[3];
    double trianArea;
};

#endif

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
#ifndef GVS_PLANAR_SURF_H
#define GVS_PLANAR_SURF_H

#include <Obj/GvsSurface.h>

class GvsRay;
class GvsRayAllIS;

class GvsPlanarSurf : public GvsSurface
{
public:
    GvsPlanarSurf(GvsSurfaceShader* shader);
    GvsPlanarSurf(const GvsPlanarSurf& surf);
    ~GvsPlanarSurf(void);

    virtual GvsPlanarSurf* getClone(void) const = 0;

    virtual m4d::vec3 normal() const;

    virtual bool testIntersection(GvsRay& ray);

    virtual bool allIntersections(GvsRayAllIS& ray);

    virtual bool testLocalIntersection(
        GvsRay& ray, const int rayPartIndex, const int seg, const m4d::vec4 p0, const m4d::vec4 p1);

    virtual bool testLocalIntersection(GvsRay& ray, const int rayPartIndex, const int seg, const m4d::vec4 p0,
        const m4d::vec4 p1, GvsSurface* parentSurf);

    virtual bool allLocalIntersections(
        GvsRayAllIS& ray, const int rayPartIndex, const int seg, const m4d::vec4 p0, const m4d::vec4 p1);

    virtual GvsBoundBox boundingBox(void) const;

    virtual bool isValidHit(m4d::vec3 rp);

    virtual void scale(const m4d::vec3& scaleVec);
    virtual void translate(const m4d::vec3& transVec);
    virtual void rotate(const m4d::vec3& rotAxis, double rotAngle);
    virtual void transform(const m4d::Matrix<double, 3, 4>& mat);

    virtual int SetParam(std::string pName, m4d::Matrix<double, 3, 4> mat);

protected:
    virtual void calcBoundBox(void) = 0;

    bool rayIntersect(const m4d::vec3& p0, const m4d::vec3& p1, double tp0, double tp1, double& alpha, double& thit,
        m4d::vec3& rayIntersecPnt) const;

protected:
    GvsBoundBox planarSurfBoundBox;
    m4d::vec3 planeNormal;
    double planeDist;

    m4d::Matrix<double, 3, 4> volTransfMat;
    m4d::Matrix<double, 3, 4> volInvTransfMat;

    m4d::Matrix<double, 3, 4> volParamTransfMat;
    m4d::Matrix<double, 3, 4> volParamInvTransfMat;
    bool mHaveSetParamTransfMat;
};

#endif

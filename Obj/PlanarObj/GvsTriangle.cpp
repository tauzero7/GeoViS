/**
 * @file    GvsTriangle.cpp
 * @author  Thomas Mueller
 *
 *  This file is part of GeoViS.
 */
#include "Obj/PlanarObj/GvsTriangle.h"

#include <Obj/STMotion/GvsStMotion.h>
#include <Ray/GvsRay.h>
#include <Ray/GvsRayAllIS.h>

#include <cassert>

#include "math/TransfMat.h"

double area(const m4d::vec2& v1, const m4d::vec2& v2)
{
    return 0.5 * (v1.x(0) * v2.x(1) - v1.x(1) * v2.x(0));
}

double area(const m4d::vec3& v1, const m4d::vec3& v2)
{
    return 0.5 * (v1 ^ v2).getNorm();
}

GvsTriangle::GvsTriangle(void)
    : GvsPlanarSurf(nullptr)
{
    // STMotion ??
    triangleVertex[0] = m4d::vec3(0.0, 0.0, 0.0);
    triangleVertex[1] = m4d::vec3(1.0, 0.0, 0.0);
    triangleVertex[2] = m4d::vec3(0.0, 1.0, 0.0);

    triangleUV[0] = m4d::vec2(0.0, 0.0);
    triangleUV[1] = m4d::vec2(1.0, 0.0);
    triangleUV[2] = m4d::vec2(0.0, 1.0);

    planeNormal = m4d::vec3(0.0, 0.0, 1.0);

    mMetric = nullptr;
    stMotion = nullptr;
    mObjType = inCoords;
    calcBoundBox();
    AddParam("transform", gvsDT_MAT3D);
}

GvsTriangle::GvsTriangle(const m4d::vec3& p0, const m4d::vec3& p1, const m4d::vec3& p2, GvsSurfaceShader* shader,
    m4d::Metric* metric, GvsObjType objType)
    : GvsPlanarSurf(shader)
{
    triangleVertex[0] = p0;
    triangleVertex[1] = p1;
    triangleVertex[2] = p2;

    planeNormal = calcTriNormal();

    triangleUV[0] = m4d::vec2(0.0, 0.0);
    triangleUV[1] = m4d::vec2(1.0, 0.0);
    triangleUV[2] = m4d::vec2(0.0, 1.0);

    mMetric = metric;
    stMotion = nullptr;
    mObjType = objType;
    calcBoundBox();
    AddParam("transform", gvsDT_MAT3D);
}

GvsTriangle::GvsTriangle(const m4d::vec3& p0, const m4d::vec3& p1, const m4d::vec3& p2, GvsSurfaceShader* shader,
    m4d::Metric* metric, GvsStMotion* motion, GvsObjType objType)
    : GvsPlanarSurf(shader)
{
    triangleVertex[0] = p0;
    triangleVertex[1] = p1;
    triangleVertex[2] = p2;

    planeNormal = calcTriNormal();

    triangleUV[0] = m4d::vec2(0.0, 0.0);
    triangleUV[1] = m4d::vec2(1.0, 0.0);
    triangleUV[2] = m4d::vec2(0.0, 1.0);

    mMetric = metric;
    stMotion = motion;
    mObjType = objType;
    calcBoundBox();
    AddParam("transform", gvsDT_MAT3D);
}

GvsTriangle::GvsTriangle(const m4d::vec3& p0, const m4d::vec3& p1, const m4d::vec3& p2, const m4d::vec2& uv0,
    const m4d::vec2& uv1, const m4d::vec2& uv2, GvsSurfaceShader* shader, m4d::Metric* metric, GvsObjType objType)
    : GvsPlanarSurf(shader)
{
    triangleVertex[0] = p0;
    triangleVertex[1] = p1;
    triangleVertex[2] = p2;

    planeNormal = calcTriNormal();

    triangleUV[0] = uv0;
    triangleUV[1] = uv1;
    triangleUV[2] = uv2;

    mMetric = metric;
    stMotion = nullptr;
    mObjType = objType;
    calcBoundBox();
    AddParam("transform", gvsDT_MAT3D);
}

GvsTriangle::GvsTriangle(const m4d::vec3& p0, const m4d::vec3& p1, const m4d::vec3& p2, const m4d::vec2& uv0,
    const m4d::vec2& uv1, const m4d::vec2& uv2, GvsSurfaceShader* shader, m4d::Metric* metric, GvsStMotion* motion,
    GvsObjType objType)
    : GvsPlanarSurf(shader)
{
    triangleVertex[0] = p0;
    triangleVertex[1] = p1;
    triangleVertex[2] = p2;

    planeNormal = calcTriNormal();

    triangleUV[0] = uv0;
    triangleUV[1] = uv1;
    triangleUV[2] = uv2;

    mMetric = metric;
    stMotion = motion;
    mObjType = objType;
    AddParam("transform", gvsDT_MAT3D);
}

GvsTriangle::GvsTriangle(const GvsTriangle& triangle)
    : GvsPlanarSurf(triangle)
{
    triangleVertex[0] = triangle.triangleVertex[0];
    triangleVertex[1] = triangle.triangleVertex[1];
    triangleVertex[2] = triangle.triangleVertex[2];

    planeNormal = triangle.planeNormal;

    triangleUV[0] = triangle.triangleUV[0];
    triangleUV[1] = triangle.triangleUV[1];
    triangleUV[2] = triangle.triangleUV[2];

    mMetric = triangle.mMetric;
    stMotion = triangle.stMotion;
    mObjType = triangle.mObjType;
    calcBoundBox();
    AddParam("transform", gvsDT_MAT3D);
}

GvsTriangle::~GvsTriangle(void) {}

GvsPlanarSurf* GvsTriangle::getClone(void) const
{
    GvsTriangle* clone = new GvsTriangle(*this);
    assert(clone != nullptr);
    return clone;
}

bool GvsTriangle::isValidHit(m4d::vec3 rp)
{
    m4d::vec3 vq1 = triangleVertex[0] - rp;
    m4d::vec3 vq2 = triangleVertex[1] - rp;
    m4d::vec3 vq3 = triangleVertex[2] - rp;

    double Area12 = area(vq1, vq2);
    double Area13 = area(vq1, vq3);
    double Area23 = area(vq2, vq3);
    // fprintf(stderr, " is: %f %f\n", Area12 + Area13 + Area23, triangleArea);

    return (Area12 + Area13 + Area23 <= 1.0 * triangleArea + GVS_EPS);
}

m4d::vec3 GvsTriangle::calcTriNormal()
{
    m4d::vec3 p1 = triangleVertex[0];
    m4d::vec3 p2 = triangleVertex[1];
    m4d::vec3 p3 = triangleVertex[2];
    m4d::vec3 n = (p2 - p1) ^ (p3 - p1);
    triangleArea = 0.5 * n.getNorm();
    n = n.getNormalized();
    planeDist = p1 | n;
    return n;
}

void GvsTriangle::calcNormal(GvsSurfIntersec& intersec) const
{
    intersec.setNormal(planeNormal);
}

void GvsTriangle::calcTexUVParam(GvsSurfIntersec& intersec) const
{
    m4d::vec3 t0 = triangleVertex[0];
    m4d::vec3 t1 = triangleVertex[1];
    m4d::vec3 t2 = triangleVertex[2];

    m4d::vec3 ip = intersec.localPoint(); //.getAsV3D();
    m4d::vec3 e0 = t1 - t0;
    m4d::vec3 e1 = t2 - t0;
    m4d::vec3 e2 = ip - t0;

    double d00 = e0 | e0;
    double d01 = e0 | e1;
    double d11 = e1 | e1;
    double d20 = e2 | e0;
    double d21 = e2 | e1;

    double denom = 1.0 / (d00 * d11 - d01 * d01);
    double u = (d11 * d20 - d01 * d21) * denom;
    double v = (d00 * d21 - d01 * d20) * denom;

    double w = 1.0 - u - v;
    intersec.setTexUVParam(w * triangleUV[0].x(0) + v * triangleUV[1].x(0) + u * triangleUV[2].x(0),
        w * triangleUV[0].x(1) + v * triangleUV[1].x(1) + u * triangleUV[2].x(1));
    // intersec.setTexUVParam(u, v);
}

void GvsTriangle::scale(const m4d::vec3& scaleVec)
{
    for (int i = 0; i < 3; i++) {
        triangleVertex[i] *= scaleVec;
    }

    planeNormal = calcTriNormal();
    calcBoundBox();
}

void GvsTriangle::translate(const m4d::vec3& transVec)
{
    for (int i = 0; i < 3; i++) {
        triangleVertex[i] += transVec;
    }
    calcBoundBox();
}

void GvsTriangle::rotate(const m4d::vec3& rotAxis, const double rotAngle)
{
    m4d::RotateMat3D rotMat = m4d::RotateMat3D(rotAxis, rotAngle);
    for (int i = 0; i < 3; i++) {
        triangleVertex[i] = rotMat * triangleVertex[i];
    }
    planeNormal = rotMat * planeNormal;
}

void GvsTriangle::transform(const m4d::Matrix<double, 3, 4>& mat)
{
    for (int i = 0; i < 3; i++) {
        triangleVertex[i] = mat * triangleVertex[i];
    }

    // planeNormal = rotMat * planeNormal;
    calcBoundBox();
}

void GvsTriangle::Print(FILE* fptr)
{
    fprintf(stderr, "Triangle {\n");
    fprintf(stderr, "\tv1: ");
    triangleVertex[0].printS(fptr);
    fprintf(stderr, "\tv2: ");
    triangleVertex[1].printS(fptr);
    fprintf(stderr, "\tv3: ");
    triangleVertex[2].printS(fptr);
    fprintf(stderr, "}\n");
}

void GvsTriangle::calcProjEdges(int projPlane, const m4d::vec3& intersecPt, m4d::vec2& projEdge20,
    m4d::vec2& projEdge21, m4d::vec2& projEdge2p) const
{
    switch (projPlane) {
        case 0:
            projEdge20[0] = triangleVertex[0].x(1) - triangleVertex[2].x(1);
            projEdge20[1] = triangleVertex[0].x(2) - triangleVertex[2].x(2);
            projEdge21[0] = triangleVertex[1].x(1) - triangleVertex[2].x(1);
            projEdge21[1] = triangleVertex[1].x(2) - triangleVertex[2].x(2);
            projEdge2p[0] = intersecPt.x(1) - triangleVertex[2].x(1);
            projEdge2p[1] = intersecPt.x(2) - triangleVertex[2].x(2);
            break;
        case 1:
            projEdge20[0] = triangleVertex[0].x(0) - triangleVertex[2].x(0);
            projEdge20[1] = triangleVertex[0].x(2) - triangleVertex[2].x(2);
            projEdge21[0] = triangleVertex[1].x(0) - triangleVertex[2].x(0);
            projEdge21[1] = triangleVertex[1].x(2) - triangleVertex[2].x(2);
            projEdge2p[0] = intersecPt.x(0) - triangleVertex[2].x(0);
            projEdge2p[1] = intersecPt.x(2) - triangleVertex[2].x(2);
            break;
        case 2:
            projEdge20[0] = triangleVertex[0].x(0) - triangleVertex[2].x(0);
            projEdge20[1] = triangleVertex[0].x(1) - triangleVertex[2].x(1);
            projEdge21[0] = triangleVertex[1].x(0) - triangleVertex[2].x(0);
            projEdge21[1] = triangleVertex[1].x(1) - triangleVertex[2].x(1);
            projEdge2p[0] = intersecPt.x(0) - triangleVertex[2].x(0);
            projEdge2p[1] = intersecPt.x(1) - triangleVertex[2].x(1);
            break;
    }
}

void GvsTriangle::calcBoundBox(void)
{
    planarSurfBoundBox = GvsBoundBox(triangleVertex[0], triangleVertex[1]);
    planarSurfBoundBox.extendBoxToContain(triangleVertex[2]);
}

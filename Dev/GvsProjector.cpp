/**
 * @file    GvsProjector.cpp
 * @author  Thomas Mueller
 *
 *  This file is part of GeoViS.
 */
#include <iostream>

#include "Dev/GvsDevice.h"
#include "Dev/GvsProjector.h"
#include "Obj/GvsSceneObj.h"
#include "Ray/GvsRayGen.h"
#include "Ray/GvsSurfIntersec.h"
#include "Shader/GvsShader.h"
#include "Utils/GvsGramSchmidt.h"

#include <metric/m4dMetric.h>

#include "Utils/GvsLog.h"
extern GvsLog& LOG;

GvsProjector ::GvsProjector()
    : GvsBase()
    , rayGen(nullptr)
    , locTetrad(nullptr)
    , stMotion(nullptr)
{

    GvsBase::AddParam("position", gvsDT_VEC4);
    GvsBase::AddParam("actualpos", gvsDT_INT);
    errorColor = GvsColor(0.0);
    constraintColor = GvsColor(0.0);
    breakDownColor = GvsColor(0.0);
}

GvsProjector ::GvsProjector(GvsRayGen* gen)
    : rayGen(gen)
    , locTetrad(nullptr)
    , stMotion(nullptr)
{

    GvsBase::AddParam("position", gvsDT_VEC4);
    GvsBase::AddParam("actualpos", gvsDT_INT);
    errorColor = GvsColor(0.0);
    constraintColor = GvsColor(0.0);
    breakDownColor = GvsColor(0.0);
}

GvsProjector::~GvsProjector()
{
    // nothing to delete
}

void GvsProjector ::setRayGen(GvsRayGen* gen)
{
    rayGen = gen;
}

GvsRayGen* GvsProjector ::getRayGen() const
{
    return rayGen;
}

GvsColor GvsProjector ::getBackgroundColor() const
{
    return backgroundColor;
}

void GvsProjector ::setBackgroundColor(const GvsColor& backCol)
{
    backgroundColor = backCol;
}

GvsColor GvsProjector::getErrorColor() const
{
    return errorColor;
}

void GvsProjector::setErrorColor(const GvsColor& col)
{
    errorColor = col;
}

GvsColor GvsProjector::getConstraintColor() const
{
    return constraintColor;
}

void GvsProjector::setConstraintColor(const GvsColor& col)
{
    constraintColor = col;
}

GvsColor GvsProjector::getBreakDownColor() const
{
    return breakDownColor;
}

void GvsProjector::setBreakDownColor(const GvsColor& col)
{
    breakDownColor = col;
}

void GvsProjector ::setLocalTetrad(GvsLocalTetrad* lT)
{
    locTetrad = lT;
    locTetrad->transformTetrad(true);
}

GvsLocalTetrad* GvsProjector::getLocalTetrad()
{
    return locTetrad;
}

void GvsProjector ::setPosition(const m4d::vec4& pos)
{
    assert(locTetrad != NULL);
    locTetrad->setPosition(pos);
    locTetrad->adjustTetrad();
    GvsBase::SetParam("position", pos);
}

m4d::vec4 GvsProjector ::getPosition() const
{
    assert(locTetrad != NULL);
    return locTetrad->getPosition();
}

void GvsProjector ::setMotion(GvsStMotion* motion)
{
    assert(motion != NULL);
    stMotion = motion;
    locTetrad = stMotion->getLocalTetrad(0);
}

GvsStMotion* GvsProjector ::getMotion() const
{
    return stMotion;
}

void GvsProjector::getSampleColor(GvsDevice* device, double x, double y, GvsColor& col, gvsData& data) const
{
    assert((rayGen != NULL) && (locTetrad != NULL));

    m4d::vec4 rayOrigin = locTetrad->getPosition();
    if (device->camEye == gvsCamEyeLeft) {
        m4d::vec3 leftEyePos = device->camera->GetLeftEyePos();
        m4d::vec4 e0, e1, e2, e3;
        locTetrad->getTetrad(e0, e1, e2, e3);
        rayOrigin += leftEyePos.x(0) * e1 + leftEyePos.x(1) * e2 + leftEyePos.x(2) * e3;
    }
    else if (device->camEye == gvsCamEyeRight) {
        m4d::vec3 rightEyePos = device->camera->GetRightEyePos();
        m4d::vec4 e0, e1, e2, e3;
        locTetrad->getTetrad(e0, e1, e2, e3);
        rayOrigin += rightEyePos.x(0) * e1 + rightEyePos.x(1) * e2 + rightEyePos.x(2) * e3;
    }

    // rayOrigin and rayDir in coordinates
    m4d::vec4 rayDir;
    m4d::vec3 localRayDir;
    getRayDir(device, x, y, rayDir, localRayDir);

    //    LOG.printf(5,"Pixel: %4d %4d ************************\n",static_cast<int>(x),static_cast<int>(y));

    GvsRayVisual* eyeRay = new GvsRayVisual(rayGen);

    GvsCamFilter camFilter = device->camera->getCamFilter();
    col = errorColor;

    if (!rayDir.getAsV3D().isZero()) {
        bool validRay = false;
        switch (camFilter) {
            case gvsCamFilterRGBpt:
            case gvsCamFilterRGBIntersec:
            case gvsCamFilterRGB: {
                validRay = eyeRay->recalc(rayOrigin, rayDir);
                break;
            }
            case gvsCamFilterRGBpdz: {
                validRay = eyeRay->recalc(rayOrigin, rayDir, locTetrad);
                break;
            }
            case gvsCamFilterRGBjac: {
                validRay = eyeRay->recalcJacobi(rayOrigin, rayDir, localRayDir, locTetrad);
                break;
            }
        }

        if (camFilter == gvsCamFilterRGB || camFilter == gvsCamFilterRGBpdz || camFilter == gvsCamFilterRGBjac
            || camFilter == gvsCamFilterRGBpt || camFilter == gvsCamFilterRGBIntersec) {
            if (validRay) {
                col = getSampleColor(eyeRay, device);
                if (camFilter == gvsCamFilterRGBIntersec) {
                    data = getSampleIntersection(eyeRay, device);
                }
            }
        }
    }
    else {
        col.setValid(false);
    }
    delete eyeRay;
}

GvsColor GvsProjector::getSampleColor(GvsRayVisual*& eyeRay, GvsDevice* device) const
{

    m4d::vec4 lightDirStart, locLightDirStart, locLightDirEnd, lightDirEnd;
    double i, frak, wObs, wSrc;
    m4d::vec5 jacobi;

    GvsCamFilter camFilter = device->camera->getCamFilter();
    GvsColor sampleColor = getBackgroundColor();

    bool intersecFound = false;
    if ((device->sceneGraph != NULL) && (device->sceneGraph->testIntersection(*eyeRay))) {
        GvsShader* shader = eyeRay->intersecShader();
        if (shader != NULL) {
            sampleColor = shader->getIncidentLight(device, *eyeRay);
            // sampleColor.Print();

            // Direction of the light ray at the intersection points.
            GvsSurfIntersec* surfIntersec = eyeRay->getSurfIntersec();
            if (LOG.level() == 5) {
                surfIntersec->print(LOG.ptr());
                surfIntersec->surface()->Print(LOG.ptr());
            }

            if ((camFilter == gvsCamFilterRGBpdz) || (camFilter == gvsCamFilterRGBjac)) {

                // initial ray direction
                lightDirStart = eyeRay->getTangente(0);

                // Light direction wrt local tetrad. The initial frequency w1
                // follows from the scalar product of k=-w1(e0+n^ie_i) with e0.
                locTetrad->transformTetrad(true);
                locLightDirStart = locTetrad->coordToLocal(-lightDirStart);
                wObs = locLightDirStart.x(0);

                // Calculate the mean of the directions:
                frak = modf(surfIntersec->dist(), &i);
                lightDirEnd = (1.0 - frak) * eyeRay->getTangente(surfIntersec->getRaySegNumber())
                    + frak * eyeRay->getTangente(surfIntersec->getRaySegNumber() + 1);

                // reverse light dir that it represents an outgoint light ray
                lightDirEnd = -lightDirEnd;

                GvsObjType objType = surfIntersec->object()->getObjType();
                GvsLocalTetrad* lt;
                if (objType == inCoords) {
                    lt = new GvsLocalTetrad(device->metric, surfIntersec->point(), m4d::vec4(1, 0, 0, 0));
                    // surfIntersec->point().printS();
                    lt->transformTetrad(true, m4d::enum_nat_tetrad_default);
                    locLightDirEnd = lt->coordToLocal(lightDirEnd);
                    delete lt;
                }
                else {
                    lt = surfIntersec->getLocalTetrad();
                    locLightDirEnd = lt->coordToLocal(lightDirEnd);
                }
                wSrc = locLightDirEnd.x(0);
                // std::cerr << wObs << " " << wSrc << " " << wSrc/wObs << std::endl;

                sampleColor.data.freqshift = wSrc / wObs;
                memcpy(sampleColor.data.pos, eyeRay->surfIntersec().point().data(), sizeof(double) * 4);
                memcpy(sampleColor.data.dir, lightDirEnd.data(), sizeof(double) * 4);
                if (camFilter == gvsCamFilterRGBjac) {
                    jacobi = (1.0 - frak) * eyeRay->getJacobi(surfIntersec->getRaySegNumber())
                        + frak * eyeRay->getJacobi(surfIntersec->getRaySegNumber() + 1);

                    memcpy(sampleColor.data.jacobi, jacobi.data(), sizeof(double) * 5);
                }

                if (surfIntersec->surface() != NULL) {
                    sampleColor.data.objID = static_cast<double>(surfIntersec->surface()->GetID());
                }
            }
            else if (camFilter == gvsCamFilterRGBpt) {
                memcpy(sampleColor.data.pos, eyeRay->surfIntersec().point().data(), sizeof(double) * 4);
                memcpy(sampleColor.data.uv, eyeRay->surfIntersec().texUVParam().data(), sizeof(double) * 2);
                if (surfIntersec->surface() != NULL) {
                    sampleColor.data.objID = static_cast<double>(surfIntersec->surface()->GetID());
                }
            }
            sampleColor = sampleColor.trim();
            intersecFound = true;
        }
    }

    if (!intersecFound
        && (eyeRay->getBreakCond() == m4d::enum_break_constraint || eyeRay->getBreakCond() == m4d::enum_break_cond)) {
        sampleColor = constraintColor;
    }
    return sampleColor;
}

void GvsProjector::getSampleIntersection(GvsDevice* device, double x, double y)
{
#if 0
    assert ( (rayGen != NULL) && (locTetrad != NULL) );

    m4d::vec4 rayOrigin = locTetrad->getPosition();

    if (device->camEye == gvsCamEyeLeft) {
        m4d::vec3 leftEyePos = device->camera->GetLeftEyePos();
        m4d::vec4 e0,e1,e2,e3;
        locTetrad->getTetrad(e0,e1,e2,e3);
        rayOrigin += leftEyePos.x(0)*e1 + leftEyePos.x(1)*e2 + leftEyePos.x(2)*e3;
    } else if (device->camEye == gvsCamEyeRight) {
        m4d::vec3 rightEyePos = device->camera->GetRightEyePos();
        m4d::vec4 e0,e1,e2,e3;
        locTetrad->getTetrad(e0,e1,e2,e3);
        rayOrigin += rightEyePos.x(0)*e1 + rightEyePos.x(1)*e2 + rightEyePos.x(2)*e3;
    }

    // rayOrigin and rayDir in coordinates
    m4d::vec4 rayDir;
    m4d::vec3 localRayDir;
    getRayDir ( device, x, y, rayDir, localRayDir );

    GvsRayAllIS* eyeRay = new GvsRayAllIS(rayGen);

    if (!rayDir.getAsV3D().isZero()) {
        bool validRay = eyeRay->recalc(rayOrigin, rayDir);
        if (validRay) {
             getSampleIntersection(eyeRay, device);
        }
    } else {
        //col.setValid(false);
    }
    delete eyeRay;
    //TODO: return col;
#endif
}

/*
void GvsProjector::getSampleIntersection(GvsRayAllIS*& eyeRay, GvsDevice* device) {

//    m4d::vec4 lightDirStart, locLightDirStart, locLightDirEnd, lightDirEnd;
//    double i,frak, wObs, wSrc;
//    m4d::vec5 jacobi;

return;

    if ( (device->sceneGraph != NULL) &&
         (device->sceneGraph->testIntersection( *eyeRay )) )
    {
        GvsSurfIntersec* surfIntersec = eyeRay->getFirstSurfIntersec();
        while((surfIntersec = eyeRay->getNextSurfIntersec()) != NULL) {

        }
       // TODO
    }

    // TODO
}
*/

gvsData GvsProjector::getSampleIntersection(GvsRayVisual*& eyeRay, GvsDevice* device) const
{
    GvsSurfIntersec* surfIntersec = eyeRay->getSurfIntersec();
    gvsData data;
    if (surfIntersec != NULL) {
        m4d::vec4 ip = surfIntersec->point();
        // ip.printF();
        data.pos[0] = ip[0];
        data.pos[1] = ip[1];
        data.pos[2] = ip[2];
        data.pos[3] = ip[3];

        m4d::vec2 texuv = surfIntersec->texUVParam();
        data.uv[0] = texuv.x(0);
        data.uv[1] = texuv.x(1);
    }
    return data;
}

void GvsProjector ::setActualPos(int nr)
{
    if (stMotion == NULL) {
        fprintf(stderr, "GvsProjector::setActualPos... No motion available!\n");
        return;
    }
    if (nr >= 0 && nr < stMotion->getNumPositions()) {
        locTetrad = stMotion->getLocalTetrad(nr);
        // locTetrad->Print();
    }
}

int GvsProjector::SetParam(const std::string pName, m4d::vec4 pt)
{
    int isOkay = GvsBase::SetParam(pName, pt);
    if (isOkay >= gvsSetParamNone && pName == "position") {
        setPosition(pt);
    }
    return isOkay;
}

int GvsProjector::SetParam(const std::string pName, int nr)
{
    int isOkay = GvsBase::SetParam(pName, nr);
    if (isOkay >= gvsSetParamNone && pName == "actualpos") {
        setActualPos(nr);
    }
    return isOkay;
}

void GvsProjector::setTetrad(
    const m4d::vec4& e0, const m4d::vec4& e1, const m4d::vec4& e2, const m4d::vec4& e3, bool inCoord)
{
    locTetrad->setTetrad(e0, e1, e2, e3, false);
    locTetrad->setInCoords(inCoord);
}

void GvsProjector::getRayDir(
    GvsDevice* device, const double x, const double y, m4d::vec4& rayDirection, m4d::vec3& localRayDir) const
{

    rayDirection = m4d::vec4();
    localRayDir = device->camera->GetRayDir(x, y);

    m4d::vec4 base0 = locTetrad->getE(0);
    m4d::vec4 base1 = locTetrad->getE(1);
    m4d::vec4 base2 = locTetrad->getE(2);
    m4d::vec4 base3 = locTetrad->getE(3);
    // locTetrad->printP();

    m4d::vec4 rayDir = -base0 + localRayDir.x(0) * base1 + localRayDir.x(1) * base2 + localRayDir.x(2) * base3;
    // rayDir.print();

    // if local tetrad is given with respect to a natural local frame, then
    // one has to transforma  from local tetrad to coordinates
    if (locTetrad->getInCoords()) {
        rayDirection = rayDir;
    }
    else {
        device->metric->localToCoord(locTetrad->getPosition(), rayDir, rayDirection);
    }
}

void GvsProjector::Print(FILE* fptr)
{
    fprintf(fptr, "ProjectorStd {\n");
    if (locTetrad == NULL) {
        fprintf(fptr, "\tlocal tetrad is missing!\n");
    }
    else {
        fprintf(fptr, "\tPosition: ");
        (locTetrad->getPosition()).printS(fptr);
        fprintf(fptr, "\tVelocity: ");
        (locTetrad->getVelocity()).printS(fptr);
        fprintf(fptr, "\ttetrad in coords: %s\n", (locTetrad->getInCoords()) ? "yes" : "no");
        fprintf(fptr, "\tright-handed: %s\n", (locTetrad->isRightHanded()) ? "yes" : "no");
        fprintf(fptr, "\tbgColor:  ");
        backgroundColor.Print(fptr);
        rayGen->Print(fptr);
        locTetrad->Print(fptr);
    }
    fprintf(fptr, "}\n\n");
}

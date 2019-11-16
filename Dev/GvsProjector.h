/**
 * @file    GvsProjector.h
 * @author  Thomas Mueller
 *
 * @brief  The projector represents the observer within a scene given
 *         with respect to a local reference frame (LRF). This LRF can
 *         be either defined as simple local tetrad or as a motion.
 *
 *  This file is part of GeoViS.
 */
#ifndef GVS_PROJECTOR_H
#define GVS_PROJECTOR_H

#include "GvsGlobalDefs.h"

#include "Cam/GvsCamera.h"
#include "Obj/GvsBase.h"
#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Obj/STMotion/GvsStMotion.h"
#include "Ray/GvsRayAllIS.h"
#include "Ray/GvsRayVisual.h"

/**
 * The projector represents the observer within a scene given
 * with respect to a local reference frame (LRF). This LRF can
 * be either defined as simple local tetrad or as a motion.
 *
 */
class GvsProjector : public GvsBase
{
public:
    GvsProjector();
    explicit GvsProjector(GvsRayGen* gen);
    GvsProjector(GvsRayGen* gen, GvsLocalTetrad* lT);
    virtual ~GvsProjector();

    void setRayGen(GvsRayGen* gen);
    GvsRayGen* getRayGen() const;

    GvsColor getBackgroundColor() const;
    void setBackgroundColor(const GvsColor& backCol);

    /// If unknown error occured, use this color
    GvsColor getErrorColor() const;
    void setErrorColor(const GvsColor& col);

    /// If the constraint equation is violated, use this color
    GvsColor getConstraintColor() const;
    void setConstraintColor(const GvsColor& col);

    GvsColor getBreakDownColor() const;
    void setBreakDownColor(const GvsColor& col);

    /**
     * Get the sample color for pixel (x,y).
     *   A visual ray for pixel (x,y) is generated depending on the camera of the scene.
     *   Then, the 'getSampleColor(eyeRay, device)' is called.
     * @param device   pointer to current scene device
     * @param x   x-coordinate of pixel
     * @param y   y-coordinate of pixel
     * @return  rendered color
     */
    void getSampleColor(GvsDevice* device, double x, double y, GvsColor& col, gvsData& data) const;

    /**
     * Get the sample color from the light ray.
     *   The visual ray is tested for intersections with all objects in the scene.
     * @param eyeRay  visual ray
     * @param device  scene device
     * @return rendered color
     */
    GvsColor getSampleColor(GvsRayVisual*& eyeRay, GvsDevice* device) const;

    void getSampleIntersection(GvsDevice* device, double x, double y);
    // void getSampleIntersection(GvsRayAllIS*& eyeRay, GvsDevice* device);
    gvsData getSampleIntersection(GvsRayVisual*& eyeRay, GvsDevice* device) const;

    /**
     * Set the local tetrad (local reference frame) of the observer.
     * @param lT  pointer to local tetrad
     */
    void setLocalTetrad(GvsLocalTetrad* lT);

    /**
     * Get the currently set local tetrad of the observer.
     * @return  pointer to the local tetrad.
     */
    GvsLocalTetrad* getLocalTetrad();

    /**
     * Set the local tetrad (local reference frame) of the observer by means of the basis
     * vectors e0-e3
     * @param e0   basis-vector e0 of the local tetrad
     * @param e1   basis-vector e1 of the local tetrad
     * @param e2   basis-vector e2 of the local tetrad
     * @param e3   basis-vector e3 of the local tetrad
     * @param inCoord   true if basis-vectors are in coordinate representation
     */
    void setTetrad(
        const m4d::vec4& e0, const m4d::vec4& e1, const m4d::vec4& e2, const m4d::vec4& e3, bool inCoord = false);

    /**
     * Set the current position of the observer.
     *   This method can be called only after setting a local tetrad.
     *   The local tetrad will be adapted accordingly.
     * @param pos
     */
    void setPosition(const m4d::vec4& pos);

    /**
     * Get the current position of the observer.
     * @return position
     */
    m4d::vec4 getPosition() const;

    virtual void setMotion(GvsStMotion* motion);
    virtual GvsStMotion* getMotion() const;

    virtual void setActualPos(int nr); //!< Select a position from the motion.

    /**
     * Set projector parameters by key-value pair.
     * @param pName   parameter name
     * @param pt      parameter value
     * @return
     */
    virtual int SetParam(std::string pName, m4d::vec4 pt);
    virtual int SetParam(std::string pName, int nr);

    /** Get ray direction
     * The initial light ray direction is determined with respect to the camera model
     * and the local tetrad of the observer.
     * @param device   Pointer to the actual device.
     * @param x    Horizontal image pixel [0,resH-1]
     * @param y    Vertical image pixel [0,resV-1]
     * @param rayDirection   Initial direction of light ray given as four-vector in coordinate representation.
     * @param localRayDir    Initial direction of light ray with respect to local tetrad.
     */
    virtual void getRayDir(
        GvsDevice* device, const double x, const double y, m4d::vec4& rayDirection, m4d::vec3& localRayDir) const;

    virtual void Print(FILE* fptr = stderr);

protected:
    GvsColor backgroundColor;
    GvsColor errorColor;
    GvsColor constraintColor; //!< If the constraint equation is violated then use this color
    GvsColor breakDownColor; //!< If the geodesic integration breaks down (black hole horizon)
    GvsRayGen* rayGen;
    GvsLocalTetrad* locTetrad; //!< If the projector is static.
    GvsStMotion* stMotion; //!< If the projector is in motion.
};

#endif

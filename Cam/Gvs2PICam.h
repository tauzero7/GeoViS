/**
 * @file    Gvs2PICam.h
 * @author  Thomas Mueller
 *
 * @brief  Camera model for 2Pi projection that can be used as domemaster
 *         for planetarium projections.
 *
 *  This file is part of GeoViS.
 */
#ifndef GVS_2PI_CAM_H
#define GVS_2PI_CAM_H

#include "Cam/GvsCamera.h"
#include <GvsGlobalDefs.h>

#include "m4dGlobalDefs.h"

class Gvs2PICam : public GvsCamera
{
public:
    Gvs2PICam();
    Gvs2PICam(const double heading, const double pitch, const int res);
    virtual ~Gvs2PICam();

    void setHeading(const double heading);
    void setPitch(const double pitch);

    double getHeading() const;
    double getPitch() const;

    std::string install();

    virtual m4d::vec3 GetRayDir(const double x, const double y);

    virtual int SetParam(std::string pName, double val);

    virtual void Print(FILE* fptr = stderr);

private:
    double viewHeading; //!< View heading in degrees
    double viewPitch; //!< View pitch in degrees
};

#endif

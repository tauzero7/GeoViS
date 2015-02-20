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
#ifndef GVS_PIN_HOLE_CAM_H
#define GVS_PIN_HOLE_CAM_H

#include "GvsGlobalDefs.h"
#include "GvsCamera.h"

#include "m4dGlobalDefs.h"

/**
 * @brief The pinhole camera is that what you think.
 */
class GvsPinHoleCam : public GvsCamera
{
public:
    GvsPinHoleCam();
    GvsPinHoleCam(const m4d::vec3 &dir, const m4d::vec3 &vup, const m4d::vec2 &fov, const m4d::ivec2 &res);
    virtual ~GvsPinHoleCam();

    void   setViewDirection ( const m4d::vec3 &dir );
    void   setViewUpVector  ( const m4d::vec3 &vup );
    void   setFieldOfView   ( const m4d::vec2 &fov );

    m4d::vec3  getViewDirection ( ) const;
    m4d::vec3  getViewUpVector  ( ) const;
    m4d::vec2  getFieldOfView   ( ) const;

    std::string  install ( );

    virtual m4d::vec3 GetRayDir ( const double x, const double y );
    virtual void      PixelToAngle ( const double x, const double y, double &ksi, double &chi );

    virtual int SetParam ( std::string pName, m4d::vec2 vec );
    virtual int SetParam ( std::string pName, m4d::vec3 vec );

    virtual void Print( FILE* fptr = stderr );

protected:
    m4d::vec3  viewDirection;   //!< Direction of view with respect to local frame
    m4d::vec3  viewUpVector;    //!< Up vector of the camera
    m4d::vec2  viewField;       //!< Field of view

    m4d::vec3  viewHorizHalfVector;
    m4d::vec3  viewVertHalfVector;
    m4d::vec3  HorizDir, VertDir;
};

#endif

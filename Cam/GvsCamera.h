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
#ifndef GVS_CAMERA_H
#define GVS_CAMERA_H

#include "GvsGlobalDefs.h"
#include "Obj/GvsBase.h"

#include "m4dGlobalDefs.h"

/**
 * @brief  Base class for all camera models.
 *
 */
class API_EXPORT GvsCamera : public GvsBase
{
public:
    GvsCamera();
    GvsCamera( const GvsCamFilter filter );
    virtual ~GvsCamera();

    // projector asks for the ray direction for the pixel (x,y)
    virtual m4d::vec3 GetRayDir ( const double x, const double y ) = 0;
    virtual void      PixelToAngle ( const double x, const double y, double &ksi, double &chi );

    void        SetResolution( const m4d::ivec2 &res );
    m4d::ivec2  GetResolution() const;

    void         setCamFilter ( GvsCamFilter filter );
    GvsCamFilter getCamFilter ( ) const;

    void   setAspectRatio     ( double a );
    double getAspectRatio     ( ) const;

    void    SetEyeSep( double sep );
    double  GetEyeSep();
    virtual m4d::vec3 GetLeftEyePos();
    virtual m4d::vec3 GetRightEyePos();

    void   setParameter       ( double param );
    double getParameter      ( );

    bool   isRedshift();
    bool   isTimelapse();
    bool   isPolarisation();
    bool   isAllData();
    bool   isMask();

    bool   isStereoCam();

    virtual int  SetParam ( std::string pName, m4d::ivec2 val );
    virtual void Print( FILE* fptr = stderr );


protected:
    m4d::ivec2  viewResolution;  //!< Image resolution.
    double      aspectRatio;     //!< Aspect ratio of image.

    GvsCamFilter camFilter;      //!< Camera filter: RGB, RGBpdz, RGBjac

    bool mRedShift;
    bool mTimeShift;
    bool mPolarisation;
    bool mAllData;
    bool mMask;
    double mParam;

    bool mIsStereoCam;
    double mEyeSep;
};

#endif

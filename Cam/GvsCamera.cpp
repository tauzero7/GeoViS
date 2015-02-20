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

#include "GvsCamera.h"


GvsCamera::GvsCamera() : GvsBase(),
    aspectRatio(1.0),
    camFilter(gvsCamFilterRGB),
    mRedShift(false), mTimeShift(false), mPolarisation(false), mAllData(false), mMask(false),
    mIsStereoCam(false) {
    viewResolution = m4d::ivec2(720,576);
}

GvsCamera::GvsCamera( const GvsCamFilter filter ) :
    aspectRatio(1.0),
    camFilter(filter),
    mRedShift(false), mTimeShift(false), mPolarisation(false), mAllData(false), mMask(false),
    mIsStereoCam(false) {
    viewResolution = m4d::ivec2(720,576);
}

GvsCamera::~GvsCamera() {
}

void GvsCamera::PixelToAngle ( const double , const double , double &, double & ) {
    fprintf(stderr,"PixelToAngle: not implemented yet\n");
}

void GvsCamera::SetResolution( const m4d::ivec2 &res ) {
    viewResolution = res;
}

m4d::ivec2 GvsCamera::GetResolution() const {
    return viewResolution;
}

void GvsCamera::setAspectRatio(double a) {
    aspectRatio = a;
}

double GvsCamera::getAspectRatio() const {
    return aspectRatio;
}


void   GvsCamera::SetEyeSep( double sep ) {
    mEyeSep = sep;
}

double  GvsCamera::GetEyeSep() {
    return mEyeSep;
}

m4d::vec3 GvsCamera::GetLeftEyePos() {
    return m4d::vec3();
}

m4d::vec3 GvsCamera::GetRightEyePos() {
    return m4d::vec3();
}

void GvsCamera::setParameter ( double param ) {
    mParam = param;
}

double GvsCamera::getParameter() {
    return mParam;
}

void GvsCamera::setCamFilter ( GvsCamFilter filter ) {
    camFilter = filter;
    mRedShift = mTimeShift = mPolarisation = false;
}

GvsCamFilter GvsCamera::getCamFilter ( ) const {
    return camFilter;
}

bool GvsCamera::isRedshift() {
    return mRedShift;
}

bool GvsCamera::isTimelapse() {
    return mTimeShift;
}

bool GvsCamera::isPolarisation() {
    return mPolarisation;
}

bool GvsCamera::isAllData() {
    return mAllData;
}

bool GvsCamera::isMask() {
    return mMask;
}

bool GvsCamera::isStereoCam() {
    return mIsStereoCam;
}

int GvsCamera::SetParam(std::string pName, m4d::ivec2 val ) {
    int isOkay = GvsBase::SetParam(pName,val);
    return isOkay;
}

void GvsCamera::Print( FILE* fptr ) {
    fprintf(fptr,"Camera {\n");
    fprintf(fptr,"\tres  %4d x %4d\n",viewResolution.x(0),viewResolution.x(1));
    fprintf(fptr,"\tfilt %s\n",GvsCamFilterNames[camFilter].c_str());
    fprintf(fptr,"}\n");
}

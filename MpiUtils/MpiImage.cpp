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

#include "MpiImage.h"
#include "Img/GvsChannelImg2D.h"
#include "Img/GvsPicIOEnvelope.h"
#include "Img/GvsPictureIO.h"

#include <iostream>
#include <sstream>

GvsMpiImage ::GvsMpiImage()
    : mNumChannels(3)
    , mImageWidth(0)
    , mImageHeight(0)
    , mBuffer(NULL)
    , mData(NULL)
    , mWithData(false)
    , mNumTasksLeft(0)
    , mActive(false)
{
    mOutfilename = "";
}

GvsMpiImage ::~GvsMpiImage()
{
    if (mBuffer != NULL) {
        delete[] mBuffer;
    }
    if (mData != NULL) {
        delete[] mData;
    }
}

void GvsMpiImage::setWithData(bool withData)
{
    mWithData = withData;
}

void GvsMpiImage::setOutfilename(const std::string filename, const std::string fileExt, const int num)
{
    // std::ostringstream buf;

    std::string fName;
    std::string fExt;
    GvsPictureIO::get_extension(filename, fName, fExt);

    // buf << fName << "_" << num << fileExt << fExt;

    char buf[1024];
    sprintf(buf, "%s_%06d%s%s", fName.c_str(), num, fileExt.c_str(), fExt.c_str());

    mOutfilename = std::string(buf);
}

std::string GvsMpiImage ::getOutfilename() const
{
    return mOutfilename;
}

void GvsMpiImage ::setImageResolution(int x, int y)
{
    assert((x > 0) && (y > 0));
    mImageWidth = x;
    mImageHeight = y;
}

void GvsMpiImage ::setNumChannels(int num)
{
    assert((num > 0) && (num < 5));
    mNumChannels = num;
}

int GvsMpiImage ::getNumChannels(void) const
{
    return mNumChannels;
}

void GvsMpiImage ::insertRegion(int x1, int y1, int x2, int y2, uchar* p, gvsData* data)
{
    if (mActive == false) {
        activate();
    }

    assert(mBuffer != NULL);
    assert(!((x1 >= mImageWidth) || (x2 >= mImageWidth) || (x1 < 0) || (x2 < 0) || (y1 >= mImageHeight)
        || (y2 >= mImageHeight) || (y1 < 0) || (y2 < 0)));

    gvsData* dptr = data;
    for (int j = y1; j <= y2; j++) {
        for (int i = x1; i <= x2; i++) {
            for (int c = 0; c < mNumChannels; c++) {
                mBuffer[(i + mImageWidth * j) * mNumChannels + c] += *p;
                p++;
            }
            if (data != nullptr && mData != nullptr) {
                memcpy(&mData[j * mImageWidth + i], dptr, sizeof(gvsData));
                dptr++;
            }
        }
    }
    decreaseNumTasksLeft();
}

void GvsMpiImage ::setNumTasks(int num)
{
    mNumTasksLeft = num;
}

int GvsMpiImage ::getNumTasksLeft(void) const
{
    return mNumTasksLeft;
}

void GvsMpiImage ::writePicture(GvsCamFilter filter, double gamma)
{
    std::cerr << "GvsMpiImage :: writePicture\n";
    assert(mOutfilename != "");

    GvsChannelImg2D image(mImageWidth, mImageHeight, mNumChannels, mWithData);
    correctGamma(gamma);
    image.setBlock(0, 0, mImageWidth, mImageHeight, mBuffer);
    if (mWithData) {
        image.setDataBlock(0, 0, mImageWidth, mImageHeight, mData);
        image.writeIntersecData(mOutfilename.c_str(), filter);
    }
    GvsPicIOEnvelope().writeChannelImg(image, mOutfilename.c_str());
}

bool GvsMpiImage ::writeImageFileIfPossible(GvsCamFilter filter, double gamma)
{
    if (mNumTasksLeft <= 0) {
        writePicture(filter, gamma);
        stop(); // leert den Speicher
        return true;
    }
    return false;
}

void GvsMpiImage ::Print(FILE* fptr) const
{
    fprintf(fptr, "GvsMpiImage: {\n");
    fprintf(fptr, "\tout:%s  tasks left: %d\n", mOutfilename.c_str(), mNumTasksLeft);
    fprintf(fptr, "}\n");
}

void GvsMpiImage ::correctGamma(double gamma)
{
    if (gamma == 0.0 || gamma == 1.0)
        return;

    double invGamma = 1.0 / gamma;

    int bufferSize = mImageWidth * mImageHeight * mNumChannels;
    double buf;
    for (int i = 0; i < bufferSize; i++) {
        buf = pow((double)mBuffer[i], invGamma);
        mBuffer[i] = (uchar)buf;
    }
}

// If there is no buffer yet, do allocate it.
void GvsMpiImage::activate()
{
    if (mBuffer == NULL) {
        int bufferSize = mImageWidth * mImageHeight * mNumChannels;
        mBuffer = new uchar[bufferSize];

        for (int i = 0; i < bufferSize; i++) {
            mBuffer[i] = 0;
        }
    }

    if (mWithData && mData == NULL) {
        int bufferSize = mImageWidth * mImageHeight;
        mData = new gvsData[bufferSize];
    }

    mActive = true;
}

void GvsMpiImage ::decreaseNumTasksLeft(void)
{
    mNumTasksLeft--;
}

void GvsMpiImage ::stop(void)
{
    if (mBuffer != NULL) {
        delete[] mBuffer;
        mBuffer = NULL;
    }
    if (mData != NULL) {
        delete[] mData;
        mData = NULL;
    }
    mActive = false;
}

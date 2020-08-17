/**
 * @file    GvsCheckerT2D.h
 * @author  Thomas Mueller
 *
 * @brief  Checker texture
 *
 *  This file is part of GeoViS.
 */
#ifndef GVS_CHECKER_T2D_H
#define GVS_CHECKER_T2D_H

#include "GvsTexture2D.h"

class GvsCheckerT2D : public GvsTexture2D
{
public:
    GvsCheckerT2D(void);
    GvsCheckerT2D(GvsTexture* t0, GvsTexture* t1);
    GvsCheckerT2D(GvsTexture* t0, GvsTexture* t1, const m4d::Matrix<double, 2, 3>&);
    ~GvsCheckerT2D(void);

    virtual void Print(FILE* fptr = stderr);

    virtual double sampleValue(GvsSurfIntersec&) const;
    virtual GvsColor sampleColor(GvsSurfIntersec&) const;

    GvsTexture* texture0() { return tex0; }
    GvsTexture* texture1() { return tex1; }

    void setTexture0(GvsTexture* tex);
    void setTexture1(GvsTexture* tex);

private:
    GvsTexture* tex0;
    GvsTexture* tex1;
};

#endif

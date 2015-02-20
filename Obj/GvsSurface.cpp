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

#include "GvsSurface.h"
#include "Shader/Surface/GvsSurfaceShader.h"
#include "Ray/GvsSurfIntersec.h"


GvsSurface::GvsSurface() {
    surfShader = NULL;
}

GvsSurface::GvsSurface(GvsSurfaceShader *shader) {
    surfShader = shader;
}

GvsSurface::~GvsSurface() {
    // TODO: only set 0 or delete ??
    surfShader = NULL;
}

void GvsSurface::setShader ( GvsSurfaceShader *shader ) {
    surfShader = shader;
}

GvsSurfaceShader* GvsSurface::shader () const {
    return surfShader;
}

GvsShader* GvsSurface::shader( const GvsSurfIntersec &  ) const {
    return surfShader;
}


void GvsSurface::calcTexUVParam( GvsSurfIntersec & intersec ) const {
    if (intersec.surfSTParamAreValid()) {
        intersec.setTexUVParam( intersec.surfSTParam() );
    }
    else {
        GvsBoundBox BB = boundingBox();
        m4d::vec3 BBsize = BB.uppBounds() - BB.lowBounds();

        int i0 = BBsize.mostDominantCoord();  // i0 ist der Index fuer die
        // laengste Boxkante
        int i1 = (i0 + 1) % 3;
        int i2 = (i0 + 2) % 3;

        if (BBsize[i1] < BBsize[i2])      // i1 ist der Index fuer die zweit-
            i1 = i2;                        // laengste Boxkante

        double uParam = (intersec.point()[i0] - BB.lowBounds()[i0]) / BBsize[i0];
        double vParam = (intersec.point()[i1] - BB.lowBounds()[i1]) / BBsize[i1];

        intersec.setTexUVParam( uParam, vParam );
    }
}


//-------------------------------------------------------------------------
//   Method    GvsSurface :: c a l c D e r i v a t i v e s
//-------------------------------------------------------------------------
//   Default insbes. fuer Primitiva, die nicht explizit parametrisiert
//   sind und daher auch keine offensichtlichen Ableitungen berechnen
//   koennen.
//   Es werden die Ableitungen des Einheitskugelpunktes berechnet,
//   der intersec.normal() als Normale hat.
//-------------------------------------------------------------------------

// void GvsSurface :: calcDerivatives( GvsSurfIntersec & intersec ) const
// {
//    m4d::vec3 norm = intersec.normal();
//
//    double phi = atan2( norm.y(), norm.x() );
//    double theta = asin( norm.z() );
//
//    m4d::vec3 Du = m4d::vec3( cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta) );
//    intersec.setDerivS( Du );
//
//    m4d::vec3 Dv = intersec.normal() ^ Du;
//    intersec.setDerivT( Dv );
// }

void GvsSurface::calcDerivatives( GvsSurfIntersec & intersec ) const {
    m4d::vec3 norm = intersec.normal();
    double length = norm.normalize();

    double s = atan2( norm.x(1), norm.x(0) );
    double t = atan2( norm.x(2), sqrt( norm.x(0)*norm.x(0) + norm.x(1)*norm.x(1) ) );

    m4d::vec3 q = m4d::vec3( -cos(s)*sin(t), -sin(s)*sin(t), cos(t) );
    m4d::vec3 Ft = sqrt(length) * q;
    intersec.setDerivT( Ft );

    intersec.setDerivS( Ft ^ norm );
}

void GvsSurface::Print ( FILE *fptr ) {
    fprintf(fptr,"GvsSurface {}\n");
    // TODO
}

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

#include "GvsColor.h"

const double sixty = 1.047198;

GvsColor RgbBlack  ( 0, 0, 0 );
GvsColor RgbWhite  ( 1, 1, 1 );
GvsColor RgbRed    ( 1, 0, 0 );
GvsColor RgbGreen  ( 0, 1, 0 );
GvsColor RgbBlue   ( 0, 0, 1 );
GvsColor RgbYellow ( 1, 1, 0 );
GvsColor RgbMagenta( 1, 0, 1 );
GvsColor RgbCyan   ( 0, 1, 1 );


GvsColor :: GvsColor ( double gray ) {
    red = green = blue = gray;
    alpha = 0.0;
    valid = true;
}

GvsColor :: GvsColor ( double r, double g, double b, double a ) {
    red   = r;
    green = g;
    blue  = b;
    alpha = a;
    valid = true;
}


GvsColor :: ~GvsColor() {
}

void GvsColor::setValid( bool valid ) {
    this->valid = valid;
}

bool GvsColor::isValid() {
    return valid;
}


double GvsColor::operator[] ( int i ) const {
    if ( i == 0 ) {
        return red;
    }
    else if ( i == 1 ) {
        return green;
    }
    else if ( i == 2 ) {
        return blue;
    }
    else if ( i == 3 ) {
        return alpha;
    }
    else  {
        std::cerr << "Error in GvsColor::operator[]: Index out of bounds." << std::endl;
        return red;
    }
}

double&
GvsColor :: operator[] ( int i )
{
    if ( i == 0 ) {
        return red;
    }
    else if ( i == 1 ) {
        return green;
    }
    else if ( i == 2 ) {
        return blue;
    }
    else if ( i == 3 ) {
        return alpha;
    }
    else
    {
        std::cerr << "Error in GvsColor::operator[]: Index out of bounds." << std::endl;
        return red;
    }
}

void
GvsColor :: operator= ( const GvsColor &col )
{
    red   = col.red;
    green = col.green;
    blue  = col.blue;
    alpha = col.alpha;

    data = col.data;
}

//-------------------------------------------------------------------------
//   friend function     m a x R g b D i s t
//-------------------------------------------------------------------------
double maxRgbDist ( const GvsColor &col0, const GvsColor &col1 )
{
    float rdist = fabs( col1.red   - col0.red   );
    float gdist = fabs( col1.green - col0.green );
    float bdist = fabs( col1.blue  - col0.blue  );

    return (rdist > gdist) ? ((rdist > bdist) ? rdist : bdist)
                           : ((gdist > bdist) ? gdist : bdist);
}

//-------------------------------------------------------------------------
//   friend function     m a n h a t t e n D i s t
//-------------------------------------------------------------------------
double manhattenDist ( const GvsColor &col0, const GvsColor &col1 )
{
    return fabs( col1.red   - col0.red   ) +
            fabs( col1.green - col0.green ) +
            fabs( col1.blue  - col0.blue  );
}

// --------------------------------------------------------------------------
//              trim
// --------------------------------------------------------------------------
GvsColor
GvsColor :: trim ()
{
    if ( red > 1.0 )
    {
        //std::cerr << "Warning: RvsColor::trim() called (red)." << std::endl;
        red = 1.0;
    }

    if ( green > 1.0 )
    {
        //std::cerr << "Warning: RvsColor::trim() called (green)." << std::endl;
        green = 1.0;
    }

    if ( blue > 1.0 )
    {
        //std::cerr << "Warning: RvsColor::trim() called (blue)." << std::endl;
        blue = 1.0;
    }

    return *this;
}


GvsColor GvsColor :: trimAlpha () {
    if ( alpha > 1.0 )   {
        //std::cerr << "Warning: RvsColor::trim() called (alpha)." << std::endl;
        alpha = 1.0;
    }
    return *this;
}


double GvsColor::luminance() const {
    return 0.299 * red + 0.587 * green + 0.114 * blue;
}

void GvsColor::setRGB( double r, double g, double b ) {
    red = r;
    green = g;
    blue = b;
}

void GvsColor::setHSV( double H, double S, double V ) {
    // http://de.wikipedia.org/wiki/HSV-Farbraum
    // H in [0,360] degree
    // S,V in [0,1]

    int hi = int(floor(H*GVS_deg2rad/sixty));
    double f = H*GVS_deg2rad/sixty - hi;
    double p = V*(1.0 - S);
    double q = V*(1.0 - S*f);
    double t = V*(1.0 - S*(1.0-f));

    switch (hi) {
        case 0:
        case 6: {
            setRGB(V,t,p);
            break;
        }
        case 1: {
            setRGB(q,V,p);
            break;
        }
        case 2: {
            setRGB(p,V,t);
            break;
        }
        case 3: {
            setRGB(p,q,V);
            break;
        }
        case 4: {
            setRGB(t,p,V);
            break;
        }
        case 5: {
            setRGB(V,p,q);
            break;
        }
    }
}


// --------------------------------------------------------------------------
//              printRGB(a), all
// --------------------------------------------------------------------------
void
GvsColor :: printRGB ( std::ostream &os ) const
{
    os << red << " " << green << " " << blue << std::endl;
}

void
GvsColor :: printRGBa ( std::ostream &os ) const
{
    os << red << " " << green << " " << blue << " " << alpha << std::endl;
}

void
GvsColor :: printAll  ( std::ostream &os ) const
{
    std::cerr << "GvsColor :: printAll()...\n";
    os << red << " " << green << " " << blue << " " << alpha << "  ";
    //os << data.redShift << " ";
    //os << data.timeShift << " ";
    /*
  for (int i=0; i<5; i++)
    os << data.pos[i] << " ";

  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      os << data.base[i][j] << " ";
*/
    os << std::endl;
}

void GvsColor::Print ( FILE* fptr ) const {
    fprintf(fptr,"%6.3f %6.3f %6.3f %6.3f\n",red,green,blue,alpha);
}


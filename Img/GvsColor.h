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
#ifndef GVS_COLOR_H
#define GVS_COLOR_H

#include "GvsGlobalDefs.h"
#include <iostream>

/**
 * @brief The GvsColor class stores not only the RGB value of a pixel
 *        but also other data like redshift etc.
 */
class GvsColor
{
public:
    GvsColor ( double gray = 0.0 );
    GvsColor ( double r, double g, double b, double alpha = 1.0 );
    ~GvsColor();

    void      setValid( bool valid );
    bool      isValid();

    GvsColor  operator+ ( const GvsColor &col ) const;
    GvsColor  operator- ( const GvsColor &col ) const;
    GvsColor  operator* ( const GvsColor &col ) const;
    GvsColor  operator* ( double scale        ) const;
    GvsColor  operator/ ( double scale        ) const;
    void      operator= ( const GvsColor &col );

    void      operator+= ( const GvsColor &col );
    void      operator-= ( const GvsColor &col );
    void      operator*= ( const GvsColor &col );
    void      operator*= ( double scale        );
    void      operator/= ( double scale        );

    int       operator== ( const GvsColor &col ) const;
    int       operator!= ( const GvsColor &col ) const;
    int       operator<  ( const GvsColor &col ) const;
    int       operator<= ( const GvsColor &col ) const;
    int       operator>  ( const GvsColor &col ) const;
    int       operator>= ( const GvsColor &col ) const;

    double    operator[] ( int i ) const;
    double&   operator[] ( int i );

    GvsColor  trim       ( );
    GvsColor  trimAlpha  ( );
    double    luminance  ( ) const;

    void      setRGB( double r, double g, double b );
    void      setHSV( double H, double S, double V );

    void      printRGB  ( std::ostream &os = std::cerr ) const;
    void      printRGBa ( std::ostream &os = std::cerr ) const;
    void      printAll  ( std::ostream &os = std::cerr ) const;
    void      Print ( FILE* fptr = stderr ) const;

    // friends:
    friend  GvsColor  operator*     ( double scale, const GvsColor &col );
    friend  double    maxRgbDist    ( const GvsColor &col0, const GvsColor &col1 );
    friend  double    manhattenDist ( const GvsColor &col0, const GvsColor &col1 );


public:
    bool     valid;                     // is valid color
    double   red, green, blue, alpha;   // rgb-values
    gvsData  data;
};


/*-------------------------------------------------------------------------*/
/*                     C o n s t a n t s                                   */
/*-------------------------------------------------------------------------*/

extern GvsColor RgbBlack;
extern GvsColor RgbWhite;
extern GvsColor RgbRed;
extern GvsColor RgbGreen;
extern GvsColor RgbBlue;
extern GvsColor RgbYellow;
extern GvsColor RgbMagenta;
extern GvsColor RgbCyan;

/*-------------------------------------------------------------------------*/
/*                     I n l i n e    f u n c t i o n s                    */
/*-------------------------------------------------------------------------*/

inline GvsColor
GvsColor :: operator+ ( const GvsColor &col ) const
{
    return GvsColor ( red + col.red, green + col.green, blue + col.blue, alpha + col.alpha );
}

inline GvsColor
GvsColor :: operator- ( const GvsColor &col ) const
{
    return GvsColor ( red - col.red, green - col.green, blue - col.blue, alpha - col.alpha );
}

inline GvsColor
GvsColor :: operator* ( const GvsColor &col ) const
{
    return GvsColor( red * col.red, green * col.green, blue * col.blue, alpha * col.alpha );
}

inline GvsColor
GvsColor :: operator* ( double scale ) const
{
    return GvsColor( red * scale, green * scale, blue * scale, alpha * scale );
}

inline GvsColor
GvsColor :: operator/ ( double scale ) const
{
    return GvsColor( red / scale, green / scale, blue / scale, alpha / scale );
}

inline void
GvsColor :: operator+= ( const GvsColor &col )
{
    red += col.red;
    green += col.green;
    blue += col.blue;
    alpha += col.alpha;
}

inline void
GvsColor :: operator-= ( const GvsColor &col )
{
    red -= col.red;
    green -= col.green;
    blue -= col.blue;
    alpha -= col.alpha;
}

inline void
GvsColor :: operator*= ( const GvsColor &col )
{
    red *= col.red;
    green *= col.green;
    blue *= col.blue;
    alpha *= col.alpha;
}

inline void
GvsColor :: operator*= ( double scale )
{
    red *= scale;
    green *= scale;
    blue *= scale;
    alpha *= scale;
}

inline void
GvsColor :: operator/= ( double scale )
{
    red /= scale;
    green /= scale;
    blue /= scale;
    alpha /= scale;
}

inline int
GvsColor :: operator== ( const GvsColor &col ) const
{
    return (red == col.red) && (green == col.green) && (blue == col.blue) && (alpha == col.alpha);
}

inline int
GvsColor :: operator!= ( const GvsColor &col ) const
{
    return (red != col.red) || (green != col.green) || (blue != col.blue) || (alpha != col.alpha);
}

inline int
GvsColor :: operator < ( const GvsColor &col ) const
{
    return (red < col.red) && (green < col.green) && (blue < col.blue);
}

inline int
GvsColor :: operator <= ( const GvsColor &col ) const
{
    return (red <= col.red) && (green <= col.green) && (blue <= col.blue);
}

inline int
GvsColor :: operator > ( const GvsColor &col ) const
{
    return (red > col.red) && (green > col.green) && (blue > col.blue);
}

inline int
GvsColor :: operator >= ( const GvsColor &col ) const
{
    return (red >= col.red) && (green >= col.green) && (blue >= col.blue);
}

//friend
inline GvsColor operator* ( double scale, const GvsColor &col )
{
    return GvsColor( scale * col.red, scale * col.green, scale * col.blue, scale * col.alpha );
}


#endif

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

#include "Obj/GvsBoundBox.h"

#include "math/TransfMat.h"

//----------------------------------------------------------------------------
//         constructor, destructor
//----------------------------------------------------------------------------
GvsBoundBox::GvsBoundBox ( void )
{
}

GvsBoundBox::GvsBoundBox ( const m4d::vec3& lowBounds, const m4d::vec3& uppBounds,
                           bool testBounds)
{
    if ( testBounds )
    {
        for ( int i = 0; i < 3; i++ )
            if ( lowBounds[i] <= uppBounds[i] )
            {
                boxLowBounds[i] = lowBounds[i];
                boxUppBounds[i] = uppBounds[i];
            }
            else
            {
                boxLowBounds[i] = uppBounds[i];
                boxUppBounds[i] = lowBounds[i];
            }
    }
    else
    {
        boxLowBounds = lowBounds;
        boxUppBounds = uppBounds;
    }
}

GvsBoundBox :: GvsBoundBox ( double x,  double y,  double z,
                             double dx, double dy, double dz,
                             bool testBounds)
{
    if ( testBounds )
    {
        if ( dx >= 0 )
        {
            boxLowBounds[0] = x;
            boxUppBounds[0] = x + dx;
        }
        else
        {
            boxLowBounds[0] = x + dx;
            boxUppBounds[0] = x;
        }

        if ( dy >= 0 )
        {
            boxLowBounds[1] = y;
            boxUppBounds[1] = y + dy;
        }
        else
        {
            boxLowBounds[1] = y + dy;
            boxUppBounds[1] = y;
        }

        if ( dz >= 0 )
        {
            boxLowBounds[2] = z;
            boxUppBounds[2] = z + dz;
        }
        else
        {
            boxLowBounds[2] = z + dz;
            boxUppBounds[2] = z;
        }
    }
    else
    {
        boxLowBounds[0] = x;
        boxLowBounds[1] = y;
        boxLowBounds[2] = z;
        boxUppBounds[0] = x + dx;
        boxUppBounds[1] = y + dy;
        boxUppBounds[2] = z + dz;
    }
}

GvsBoundBox::GvsBoundBox ( const GvsBoundBox& otherBox )
{
    boxLowBounds = otherBox.boxLowBounds;
    boxUppBounds = otherBox.boxUppBounds;
}

//-------------------------------------------------------------------------
//   Method     GvsBoundBox :: i s E m p t y
//-------------------------------------------------------------------------
bool
GvsBoundBox :: isEmpty() const
{
    return (boxLowBounds[0] >= boxUppBounds[0]) ||
            (boxLowBounds[1] >= boxUppBounds[1]) ||
            (boxLowBounds[2] >= boxUppBounds[2]);
}


//-------------------------------------------------------------------------
//   Method     GvsBoundBox :: v o l u m e
//-------------------------------------------------------------------------
double
GvsBoundBox :: volume() const
{
    return (boxUppBounds[0] - boxLowBounds[0]) *
            (boxUppBounds[1] - boxLowBounds[1]) *
            (boxUppBounds[2] - boxLowBounds[2]);
}


//-------------------------------------------------------------------------
//   Method     GvsBoundBox :: s u r f a c e
//-------------------------------------------------------------------------
double
GvsBoundBox :: surface() const
{
    m4d::vec3 size = boxUppBounds - boxLowBounds;

    return 2.0 * (size[0] * size[1] + size[1] * size[2] + size[2] * size[0]);
}

//-------------------------------------------------------------------------
//     operator=
//-------------------------------------------------------------------------
bool
GvsBoundBox :: contains  ( const m4d::vec3 &point ) const
{
    return ( (point[0] >= boxLowBounds[0]) && (point[0] <= boxUppBounds[0]) &&
            (point[1] >= boxLowBounds[1]) && (point[1] <= boxUppBounds[1]) &&
            (point[2] >= boxLowBounds[2]) && (point[2] <= boxUppBounds[2]) );
}

//-------------------------------------------------------------------------
//     extendBoxToContain
//-------------------------------------------------------------------------

void
GvsBoundBox :: extendBoxToContain( const m4d::vec3& pt )
{
    for ( int i = 0; i < 3; i++ )
        if ( pt[i] < boxLowBounds[i] )
            boxLowBounds[i] = pt[i];
        else if ( pt[i] > boxUppBounds[i] )
            boxUppBounds[i] = pt[i];
}

void GvsBoundBox::setBounds(const m4d::vec3 lower, const m4d::vec3 upper) {
    boxLowBounds = lower;
    boxUppBounds = upper;
}


void GvsBoundBox::setBounds(
        const double llx, const double lly, const double llz,
        const double urx, const double ury, const double urz)
{
    boxLowBounds = m4d::vec3(llx, lly, llz);
    boxUppBounds = m4d::vec3(urx, ury, urz);
}


//-------------------------------------------------------------------------
//     operator=
//-------------------------------------------------------------------------
GvsBoundBox&
GvsBoundBox :: operator= ( const GvsBoundBox &otherBox )
{
    boxLowBounds = otherBox.boxLowBounds;
    boxUppBounds = otherBox.boxUppBounds;
    return *this;
}

//-------------------------------------------------------------------------
//     operator+   ( union )
//-------------------------------------------------------------------------
GvsBoundBox
GvsBoundBox :: operator+ ( const GvsBoundBox &otherBox ) const
{
    GvsBoundBox unionBox;

    for (int i=0; i<3; i++)
    {
        unionBox.boxLowBounds[i] = (boxLowBounds[i] < otherBox.boxLowBounds[i])
                ? boxLowBounds[i]
                  : otherBox.boxLowBounds[i];
        unionBox.boxUppBounds[i] = (boxUppBounds[i] > otherBox.boxUppBounds[i])
                ? boxUppBounds[i]
                  : otherBox.boxUppBounds[i];
    }
    return unionBox;
}

//-------------------------------------------------------------------------
//     operator+   ( union )
//-------------------------------------------------------------------------
GvsBoundBox&
GvsBoundBox :: operator+= ( const GvsBoundBox &otherBox )
{
    for (int i=0; i<3; i++ )
    {
        if ( otherBox.boxLowBounds[i] < boxLowBounds[i] )
            boxLowBounds[i] = otherBox.boxLowBounds[i];
        if ( otherBox.boxUppBounds[i] > boxUppBounds[i] )
            boxUppBounds[i] = otherBox.boxUppBounds[i];
    }
    return *this;
}

//-------------------------------------------------------------------------
//     operator*   ( intersection )
//-------------------------------------------------------------------------
GvsBoundBox
GvsBoundBox :: operator* ( const GvsBoundBox &otherBox ) const
{
    GvsBoundBox intersecBox;

    for (int i=0; i<3; i++ )
    {
        intersecBox.boxLowBounds[i] = (boxLowBounds[i] > otherBox.boxLowBounds[i])
                ? boxLowBounds[i]
                  : otherBox.boxLowBounds[i];
        intersecBox.boxUppBounds[i] = (boxUppBounds[i] < otherBox.boxUppBounds[i])
                ? boxUppBounds[i]
                  : otherBox.boxUppBounds[i];
    }
    return intersecBox;
}

//-------------------------------------------------------------------------
//     operator*=   ( intersection )
//-------------------------------------------------------------------------
GvsBoundBox&
GvsBoundBox :: operator*= ( const GvsBoundBox &otherBox )
{
    for (int i=0; i<3; i++ )
    {
        if ( otherBox.boxLowBounds[i] > boxLowBounds[i] )
            boxLowBounds[i] = otherBox.boxLowBounds[i];
        if ( otherBox.boxUppBounds[i] < boxUppBounds[i] )
            boxUppBounds[i] = otherBox.boxUppBounds[i];
    }
    return *this;
}


bool GvsBoundBox :: getTentryTexit ( const m4d::vec3 &p0, const m4d::vec3 &p1, double tp0, double tp1,
                                     double &time_Entry, double &time_Exit,
                                     short &entryFace, short &exitFace ) const
{
    double t1,t2;
    short coord;

    //cerr << "enter tentryexit  " << p0 << "  " << p1 << std::endl;
    double delta_tp = tp1-tp0;

    double mx = (p1[0] - p0[0]) / delta_tp;
    double my = (p1[1] - p0[1]) / delta_tp;
    double mz = (p1[2] - p0[2]) / delta_tp;

    double ax = p0[0] - mx*tp0;
    double ay = p0[1] - my*tp0;
    double az = p0[2] - mz*tp0;

    m4d::vec3 rayOrig(ax,ay,az);
    m4d::vec3 rayDir(mx,my,mz);

    time_Entry = -DBL_MAX;
    time_Exit  = DBL_MAX;

    for ( coord = 0; coord < 3; coord++ )
    {
        if ( fabs(rayDir[coord]) < GVS_EPS )
        {
            if ( (rayOrig[coord] < boxLowBounds[coord]) ||
                 (rayOrig[coord] > boxUppBounds[coord])    )
                return false;
        }
        else
        {
            t1 = (boxLowBounds[coord] - rayOrig[coord]) / rayDir[coord];
            t2 = (boxUppBounds[coord] - rayOrig[coord]) / rayDir[coord];

            if ( t1 < t2 )
            {
                if ( t1 > time_Entry )
                {
                    time_Entry = t1;
                    entryFace = (coord << 1);
                }
                if ( t2 < time_Exit  )
                {
                    time_Exit = t2;
                    exitFace = (coord << 1) + 1;
                }
            }
            else
            {
                if ( t2 > time_Entry )
                {
                    time_Entry = t2;
                    entryFace = (coord << 1) + 1;
                }
                if ( t1 < time_Exit  )
                {
                    time_Exit = t1;
                    exitFace = (coord << 1);
                }
            }

            //if( (time_Entry > time_Exit) || (time_Exit < RVS_EPS) ) return false;
            if ( (time_Entry > time_Exit)) return false;
        }
    }
    if (tp0 < tp1)
    {
        if (time_Entry > time_Exit)
        {
            double t = time_Entry;
            time_Entry = time_Exit;
            time_Exit  = t;
            short face = entryFace;
            entryFace  = exitFace;
            exitFace   = face;
        }
    }
    else
    {
        if (time_Entry < time_Exit)
        {
            double t = time_Entry;
            time_Entry = time_Exit;
            time_Exit  = t;
            short face = entryFace;
            entryFace  = exitFace;
            exitFace   = face;
        }
    }
    //cerr << "tentryexit : intersec " << std::endl;
    return true;
}


void
GvsBoundBox :: scale ( const m4d::vec3&   scaleVec )
{
    boxLowBounds *= scaleVec;
    boxUppBounds *= scaleVec;
}

void
GvsBoundBox :: translate  ( const m4d::vec3&  transVec )
{
    boxLowBounds += transVec;
    boxUppBounds += transVec;
}

void GvsBoundBox :: rotate( const m4d::vec3&   rotAxis, double rotAngle ) {
    transform( m4d::RotateMat3D(rotAxis,-rotAngle) );
}

void GvsBoundBox :: transform( const m4d::Matrix<double,3,4>& mat ) {
    //  std::cerr << "GvsBoundBox :: transform()" << std::endl;
    if ( ! mat.isIdentMat() )
    {
        m4d::vec3 low = boxLowBounds;
        m4d::vec3 upp = boxUppBounds;
        /*
    *this = GvsBoundBox( mat * low, mat * upp );

    extendBoxToContain( mat * m4d::vec3( upp[0], low[1], low[2] ) );
    extendBoxToContain( mat * m4d::vec3( low[0], upp[1], low[2] ) );
    extendBoxToContain( mat * m4d::vec3( upp[0], upp[1], low[2] ) );
    extendBoxToContain( mat * m4d::vec3( low[0], low[1], upp[2] ) );
    extendBoxToContain( mat * m4d::vec3( upp[0], low[1], upp[2] ) );
    extendBoxToContain( mat * m4d::vec3( low[0], upp[1], upp[2] ) );
    */
    }
}


void GvsBoundBox::Print ( FILE* fptr ) {
    fprintf(fptr,"BoundBox {\n");
    fprintf(fptr,"\t low: "); boxLowBounds.printS(fptr);
    fprintf(fptr,"\t upp: "); boxUppBounds.printS(fptr);
    fprintf(fptr,"}\n");
}

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
#ifndef GVS_SOL_OBJ_SPAN_LIST_H
#define GVS_SOL_OBJ_SPAN_LIST_H

#include <Ray/GvsSurfIntersec.h>


class GvsSolObjSpanList
{
public:
    GvsSolObjSpanList ();
    GvsSolObjSpanList ( int listLength );
    GvsSolObjSpanList ( const GvsSolObjSpanList& );

    ~GvsSolObjSpanList();

    bool             insert           ( GvsSurfIntersec &,
                                        GvsSurfIntersec & );
    int              deleteFirstSpan   ( void              );
    int              deleteSpan        ( int i             );
    GvsSurfIntersec  readFirstItem     ( void              ) const;
    bool             getFirstValidSpan ( GvsSurfIntersec &,
                                         GvsSurfIntersec &  ) const;

    int             numSpans          ( void              ) const;
    int             maxSpans          ( void              ) const;
    bool            isEmpty           ( void              ) const;
    bool            increaseNumSpans  ( int addnum        );

    // assignment
    GvsSolObjSpanList& operator =  ( const GvsSolObjSpanList& );

    // union
    GvsSolObjSpanList  operator +  ( const GvsSolObjSpanList& ) const;
    GvsSolObjSpanList& operator += ( const GvsSolObjSpanList& );

    // intersection
    GvsSolObjSpanList  operator *  ( const GvsSolObjSpanList& ) const;
    GvsSolObjSpanList& operator *= ( const GvsSolObjSpanList& );

    // difference
    GvsSolObjSpanList  operator -  ( const GvsSolObjSpanList& ) const;
    GvsSolObjSpanList& operator -= ( const GvsSolObjSpanList& );

    // inverse
    GvsSolObjSpanList  operator -  ( void                     ) const;
    GvsSolObjSpanList& invert      ( void                     );

    void Print( FILE* fptr = stderr );

private:
    GvsSurfIntersec* spanLo;
    GvsSurfIntersec* spanHi;
    int              spanPtr;
    int              spanListLength;

    enum{ DefaultListLength = 16 };
};



//--------------------------------------------------------------------------
//  inline member functions
//--------------------------------------------------------------------------

inline int GvsSolObjSpanList :: numSpans() const {
    return spanPtr;
}

inline int GvsSolObjSpanList :: maxSpans() const {
    return spanListLength;
}

inline bool GvsSolObjSpanList :: isEmpty() const {
    return (spanPtr == 0);
}

inline GvsSurfIntersec  GvsSolObjSpanList :: readFirstItem() const {
    return spanLo[0];
}


inline GvsSolObjSpanList&
GvsSolObjSpanList :: operator +=( const GvsSolObjSpanList& l1 )
{
    return *this = *this + l1;
}

inline GvsSolObjSpanList&
GvsSolObjSpanList :: operator *=( const GvsSolObjSpanList& l1 )
{
    return *this = *this * l1;
}

inline GvsSolObjSpanList
GvsSolObjSpanList :: operator -( const GvsSolObjSpanList& l1 ) const
{
    return *this * -l1;
}

inline GvsSolObjSpanList&
GvsSolObjSpanList :: operator -=( const GvsSolObjSpanList& l1 )
{
    return *this = *this - l1;
}

inline GvsSolObjSpanList& GvsSolObjSpanList :: invert()
{
    return *this = - *this;
}

#endif

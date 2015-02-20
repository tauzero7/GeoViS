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

#include "GvsSolObjSpanList.h"
#include <stdlib.h>    // wg. NULL !
#include <float.h>
#include <assert.h>

/**
 * @brief GvsSolObjSpanList::GvsSolObjSpanList
 */
GvsSolObjSpanList::GvsSolObjSpanList()
{
    spanLo = new GvsSurfIntersec[ DefaultListLength ];
    assert( spanLo != NULL );

    spanHi = new GvsSurfIntersec[ DefaultListLength ];
    assert( spanHi != NULL );

    spanListLength = DefaultListLength;
    spanPtr = 0;
}

/**
 * @brief GvsSolObjSpanList::GvsSolObjSpanList
 * @param listLength
 */
GvsSolObjSpanList::GvsSolObjSpanList( int listLength )
{
    spanListLength = (listLength > 0) ? listLength : DefaultListLength;

    spanLo = new GvsSurfIntersec[ spanListLength ];
    assert( spanLo != NULL );

    spanHi = new GvsSurfIntersec[ spanListLength ];
    assert( spanHi != NULL );

    spanPtr = 0;
}

/**
 * @brief GvsSolObjSpanList::GvsSolObjSpanList
 * @param sl
 */
GvsSolObjSpanList::GvsSolObjSpanList( const GvsSolObjSpanList& sl  )
{
    spanListLength = sl.spanListLength;

    spanLo = new GvsSurfIntersec[ spanListLength ];
    assert( spanLo != NULL );

    spanHi = new GvsSurfIntersec[ spanListLength ];
    assert( spanHi != NULL );

    for ( spanPtr = 0; spanPtr < sl.spanPtr; spanPtr++ ) {
        spanLo[spanPtr] = sl.spanLo[spanPtr];
        spanHi[spanPtr] = sl.spanHi[spanPtr];
    }
}

/**
 * @brief GvsSolObjSpanList::~GvsSolObjSpanList
 */
GvsSolObjSpanList::~GvsSolObjSpanList() {
    delete [] spanLo;
    delete [] spanHi;
    spanLo = spanHi = NULL;
    spanPtr = spanListLength = 0;
}

/**
 * @brief GvsSolObjSpanList::insert
 * @param s
 * @param e
 * @return
 */
bool GvsSolObjSpanList::insert( GvsSurfIntersec &s, GvsSurfIntersec &e )
{
    GvsSurfIntersec min, max;

    if ( s < e ) {
        min = s;
        max = e;
    }
    else {
        min = e;
        max = s;
    }

    if ( spanPtr == spanListLength ) {
        if ( !increaseNumSpans(1) ) {
            return false;
        }
    }

    for( int i = 0; i < spanPtr; i++ ) {
        if ( min < spanLo[i] ) {
            for ( int j = spanPtr; j > i; j-- ) {
                spanLo[j] = spanLo[j-1];
                spanHi[j] = spanHi[j-1];
            }
            spanPtr++;
            spanLo[i] = min;
            spanHi[i] = max;
            return true;
        }
    }

    spanLo[spanPtr] = min;
    spanHi[spanPtr] = max;
    spanPtr++;
    return true;
}

/**
 * @brief GvsSolObjSpanList::deleteFirstSpan
 * @return
 */
int GvsSolObjSpanList::deleteFirstSpan() {
    if ( spanPtr == 0 ) {
        return -1;
    }

    for ( int i = 0; i < spanPtr-1; i++ ) {
        spanLo[i] = spanLo[i+1];
        spanHi[i] = spanHi[i+1];
    }

    return --spanPtr;
}

/**
 * @brief GvsSolObjSpanList::deleteSpan
 * @param i
 * @return
 */
int GvsSolObjSpanList::deleteSpan( int i ) {
    if ( i < 0 || i >= spanPtr ) {
        return -1;
    }

    for ( int j = i; j < spanPtr-1; j++ ) {
        spanLo[j] = spanLo[j+1];
        spanHi[j] = spanHi[j+1];
    }
    return --spanPtr;
}

/**
 * @brief GvsSolObjSpanList::increaseNumSpans
 * @param addnum
 * @return
 */
bool GvsSolObjSpanList::increaseNumSpans( int addnum )
{
    GvsSurfIntersec *newSpanLo = new GvsSurfIntersec[ spanListLength + addnum ];
    if ( newSpanLo == NULL ) return false;


    GvsSurfIntersec *newSpanHi = new GvsSurfIntersec[ spanListLength + addnum ];
    if ( newSpanHi == NULL ) return false;

    spanListLength += addnum;

    for ( int i = 0; i < spanPtr; i++ ) {
        newSpanLo[i] = spanLo[i];
        newSpanHi[i] = spanHi[i];
    }

    delete [] spanLo;    spanLo = newSpanLo;
    delete [] spanHi;    spanHi = newSpanHi;
    return true;
}

/**
 * @brief GvsSolObjSpanList::getFirstValidSpan
 * @param spanStart
 * @param spanEnd
 * @return
 */
bool GvsSolObjSpanList::getFirstValidSpan(GvsSurfIntersec &spanStart,
                                          GvsSurfIntersec &spanEnd    ) const
{
    for ( int i = 0; i < spanPtr; i++ ) {
        if ( spanLo[i] > GVS_EPS ) {
            spanStart = spanLo[i];
            spanEnd   = spanHi[i];
            return true;
        }
    }
    return false;
}

/**
 * @brief GvsSolObjSpanList::operator =
 * @param sl
 * @return
 */
GvsSolObjSpanList& GvsSolObjSpanList::operator=( const GvsSolObjSpanList& sl ) {
    delete [] spanLo;
    delete [] spanHi;
    spanListLength = sl.spanListLength;

    spanLo = new GvsSurfIntersec[ spanListLength ];
    assert( spanLo != NULL );

    spanHi = new GvsSurfIntersec[ spanListLength ];
    assert( spanHi != NULL );

    for ( spanPtr = 0; spanPtr < sl.spanPtr; spanPtr++ ) {
        spanLo[spanPtr] = sl.spanLo[spanPtr];
        spanHi[spanPtr] = sl.spanHi[spanPtr];
    }
    return *this;
}

/**
 * @brief GvsSolObjSpanList::operator +
 * @param rsl
 * @return
 */
GvsSolObjSpanList GvsSolObjSpanList::operator+( const GvsSolObjSpanList& rsl ) const {
    GvsSolObjSpanList slUnion( spanPtr + rsl.spanPtr );

    register int i = 0, j = 0, k = 0;
    while( i < spanPtr || j < rsl.spanPtr ) {
        if ( i < spanPtr && (j >= rsl.spanPtr || spanLo[i] < rsl.spanLo[j]) ) {
            slUnion.spanLo[k] = spanLo[i];
            slUnion.spanHi[k] = spanHi[i];
            i++;
        }
        else {
            slUnion.spanLo[k] = rsl.spanLo[j];
            slUnion.spanHi[k] = rsl.spanHi[j];
            j++;
        }

        bool spanEndFound = false;
        while ( !spanEndFound ) {
            while( i < spanPtr && spanLo[i] <= slUnion.spanHi[k] ) {
                if ( spanHi[i] > slUnion.spanHi[k] ) {
                    slUnion.spanHi[k] = spanHi[i];
                }
                i++;
            }

            spanEndFound = true;
            while( j < rsl.spanPtr && rsl.spanLo[j] <= slUnion.spanHi[k] ) {
                if ( rsl.spanHi[j] > slUnion.spanHi[k] ) {
                    slUnion.spanHi[k] = rsl.spanHi[j];
                    spanEndFound = false;
                }
                j++;
            }
        }
        k++;
    }
    slUnion.spanPtr = k;
    return slUnion;
}

/**
 * @brief GvsSolObjSpanList::operator *
 * @param rsl
 * @return
 */
GvsSolObjSpanList GvsSolObjSpanList::operator*( const GvsSolObjSpanList& rsl ) const {
    GvsSolObjSpanList isect( spanPtr + rsl.spanPtr );

    register int i = 0, j = 0, k = 0;
    while( i < spanPtr && j < rsl.spanPtr ) {
        if ( spanLo[i] <= rsl.spanLo[j] ) {
            if ( spanHi[i] < rsl.spanLo[j] ) {
                i++;
            }
            else if ( spanHi[i] < rsl.spanHi[j] ) {
                isect.spanLo[k] = rsl.spanLo[j];
                isect.spanHi[k] = spanHi[i];
                i++;
                k++;
            }
            else {
                isect.spanLo[k] = rsl.spanLo[j];
                isect.spanHi[k] = rsl.spanHi[j];
                j++;
                k++;
            }
        }
        else {
            if ( spanLo[i] > rsl.spanHi[j] ) {
                j++;
            }
            else if ( spanHi[i] > rsl.spanHi[j] ) {
                isect.spanLo[k] = spanLo[i];
                isect.spanHi[k] = rsl.spanHi[j];
                j++;
                k++;
            }
            else {
                isect.spanLo[k] = spanLo[i];
                isect.spanHi[k] = spanHi[i];
                i++;
                k++;
            }
        }
    }

    isect.spanPtr = k;
    return isect;
}

/**
 * @brief GvsSolObjSpanList::operator -
 * @return
 */
GvsSolObjSpanList GvsSolObjSpanList::operator-() const {
    GvsSolObjSpanList slInv( spanPtr + 1 );

    GvsSurfIntersec minIntersect, maxIntersect;
    minIntersect.setDist( -FLT_MAX );
    maxIntersect.setDist(  FLT_MAX );

    if ( spanPtr == 0 ) {
        slInv.spanLo[0] = minIntersect;
        slInv.spanHi[0] = maxIntersect;
        slInv.spanPtr = 1;
    }
    else {
        int k = 0;

        if ( spanLo[0] > -FLT_MAX ) {
            slInv.spanLo[k] = minIntersect;
            slInv.spanHi[k++] = spanLo[0];
        }

        for ( int i = 1; i < spanPtr; i++ ) {
            slInv.spanLo[k] = spanHi[i-1];
            slInv.spanHi[k++] = spanLo[i];
        }

        if ( spanHi[spanPtr-1] < FLT_MAX ) {
            slInv.spanLo[k] = spanHi[spanPtr-1];
            slInv.spanHi[k++] = maxIntersect;
        }

        slInv.spanPtr = k;
    }

    return slInv;
}


void GvsSolObjSpanList :: Print( FILE* fptr ) {
    fprintf(fptr,"SolObjSpanList {\n");
    for(int i=0; i<spanPtr; i++) {
        fprintf(fptr,"%2d : %f %f\n",i,spanLo[i].dist(),spanHi[i].dist());
    }
    fprintf(fptr,"}\n");
}

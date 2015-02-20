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
#ifndef GVS_LIGHT_SRC_MGR_H
#define GVS_LIGHT_SRC_MGR_H

#include "GvsLightSrc.h"
#include "Img/GvsColor.h"
#include <vector>

typedef std::vector< GvsLightSrc* >  vLightSrc;
typedef std::vector< GvsLightSrc* >::iterator vLightSrcPtr;

class GvsColor;


class GvsLightSrcMgr
{
public:
    GvsLightSrcMgr  ( );
    ~GvsLightSrcMgr ( );

    void     setAmbientLight ( const GvsColor &amb );
    GvsColor ambientLight    () const;

    void     setWithShadowRays( const bool withRays );
    bool     withShadowRays () const;

    void     append ( GvsLightSrc* elem );
    void     remove ( int num );
    void     clear  ( void );

    GvsLightSrc* getLightSrc ( int num, bool &lightFound );
    int          length      ( void  ) const;

    void     Print ( FILE* fptr = stderr );

private:
    vLightSrc     lightSrcList;
    vLightSrcPtr  lightSrcListPtr;
    int           listLength;
    bool          useShadowRays;

protected:
    GvsColor ambLight;
};


inline void GvsLightSrcMgr :: setAmbientLight ( const GvsColor &amb ) {
    ambLight = amb;
}

inline GvsColor GvsLightSrcMgr :: ambientLight () const {
    return ambLight;
}

inline int GvsLightSrcMgr :: length ( void ) const {
    return listLength;
}

inline void GvsLightSrcMgr::setWithShadowRays( const bool withRays ) {
    useShadowRays = withRays;
}

inline bool GvsLightSrcMgr::withShadowRays () const {
    return useShadowRays;
}

#endif

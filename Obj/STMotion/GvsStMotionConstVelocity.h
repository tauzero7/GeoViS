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
#ifndef GVS_STMOTION_CONSTVELOCITY_H
#define GVS_STMOTION_CONSTVELOCITY_H

#include <iostream>
#include <deque>

#include "GvsGlobalDefs.h"
#include "Obj/STMotion/GvsStMotion.h"
#include "Utils/GvsGramSchmidt.h"

#include "metric/m4dMetric.h"

enum GvsMotionCVType
{
    gvsMotionCVnone = -1, gvsMotionCVboost = 0, gvsMotionCVrot
};

struct GvsMotionCVTypeNum
{
    GvsMotionCVType type;
    int             nr;
};


/**
 * @brief The GvsStMotionConstVelocity class
 */
class GvsStMotionConstVelocity : public GvsStMotion
{
public:
    GvsStMotionConstVelocity ( );
    GvsStMotionConstVelocity ( m4d::Metric* spacetime );
    virtual ~GvsStMotionConstVelocity();

    void  setBoost    ( m4d::vec3 boost );
    void  setRotation ( m4d::vec4 rot   );

    m4d::mat4 getBoostMatrix ( const int nr ) const;
    m4d::mat4 getRotMatrix   ( const int nr, const double time = 0.0 ) const;

    GvsMotionCVType  getMotionCVType ( const int nr ) const;

    bool getTransformedPolygon ( const int segNr,
                                 const m4d::vec4& p0in, const m4d::vec4& p1in, m4d::vec4& p0out, m4d::vec4& p1out );

    virtual void  Print ( FILE* fptr = stderr );


protected:
    m4d::Metric*  mMetric;
    int  mNrMotions;
    int  mNrBoosts;
    int  mNrRotations;

    std::vector<GvsMotionCVTypeNum> mMotions;
    std::vector<m4d::vec3> mBoosts;
    std::vector<m4d::vec4> mRotations;
};



#endif

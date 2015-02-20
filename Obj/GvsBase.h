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

#ifndef GVS_BASE_H
#define GVS_BASE_H

//
//  A parameter can be made available as ChangeParameter, if there is a corresponding
//  SetParam implementation in the derived class and an 'AddParam' was set in the
//  constructor
//

#include <iostream>
#include <algorithm>
#include <map>

#include "GvsGlobalDefs.h"
#include "metric/m4dMetric.h"

#include "m4dGlobalDefs.h"
class GvsSceneObj;

/**
 * @brief The GvsBase class
 */
class GvsBase
{
public:
    GvsBase();
    virtual ~GvsBase();

    virtual bool  AddParam ( std::string pName, const GvsDataType type );
    virtual void  DelParam ( std::string pName );
    virtual void  DelAllParam( );

    virtual int   SetParam ( std::string pName, int val );
    virtual bool  GetParam ( std::string pName, int &val );

    virtual int   SetParam ( std::string pName, double val );
    virtual bool  GetParam ( std::string pName, double &val );

    virtual int   SetParam ( std::string pName, m4d::ivec2 vec );
    virtual bool  GetParam ( std::string pName, m4d::ivec2 &vec );

    virtual int   SetParam ( std::string pName, m4d::ivec3 vec );
    virtual bool  GetParam ( std::string pName, m4d::ivec3 &vec );

    virtual int   SetParam ( std::string pName, m4d::ivec4 vec );
    virtual bool  GetParam ( std::string pName, m4d::ivec4 &vec );

    virtual int   SetParam ( std::string pName, m4d::vec2 pt );
    virtual bool  GetParam ( std::string pName, m4d::vec2 &pt );

    virtual int   SetParam ( std::string pName, m4d::vec3 pt );
    virtual bool  GetParam ( std::string pName, m4d::vec3 &pt );

    virtual int   SetParam ( std::string pName, m4d::vec4 pt );
    virtual bool  GetParam ( std::string pName, m4d::vec4 &pt );

    virtual int   SetParam ( std::string pName, m4d::Matrix<double,2,3> mat );
    virtual bool  GetParam ( std::string pName, m4d::Matrix<double,2,3> &mat );

    virtual int   SetParam ( std::string pName, m4d::Matrix<double,3,4> mat );
    virtual bool  GetParam ( std::string pName, m4d::Matrix<double,3,4> &mat );

    virtual int   SetParam ( std::string pName, std::string txt );
    virtual bool  GetParam ( std::string pName, std::string &txt );


    bool  IsValidParamName ( std::string pName );
    bool  IsValidParam ( std::string pName, GvsDataType dataType );


    virtual unsigned int GetNumParams () const;
    virtual std::string  GetParamName ( unsigned int i ) const;
    virtual GvsDataType  GetDataType  ( std::string pName );
    virtual void         PrintAllParameter();


    virtual void  Add ( GvsSceneObj* obj );
    virtual void  Print ( FILE* fptr = stderr ) = 0;

    void        lowCase    ( std::string &s );
    std::string getLowCase ( std::string &s ) const;

    void ResetID() { mID = 0; }
    int  GetID() { return mID; }

protected:
    unsigned int mNumParam;
    std::map<std::string,gvs_parameter>            mParam;
    std::map<std::string,gvs_parameter>::iterator  mParamPtr;

    int mID;
    static int mObjCounter;
};


/**
 * @brief The Gvsm4dMetricDummy class
 *
 *  Necessary to subclass a metric from GvsBase !!
 */
class Gvsm4dMetricDummy : public GvsBase
{
public:
    Gvsm4dMetricDummy (m4d::Metric* cMetric);
    virtual ~Gvsm4dMetricDummy();
    m4d::Metric* m4dMetric;
    virtual int  SetParam ( std::string pName, double val);
    virtual bool GetParam ( std::string pName, double &val);
    virtual void Print( FILE* fptr = stderr );
};

#endif

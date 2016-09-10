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
#ifndef GVS_PICTURE_IO_H
#define GVS_PICTURE_IO_H

#include <GvsGlobalDefs.h>

class GvsChannelImg2D;

/**
 * @brief The PictureIO class is responsible to read and write image data
 *        depending on their file extension.
 */
class API_EXPORT GvsPictureIO
{
public:
    GvsPictureIO(){}
    virtual ~GvsPictureIO() {}

    virtual bool  readChannelImg  ( GvsChannelImg2D& chanImg, const char *filename ) = 0;
    virtual bool  writeChannelImg ( GvsChannelImg2D& chanImg, const char *filename ) = 0;

    static bool  has_extension  ( const char* filename , const char* ext );
    static bool  get_extension  ( const std::string filename, std::string& name, std::string& ext );
    static bool  add_string_ext ( const std::string filename, std::string  name, std::string &newname );
};


#endif

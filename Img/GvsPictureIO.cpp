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

#include "Img/GvsPictureIO.h"
#include "Img/GvsChannelImg2D.h"

bool GvsPictureIO::has_extension ( const char* filename , const char* ext ) {
    size_t lfn  = strlen(filename);
    size_t lext = strlen(ext);

    if (lfn <= lext) {
        return false;
    }

    if ( strcmp( filename + lfn - lext, ext ) == 0 ) {
        return true;
    }
    return false;
}


bool GvsPictureIO::get_extension ( const std::string filename, std::string& name, std::string& ext ) {
    size_t i = filename.rfind(".");

    if (i!=std::string::npos) {
        name.assign(filename,0,i);
        ext.assign(filename,i+1,filename.size());
    } else {
        fprintf(stderr,"File ending is missing\n");
        return false;
    }
    return true;
}


bool GvsPictureIO::add_string_ext ( const std::string filename, std::string name, std::string &newname ) {
    size_t i = filename.rfind(".");

    std::string ext;
    if (i!=std::string::npos) {
        newname.assign(filename,0,i);
        ext.assign(filename,i+1,filename.size());

        newname.append(".");
        newname.append(name);
        newname.append(".");
        newname.append(ext);
    } else {
        fprintf(stderr,"File ending is missing!\n");
        return false;
    }
    return true;
}

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
/**

    Parse parameters for a pinhole camera.
    @verbatim
    (init-camera '(type "PinHoleCam")
                 '(dir  #(double double double) )
                 '(vup  #(double double double) )
                 '(fov  #(double double) )
                 '(res  #(int int) )
                 '(filter "string")
                 '(id "string")
    )@endverbatim
    Direction 'dir' and vertical up-vector 'vup' are with respect to the local reference frame
    of the observer - the projector. 'fov' are given in degrees and image resolution 'res' in
    pixels.


    Parse parameters for a pinhole stereo camera.
    @verbatim
    (init-camera '(type "PinHoleCam")
                 '(dir  #(double double double) )
                 '(vup  #(double double double) )
                 '(fov  #(double double) )
                 '(sep  double)
                 '(res  #(int int) )
                 '(filter "string")
                 '(id "string")
    )@endverbatim
    Direction 'dir' and vertical up-vector 'vup' are with respect to the local reference frame
    of the observer - the projector. 'fov' are given in degrees and image resolution 'res' in
    pixels.


    Parse parameters for a 4pi camera. 'angle' defines the main direction and is to be
    given in degrees.
    @verbatim
    (init-camera '(type "4PICam")
                 '(angle  double)
                 '(res  #(int int)
                 '(filter "string")
                 '(id "string")
    )@endverbatim


    Parse parameters for a 2pi camera. The main viewing direction is defined by
    heading and pitch angles (both given in degrees).
    @verbatim
    (init-camera '(type "2PICam")
                 '(heading  double)
                 '(pitch    double)
                 '(res #(int int))
                 '(filter "string")
                 '(id "string")
    )@endverbatim
    Heading and pitch angles are given in degrees.


    Parse parameters for a panorama camera. The field of view (fov) angles have
    to be given in degrees.
    @verbatim
    (init-camera '(type "PanoramaCam")
                 '(dir  #(double double double) )
                 '(vup  #(double double double) )
                 '(fov  #(double double) )
                 '(res  #(int int) )
                 '(filter "string")
                 '(id "string")
    )@endverbatim
    The parameters are the same as for the pinhole camera.


    Camera filters can be "FilterRGB", "FilterRGBpdz", and "FilterRGBjac".
*/


#include "GvsGlobalDefs.h"

#include "Parser/GvsParseScheme.h"
#include "Parser/parse_camera.h"
#include "Parser/parse_helper.h"

#include "Cam/GvsCamera.h"
#include "Cam/GvsPinHoleCam.h"
#include "Cam/GvsPinHoleStereoCam.h"
#include "Cam/GvsPanoramaCam.h"
#include "Cam/Gvs4PICam.h"
#include "Cam/Gvs2PICam.h"


extern std::vector<GvsCamera*>          gpCamera;
extern std::map<std::string,GvsTypeID>  gpTypeID;

/**
 * @brief gvsP_init_camera
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_camera (scheme *sc, pointer args) {
#ifdef GVS_VERBOSE
    std::cerr << "\n.....gvsP_init_camera.....\n";
#endif
    if (args == sc->NIL) scheme_error("init-camera: no arguments");
    if (!is_pair(args)) scheme_error("init-camera: less arguments");

    std::string allowedNames[] = {"type","id","dir","vup","fov","res","filter","param","angle","heading","pitch","sep"};

    GvsParseAllowedNames allowedTypes[] = {{gp_string_string,0},  // type
                                             {gp_string_string,0},  // id
                                             {gp_string_double,3},  // dir
                                             {gp_string_double,3},  // vup
                                             {gp_string_double,2},  // fov
                                             {gp_string_int,2},     // res
                                             {gp_string_string,0},  // filter
                                             {gp_string_double,1},  // param
                                             {gp_string_double,1},  // angle
                                             {gp_string_double,1},  // heading
                                             {gp_string_double,1},  // pitch
                                             {gp_string_double,1}   // eye sep
                                           };

    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,12);
    args = gvsParser->parse(args);

    std::string cameraType;
    gvsParser->getParameter("type",cameraType);

    if (cameraType =="PinHoleCam") gvsP_init_pinHoleCam(gvsParser);
    else if (cameraType =="PinHoleStereoCam") gvsP_init_pinHoleStereoCam(gvsParser);
    else if (cameraType =="PanoramaCam") gvsP_init_panoramaCam(gvsParser);
    else if (cameraType =="4PICam") gvsP_init_4PICam(gvsParser);
    else if (cameraType =="2PICam") gvsP_init_2PICam(gvsParser);
    else
    {
        std::string msg = cameraType;
        msg.append(": camera is unknown!\n");
        scheme_error(msg);
    }
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtCamera"));
    return R;
}

/**
 * @brief gvsP_init_pinHoleCam
 * @param gP
 */
void gvsP_init_pinHoleCam (GvsParseScheme* gP) {
    gP->testParamNames("init-camera");

    double dir[3] = {-1.0, 0.0, 0.0};
    double vup[3] = { 0.0, 0.0, 1.0};
    double fov[2] = { 60.0, 48.0 };
    int res[2] = {720,576};
    double param = 0.0;

    gP->getParameter("dir",&dir[0]);
    gP->getParameter("vup",&vup[0]);
    gP->getParameter("fov",&fov[0]);
    gP->getParameter("res",&res[0]);


    // Search filter
    GvsCamFilter filter;
    std::string camFilterName;
    bool haveFilter = gP->getParameter("filter",camFilterName);

    if (haveFilter) {
        int camFilter=-1;
        bool filterFound = false;
        while ((camFilter<GvsNumCamFilters-1) && (filterFound==false)) {
            if (camFilterName == GvsCamFilterNames[++camFilter]) filterFound = true;
        }
        if (!filterFound) {
            std::cerr << "This camera filter does not exist!" << std::endl;
            exit(0);
        }
        filter = GvsCamFilter(camFilter);
    }
    else {
        filter = gvsCamFilterRGB;
    }

    GvsPinHoleCam* phCamera = new GvsPinHoleCam(m4d::vec3(dir[0],dir[1],dir[2]),
            m4d::vec3(vup[0],vup[1],vup[2]),
            m4d::vec2(fov[0],fov[1]),
            m4d::ivec2(res[0],res[1]));
    if (haveFilter) phCamera->setCamFilter(filter);

    if (gP->getParameter("param",&param)) {
        phCamera->setParameter(param);
    }

    gpCamera.push_back(phCamera);
    // phCamera->print(cerr);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-camera: ID already exists!");
    }

    GvsTypeID tid = {gtCamera,static_cast<int>(gpCamera.size())-1,gpCamera[gpCamera.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}


/**
 * @brief gvsP_init_pinHoleStereoCam
 * @param gP
 */
void gvsP_init_pinHoleStereoCam (GvsParseScheme* gP) {
    gP->testParamNames("init-camera");

    double dir[3] = {-1.0, 0.0, 0.0};
    double vup[3] = { 0.0, 0.0, 1.0};
    double fov[2] = { 60.0, 48.0 };
    int res[2] = {720,576};
    double sep = 0.1;
    double param = 0.0;

    gP->getParameter("dir",&dir[0]);
    gP->getParameter("vup",&vup[0]);
    gP->getParameter("fov",&fov[0]);
    gP->getParameter("res",&res[0]);
    gP->getParameter("sep", &sep);

    // Search filter
    GvsCamFilter filter;
    std::string camFilterName;
    bool haveFilter = gP->getParameter("filter",camFilterName);

    if (haveFilter) {
        int camFilter=-1;
        bool filterFound = false;
        while ((camFilter<GvsNumCamFilters-1) && (filterFound==false)) {
            if (camFilterName == GvsCamFilterNames[++camFilter]) filterFound = true;
        }
        if (!filterFound) {
            std::cerr << "This camera filter does not exist!" << std::endl;
            exit(0);
        }
        filter = GvsCamFilter(camFilter);
    }
    else {
        filter = gvsCamFilterRGB;
    }

    GvsPinHoleStereoCam* phCamera = new GvsPinHoleStereoCam(m4d::vec3(dir[0],dir[1],dir[2]),
            m4d::vec3(vup[0],vup[1],vup[2]),
            m4d::vec2(fov[0],fov[1]),
            m4d::ivec2(res[0],res[1]),
            sep);
    if (haveFilter) phCamera->setCamFilter(filter);

    if (gP->getParameter("param",&param)) {
        phCamera->setParameter(param);
    }

    gpCamera.push_back(phCamera);
    // phCamera->print(cerr);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-camera: ID already exists!");
    }

    GvsTypeID tid = {gtCamera,static_cast<int>(gpCamera.size())-1,gpCamera[gpCamera.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}



/**
 * @brief gvsP_init_panoramaCam
 * @param gP
 */
void gvsP_init_panoramaCam( GvsParseScheme* gP ) {
    gP->testParamNames("init-camera");

    double dir[3] = {-1.0, 0.0, 0.0};
    double vup[3] = { 0.0, 0.0, 1.0};
    double fov[2] = { 60.0, 48.0 };
    int res[2] = {720,576};
    double param;

    gP->getParameter("dir",&dir[0]);
    gP->getParameter("vup",&vup[0]);
    gP->getParameter("fov",&fov[0]);
    gP->getParameter("res",&res[0]);


    // Search filter
    GvsCamFilter filter;
    std::string camFilterName;
    bool haveFilter = gP->getParameter("filter",camFilterName);

    if (haveFilter) {
        int camFilter=-1;
        bool filterFound = false;
        while ((camFilter<=GvsNumCamFilters) && (filterFound==false)) {
            if (camFilterName == GvsCamFilterNames[++camFilter]) filterFound = true;
        }
        if (!filterFound) {
            std::cerr << "This filter does not exist!" << std::endl;
            exit(0);
        }
        filter = GvsCamFilter(camFilter);
    }
    else {
        filter = gvsCamFilterRGB;
    }

    GvsPanoramaCam* phCamera = new GvsPanoramaCam(m4d::vec3(dir[0],dir[1],dir[2]),
            m4d::vec3(vup[0],vup[1],vup[2]),
            m4d::vec2(fov[0],fov[1]),
            m4d::ivec2(res[0],res[1]));
    if (haveFilter) phCamera->setCamFilter(filter);
    if (gP->getParameter("param",&param)) {
        phCamera->setParameter(param);
    }

    gpCamera.push_back(phCamera);
    // phCamera->print(cerr);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-camera: ID schon vergeben!");
    }

    GvsTypeID tid = { gtCamera,static_cast<int>(gpCamera.size())-1,gpCamera[gpCamera.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}

/**
 * @brief gvsP_init_4PICam
 * @param gP
 */
void gvsP_init_4PICam (GvsParseScheme* gP)
{
    gP->testParamNames("init-camera");

    double angle = 0.0;
    int res[2] = {720,576};

    double param = 0.0;
    gP->getParameter("angle",&angle);
    gP->getParameter("res",&res[0]);


    // Search filter
    GvsCamFilter filter;
    std::string camFilterName;
    bool haveFilter = gP->getParameter("filter",camFilterName);

    if (haveFilter) {
        int camFilter=-1;
        bool filterFound = false;
        while ((camFilter<=GvsNumCamFilters) && (filterFound==false)) {
            if (camFilterName == GvsCamFilterNames[++camFilter]) filterFound = true;
        }
        if (!filterFound) {
            std::cerr << "This filter does not exist!" << std::endl;
            exit(0);
        }
        filter = GvsCamFilter(camFilter);
    }
    else {
        filter = gvsCamFilterRGB;
    }

    Gvs4PICam* phCamera = new Gvs4PICam(angle,
                                        m4d::ivec2(res[0],res[1]));
    if (haveFilter) phCamera->setCamFilter(filter);
    if (gP->getParameter("param",&param)) {
        phCamera->setParameter(param);
    }

    gpCamera.push_back(phCamera);
    // phCamera->print(cerr);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if(gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-camera: ID already assigned!");
    }

    GvsTypeID tid = {gtCamera,static_cast<int>(gpCamera.size())-1,gpCamera[gpCamera.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}

/**
 * @brief gvsP_init_2PICam
 * @param gP
 */
void gvsP_init_2PICam( GvsParseScheme* gP ) {
    gP->testParamNames("init-camera");

    double pitch = 0.0;
    double heading = 0.0;
    int res[2] = {100,100};

    gP->getParameter("heading",&heading);
    gP->getParameter("pitch",&pitch);
    gP->getParameter("res",&res[0]);

    GvsCamFilter filter;
    std::string camFilterName;
    bool haveFilter = gP->getParameter("filter",camFilterName);

    if (haveFilter)
    {
        int camFilter=-1;
        bool filterFound = false;
        do
        {
            if (camFilterName == GvsCamFilterNames[++camFilter]) filterFound = true;
        }
        while ((camFilter<GvsNumCamFilters) && (filterFound==false));
        if (!filterFound) {
            fprintf(stderr,"The filter \"%s\" does not exist!",camFilterName.c_str());
            exit(0);
        }

        filter = GvsCamFilter(camFilter);
    }
    else
    {
        filter = gvsCamFilterRGB;
    }

    Gvs2PICam* phCamera = new Gvs2PICam(heading,pitch,res[0]);
    if (haveFilter) phCamera->setCamFilter(filter);

    gpCamera.push_back(phCamera);
    // phCamera->print(cerr);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-camera: ID already assigned!");
    }

    GvsTypeID tid = {gtCamera,static_cast<int>(gpCamera.size())-1,gpCamera[gpCamera.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}

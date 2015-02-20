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
    The projector represents the local observer sitting in the local reference frame
    defined by "locTedObs".
    @verbatim
    (init-projector '(localTetrad "locTedObs")
                   ['(color  #(0.1 0.1 0.1)) ]
                    '(id "proj")
    )@endverbatim
    The default background color is black (0,0,0).


    The local tetrad could also be defined within the initialization of the projector:
    @verbatim
    (init-projector '(pos  #(0.0 10.0 1.5708 0.0 0.0))
                  [ '(e0  #(1.0 0.0 0.0 0.0)) ]
                  [ '(e1  #(0.0 1.0 0.0 0.0)) ]
                  [ '(e2  #(0.0 0.0 1.0 0.0)) ]
                  [ '(e3  #(0.0 0.0 0.0 1.0)) ]
                  [ '(incoords #f) ]
                  ['(color  #(0.1 0.1 0.1)) ]
    )@endverbatim
    If incoords = false, then the local tetrad e0-e3 is given with respect to
    the natural local tetrad.
*/

#include "Parser/parse_projector.h"
#include "Parser/parse_helper.h"

#include "Dev/GvsProjector.h"
#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Obj/STMotion/GvsStMotion.h"

//#include <Dev/GvsProjector2PI.h>

#include "GvsParseScheme.h"

extern std::vector<GvsLocalTetrad*>    gpLocalTetrad;
extern std::vector<GvsRayGen*>         gpRayGen;
extern std::vector<GvsProjector*>      gpProjector;

extern std::vector<GvsStMotion*>       gpMotion;

extern std::map<std::string,GvsTypeID>           gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;

/**
 * @brief gvsP_init_projector
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_projector (scheme *sc, pointer args) {
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_init_projector..........\n";
#endif
    if (args == sc->NIL) scheme_error("init-projector: no arguments");
    if (!is_pair(args)) scheme_error("init-projector: less arguments");

    GvsProjector*    currProj = NULL;
    GvsLocalTetrad*  locTed = NULL;
    GvsStMotion*     currMotion = NULL;
    //GvsRayGen*       currRayGen = NULL;

    std::string allowedNames[] = {
        "raygen","localtetrad","color","id","pos","e0","e1","e2","e3","incoords","motion",
        "errcolor","cstrcolor","breakcolor"
    };
    GvsParseAllowedNames allowedTypes[] = {{gp_string_string,0}, // raygen
                                           {gp_string_string,0}, // localtetrad
                                           {gp_string_double,3}, // color
                                           {gp_string_string,0}, // id
                                           {gp_string_double,5}, // pos
                                           {gp_string_double,4}, // e0
                                           {gp_string_double,4}, // e1
                                           {gp_string_double,4}, // e2
                                           {gp_string_double,4}, // e3
                                           {gp_string_bool,0},   // incoords
                                           {gp_string_string,0}, // motion
                                           {gp_string_double,3}, // error color
                                           {gp_string_double,3}, // constraint color
                                           {gp_string_double,3}  // break down color
                                          };

    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,14);

    bool haveLocTed = false;
    bool haveMotion = false;


    // Read arguments of projector
    args = gvsParser->parse(args);

    std::string msg;

    // Set ray generator
    std::string rayGenID;
    if (gvsParser->getParameter("raygen",rayGenID)) {
        if (rayGenID=="gtRayGen") {
            currProj = new GvsProjector(gpRayGen[gpRayGen.size()-1]);
        }
        else
        {
            if (gpRayGen.size()>=1)
            {
                gpTypeIDptr = getIDptr(gvsParser,"init-projector","RayGen","raygen",gtRayGen);
                currProj = new GvsProjector(gpRayGen[(gpTypeIDptr->second).vectorID]);
            } else {
                scheme_error("init-projector: no ray generator found!");
            }
        }
    } else {
        if (gpRayGen.empty())  scheme_error("init-projector: RayGen is missing!\n");
        else if (gpRayGen.size()>1) {
            scheme_error("init-projector: RayGen-ID is missing!\n");
        }
        else {
            currProj = new GvsProjector(gpRayGen[0]);
        }
#ifdef GVS_VERBOSE
        printf("Projector: use entered raygen!\n");
#endif
    }

    assert(currProj!=NULL);

    // Determine local tetrad either directly as local tetrad od by position
    double pos[4] = {0.0, 1.0, 0.0, 0.0 };

    std::string locTedID;
    if (gvsParser->getParameter("localtetrad",locTedID)) {
        if (locTedID=="gtLocTed") {
            locTed = gpLocalTetrad[gpLocalTetrad.size()-1];
            haveLocTed = true;
        } else {
            if (gpLocalTetrad.size()>=1) {
                getIDptr(gvsParser,"init-projector","local tetrad","localtetrad",gtLocTed);
                locTed = gpLocalTetrad[(gpTypeIDptr->second).vectorID];
                currProj->setLocalTetrad(locTed);
                haveLocTed = true;
            }
        }
    } else if (gvsParser->getParameter("pos",&pos[0])) {
        currProj->setPosition(m4d::vec4(pos[0],pos[1],pos[2],pos[3]));
        double e0[4] = { 1.0, 0.0, 0.0, 0.0 };
        double e1[4] = { 0.0, 1.0, 0.0, 0.0 };
        double e2[4] = { 0.0, 0.0, 1.0, 0.0 };
        double e3[4] = { 0.0, 0.0, 0.0, 1.0 };
        bool inCoords = false;
        gvsParser->getParameter("e0",&e0[0]);
        gvsParser->getParameter("e1",&e1[0]);
        gvsParser->getParameter("e2",&e2[0]);
        gvsParser->getParameter("e3",&e2[0]);
        gvsParser->getParameter("incoords",inCoords);

        haveLocTed = true;

        std::string locFrame;
        if (gvsParser->getParameter("localFrame",locFrame)) {
            for(int lf=0; lf<3; lf++) {
                //if (GvsLFTypeName[lf]==locFrame) locTed->setInCoords(false,(GvsLFType)lf);
                if (std::string(m4d::stl_nat_tetrad_types[lf])==locFrame)
                    locTed->setInCoords(false,(m4d::enum_nat_tetrad_type)lf);
            }
        }

        currProj->setTetrad(m4d::vec4(e0[0],e0[1],e0[2],e0[3]),
                m4d::vec4(e1[0],e1[1],e1[2],e1[3]),
                m4d::vec4(e2[0],e2[1],e2[2],e2[3]),
                m4d::vec4(e3[0],e3[1],e3[2],e3[3]),
                inCoords);
    }

    // instead of a local tetrad, a motion could also be entered
    std::string motionID;
    if (gvsParser->getParameter("motion",motionID)) {
        gpTypeIDptr = gpTypeID.find(motionID);
        if (gpTypeIDptr != gpTypeID.end()) {
            currMotion = gpMotion[(gpTypeIDptr->second).vectorID];
            haveMotion = true;
        } else {
            msg = "init-projector";
            msg.append(": object with ID \"");
            msg.append(motionID);
            msg.append("\" is no motion!");
            scheme_error(msg);
        }
    }


    if (haveMotion) currProj->setMotion(currMotion);
    // std::cerr << "hier: " << currMotion->getNumPositions() << std::endl;
    // currProj->Print();
    // currProj->getLocalTetrad()->printP();    exit(1);

    // Set background color
    double color[3] = { 0.0, 0.0, 0.0 };
    if (gvsParser->getParameter("color",&color[0])) {
        currProj->setBackgroundColor(GvsColor(color[0],color[1],color[2]));
    }
    
    double errorcolor[3] = { 0.0, 0.0, 0.0 };
    if (gvsParser->getParameter("errcolor",&errorcolor[0])) {
        currProj->setErrorColor(GvsColor(errorcolor[0],errorcolor[1],errorcolor[2]));
    }

    double cstrcolor[3] = { 0.0, 0.0, 0.0 };
    if (gvsParser->getParameter("cstrcolor",&cstrcolor[0])) {
        currProj->setConstraintColor(GvsColor(cstrcolor[0],cstrcolor[1],cstrcolor[2]));
    }

    double bdcolor[3] = { 0.0, 0.0, 0.0 };
    if (gvsParser->getParameter("breakcolor",&bdcolor[0])) {
        currProj->setBreakDownColor(GvsColor(bdcolor[0],bdcolor[1],bdcolor[2]));
    }

    if ((!haveLocTed) && (!haveMotion)) {
        msg = "init-projector: ";
        msg.append(locTedID);
        msg.append(" neither a local tetrad nor a motion is given!");
        scheme_error(msg);
    }

    gpProjector.push_back(currProj);

    std::string idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    } else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-projector: ID already assigned!");
    }

    GvsTypeID tid = {gtProjector,static_cast<int>(gpProjector.size())-1,gpProjector[gpProjector.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtProjector"));
    return R;
}


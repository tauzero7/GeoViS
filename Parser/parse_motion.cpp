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
        (init-motion '(type "Geodesic")
                     '(solver "gsolverID")
                     `(pos ,(vector 0.0 r_obj 1.5707963 0.0 ))
                     `(localvel ,(vector 0.0 0.0 lvel))
                     '(e0 #(1.0 0.0 0.0 0.0))
                     '(e1 #(0.0 0.0 -1.0 0.0))
                     '(e2 #(0.0 0.0 0.0 -1.0))
                     '(e3 #(0.0 1.0 0.0 0.0))
                     '(maxnumpoints 1000)
                     '(forward  500.0)
                     '(backward 500.0)
                     '(id "motion")
        )


        (gvs-print-motion '(id "motion")
                        [ '(file "output.dat") ]
        )
 */

#include <Parser/parse_motion.h>
#include <Parser/parse_helper.h>
#include "GvsParseScheme.h"


#include "metric/m4dMetric.h"
#include "Utils/GvsGeodSolver.h"

#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Obj/STMotion/GvsStMotion.h"
#include "Obj/STMotion/GvsStMotionGeodesic.h"
#include "Obj/STMotion/GvsStMotionConstVelocity.h"
//#include <Obj/STMotion/GvsStMotionWorldline.h>

extern std::vector<GvsGeodSolver*>     gpSolver;
extern std::vector<Gvsm4dMetricDummy*> gpMetric;
extern std::vector<GvsLocalTetrad*>    gpLocalTetrad;
extern std::vector<GvsStMotion*>       gpMotion;

extern std::map<std::string,GvsTypeID>           gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;

/**
 * @brief gvsP_init_motion
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_motion (scheme *sc, pointer args) {
#ifdef GVS_VERBOSE
    std::cerr << "\n.....gvsP_init_motion.....\n";
#endif

    if (args == sc->NIL) scheme_error("init-motion: no arguments");
    if (!is_pair(args)) scheme_error("init-motion: less arguments");

    std::string allowedNames[15] = {"type","id","pos","vel","localvel","e0","e1","e2","e3",
                                    "e0loc","e1loc","e2loc","e3loc","tau0","metric"
                                   };

    GvsParseAllowedNames allowedTypes[15] = {{gp_string_string,0}, // type
                                             {gp_string_string,0}, // id
                                             {gp_string_double,4}, // pos
                                             {gp_string_double,4}, // vel
                                             {gp_string_double,3}, // localvel
                                             {gp_string_double,4}, // e0
                                             {gp_string_double,4}, // e1
                                             {gp_string_double,4}, // e2
                                             {gp_string_double,4}, // e3
                                             {gp_string_double,4}, // e0loc
                                             {gp_string_double,4}, // e1loc
                                             {gp_string_double,4}, // e2loc
                                             {gp_string_double,4}, // e3loc
                                             {gp_string_double,1}, // tau0
                                             {gp_string_string,0}
                                            };

    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,15);
    args = gvsParser->parse(args);

    std::string motionType;
    gvsParser->getParameter("type",motionType);

    if (motionType == "Geodesic")           gvsP_init_geodMotion(gvsParser);
    else if (motionType == "ConstVelocity") gvsP_init_constMotion(gvsParser);
    else if (motionType == "file" )         gvsP_init_readMotion(gvsParser);

    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtMotion"));
    return R;
}


/**
 * @brief gvsP_init_geodMotion
 * @param gP
 */
void gvsP_init_geodMotion (GvsParseScheme* gP) {
    gP->setAllowedName("localtetrad",gp_string_string,0);
    gP->setAllowedName("localframe",gp_string_string,0);
    gP->setAllowedName("forward",gp_string_double,1);
    gP->setAllowedName("backward",gp_string_double,1);
    gP->setAllowedName("maxnumpoints",gp_string_int,1);
    gP->setAllowedName("solver",gp_string_string,0);
    gP->setAllowedName("stepsize",gp_string_double,1);

    gP->testParamNames("init-motion");

    double pos[4]  = {0.0, 6.0, M_PI_2, 0.0};
    double vel[4]  = {0.0, 0.0, 0.0, 0.0};
    double locVel[3] = {0.0, 0.0, 0.0};
    m4d::enum_nat_tetrad_type lfType = m4d::enum_nat_tetrad_default;
    std::string lfTypeName = "GVS_LF";
    double e0[4]   = {1.0, 0.0, 0.0, 0.0};
    double e1[4]   = {0.0, 1.0, 0.0, 0.0};
    double e2[4]   = {0.0, 0.0, 1.0, 0.0};
    double e3[4]   = {0.0, 0.0, 0.0, 1.0};
    double tau0    = 0.0;
    int maxNumPoints = 1000;

    UNUSED_ATTR  bool havePos     = false;
    UNUSED_ATTR  bool haveVel     = false;
    bool haveLocVel  = false;
    bool haveLFType  = false;
    bool haveTetrad  = false;
    UNUSED_ATTR  bool haveTau0    = false;

    UNUSED_ATTR bool haveE = false;     // tetrad is given in coordinates
    bool haveEloc = false;              // tetrad is given wrt local tetrad

    double dtForward  = 1.0;
    double dtBackward = 1.0;

    GvsGeodSolver* currSolver;
    std::string solverID;
    if (gP->getParameter("solver",solverID)) {
        if (solverID=="gtGeodSolver") {
            currSolver = gpSolver[gpSolver.size()-1];
        }
        else {
            if (gpSolver.size()>=1) {
                gpTypeIDptr = getIDptr(gP,"init-motion","GeodSolver","solver",gtGeodSolver);
                currSolver = gpSolver[(gpTypeIDptr->second).vectorID];
            }
            else {
                scheme_error("init-motion: no GeodSolver available");
            }
        }
    }
    else {
        if (gpSolver.empty()) scheme_error("init-motion: no GeodSolver availabla!");
        else if (gpSolver.size()>1) {
            scheme_error("init-motion: Solver-ID is missing!\n");
        }
        else {
            currSolver = gpSolver[0];
        }
#ifdef GVS_VERBOSE
        printf("init-motion: verwende eingegebenen GeodSolver\n");
#endif
    }


    m4d::Metric* currMetric = readMetric("init-motion",gP);

    GvsStMotionGeodesic* motion = new GvsStMotionGeodesic(currMetric,currSolver);
    double stepsize;
    if (gP->getParameter("stepsize",stepsize))
        motion->setLambdaStep(stepsize);

    GvsLocalTetrad* currLocTed;
    std::string locTedID;
    haveTetrad = gP->getParameter("localtetrad",locTedID);
    if ((haveTetrad) && (locTedID=="gtLocTed"))
    {
        currLocTed = gpLocalTetrad[gpLocalTetrad.size()-1];
        currLocTed->adjustTetrad();
    }
    // fehlt noch was!!

    if (!haveTetrad)
    {
        havePos    = gP->getParameter("pos",pos);
        haveVel    = gP->getParameter("vel",vel);
        haveLocVel = gP->getParameter("localvel",locVel);

        haveLFType = gP->getParameter("localframe",lfTypeName);
        if (haveLFType) {
            int num = searchCharArray((const char**)&m4d::stl_nat_tetrad_types[0],m4d::NUM_ENUM_NAT_TETRAD_TYPES,lfTypeName.c_str());
            if (num == -1)
                scheme_error("init-motion: localframe-Typ nicht vorhanden!");

            lfType = (m4d::enum_nat_tetrad_type)num;
        }

        // read velocity
        m4d::vec4 velocity;
        if (haveLocVel)  {
            //cerr << "parse-motion: locVel: " << locVel[0] << " " << locVel[1] << " " << locVel[2] << std::endl;
            velocity = m4d::vec4(0.0,locVel[0],locVel[1],locVel[2]);
            motion->setStartVal(m4d::vec4(pos[0],pos[1],pos[2],pos[3]), velocity, false, lfType);
        }
        else {
            velocity = m4d::vec4(vel[0],vel[1],vel[2],vel[3]);
            motion->setStartVal(m4d::vec4(pos[0],pos[1],pos[2],pos[3]), velocity, true);
        }

        haveE = gP->getParameter("e0",e0);
        haveE = gP->getParameter("e1",e1);
        haveE = gP->getParameter("e2",e2);
        haveE = gP->getParameter("e3",e3);
        haveEloc = gP->getParameter("e0loc",e0);
        haveEloc = gP->getParameter("e1loc",e1);
        haveEloc = gP->getParameter("e2loc",e2);
        haveEloc = gP->getParameter("e3loc",e3);
        haveTau0 = gP->getParameter("tau0",tau0);
        //     printf("!!!!!!!!!!!!!!!!!!!!!!!!! TAU 0 found: %f\n",tau0);

        // The local tetrad is transformed to coordinate representation for calculating motions.
        bool inCoords = false;
        if (haveEloc) inCoords = true;
        motion->setStartTetrad(m4d::vec4(e0[0],e0[1],e0[2],e0[3]),
                m4d::vec4(e1[0],e1[1],e1[2],e1[3]),
                m4d::vec4(e2[0],e2[1],e2[2],e2[3]),
                m4d::vec4(e3[0],e3[1],e3[2],e3[3]),
                inCoords,lfType);
    }

    // maximum number of points to be calculated
    if (gP->getParameter("maxnumpoints",maxNumPoints)) {
        motion->setMaxNumPoints(maxNumPoints);
    }

    if (gP->getParameter("forward",dtForward)) {
#ifdef GVS_VERBOSE
        printf("init-motion: calculateGeodesicMotion forward: %f\n",dtForward);
#endif
        motion->calculateGeodesicMotion(m4d::enum_time_forward,dtForward,tau0,stepsize);
    }

    if (gP->getParameter("backward",dtBackward)) {
#ifdef GVS_VERBOSE
        printf("init-motion: calculateGeodesicMotion backward: %f\n",dtBackward);
#endif
        motion->calculateGeodesicMotion(m4d::enum_time_backward,dtBackward,tau0,stepsize);
    }
    //motion->printAll(cerr);

    gpMotion.push_back(motion);


    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-motion: ID already assigned!");
    }

    GvsTypeID tid = {gtMotion,static_cast<int>(gpMotion.size())-1,gpMotion[gpMotion.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}


/**
 * @brief gvsP_init_constMotion
 * @param gP
 */
void gvsP_init_constMotion ( GvsParseScheme* gP ) {
    gP->setAllowedName("boost",gp_string_double,3);
    gP->setAllowedName("rotation",gp_string_double,4);

    gP->testParamNames("init-motion");

    double boost[3];
    double rot[4];

    GvsStMotionConstVelocity* motion = new GvsStMotionConstVelocity();

    //int numBoosts = 0;
    //int numRots   = 0;

    int testNum,num = 0;
    //std::string motionID;

    do {
        testNum = num;
        if (gP->getParameter("boost",boost,testNum)) {
            motion->setBoost(m4d::vec3(boost[0],boost[1],boost[2]));
            num=testNum;
        }
        if (gP->getParameter("rotation",rot,testNum)) {
            if ((rot[1]==0.0) && (rot[2]==0.0) && (rot[3]==0.0)) {
                scheme_error("init-motion: rotation axis is missing!\n");
            }
            motion->setRotation(m4d::vec4(rot[0],rot[1],rot[2],rot[3]));
            num=testNum;
        }
        num++;
    }
    while ( num < gP->getNumParam() );
    //motion->Print();

    gpMotion.push_back(motion);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-motion: ID already assigned!");
    }

    GvsTypeID tid = {gtMotion,static_cast<int>(gpMotion.size())-1,gpMotion[gpMotion.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}


void gvsP_init_readMotion( GvsParseScheme* gP )
{  
    gP->setAllowedName("filename",gp_string_string,0);
    gP->setAllowedName("havetau",gp_string_bool,0);

    gP->testParamNames("init-motion");

    std::string filename;
    if (gP->getParameter("filename",filename))
    {
        printf("init-motion: reading from motion file %s\n",filename.c_str());
    }
    bool haveTau = false;
    gP->getParameter("havetau",haveTau);

    // Einlesen der Raumzeit
    //m4d::Metric* currMetric = readMetric("init-motion",gP);

    /*
  double tau0      = 0.0;  //TODO
  GvsStMotionWorldline* motion = new GvsStMotionWorldline(currSpacetime);
  motion->setGeneralMotion(filename,haveTau);
  //motion->printAll(cout);
  gpMotion.push_back(motion);
*/



    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-motion: ID already assigned!");
    }

    GvsTypeID tid = {gtMotion,static_cast<int>(gpMotion.size())-1,gpMotion[gpMotion.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}


/**
 * @brief gvsP_print_motion
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_print_motion (scheme *sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_print_motion..........\n";
#endif
    if (args == sc->NIL) scheme_error("gvs-print-motion: no arguments");
    if (!is_pair(args)) scheme_error("gvs-print-motion: less arguments");

    std::string allowedNames[2] = {"id","file"};
    GvsParseAllowedNames allowedTypes[3] = {{gp_string_string,0}, // id
                                            {gp_string_string,0}  // files
                                           };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,2);
    args = gvsParser->parse(args);

    std::string idName,msg;
    if (!gvsParser->getParameter("id",idName)) {
        scheme_error("gvs-print-motion: Id is missing!");
    }

    // Search ID in object list.
    gpTypeIDptr = gpTypeID.find(idName);

    if (gpTypeIDptr == gpTypeID.end()) {
        msg = "ID: ";
        msg.append(idName);
        msg.append(" not available!");
        scheme_error(msg);
    }

    // Test if the ID points to a motion
    if ((gpTypeIDptr->second).gvsType != gtMotion) {
        msg = "IT: ";
        msg.append(idName);
        msg.append(" is no motion-ID!");
        scheme_error(msg);
    }

    GvsStMotion* gvsObject = (GvsStMotion*)(gpTypeIDptr->second).gvsObject;

    std::string filename;
    if (gvsParser->getParameter("file",filename)) {
        char file[128];
        strcpy(file,filename.c_str());

        gvsObject->PrintToFile(file);
    }
    else {
        gvsObject->PrintAll();
    }

    delete gvsParser;
    return sc->NIL;
}

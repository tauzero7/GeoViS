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
    Initializing a ray generator can be done in the following way. Arguments in brackets [ ] are optional.
    @verbatim
    (init-raygen '(type "RayGenSimple")
                 '(maxNumPoints  3000     )
             [   '(solver        "solver1")  ]
             [   '(boundBoxLL  #( (- dblmax) -50.0 -50.0 -50.0))  ]
             [   '(boundBoxUR  #(  dblmax   50.0  50.0  50.0))  ]
    )@endverbatim

    @verbatim
    (calc-ray '(filename "points.dat")
              '(pos      #(0.0 10.0 1.5708 0.0 0))
              '(dir      #(0.0 -1.0 0.0 0.0))
              '(raygen   "rayGenID")
    )@endverbatim

    @verbatim
    (calc-proj-ray '(filename  "punkte.dat")
                   '(projector "proj")
                   '(dir       #(-1.0 0.0 0.0))
                   '(raygen    "raygen")
                   '(cartesian #t)
    )@endverbatim
*/


#include "parse_raygen.h"
#include "parse_helper.h"

#include <GvsGlobalDefs.h>

#include "Dev/GvsProjector.h"
#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Parser/GvsParseScheme.h"
#include "Ray/GvsRayGen.h"
#include "Ray/GvsRayVisual.h"
#include "Utils/GvsGeodSolver.h"


#include <metric/m4dMetric.h>


#include <fstream>

extern std::vector<GvsRayGen*>         gpRayGen;
extern std::vector<GvsGeodSolver*>     gpSolver;
extern std::vector<Gvsm4dMetricDummy*> gpMetric;
extern std::vector<GvsLocalTetrad*>    gpLocalTetrad;
extern std::vector<GvsProjector*>      gpProjector;

extern std::map<std::string,GvsTypeID>           gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;

/**
 * @brief gvsP_init_raygen
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_raygen (scheme *sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_init_raygen..........\n";
#endif
    if (args == sc->NIL) scheme_error("init-raygen: no arguments");
    if (!is_pair(args)) scheme_error("init-raygen: less arguments");

    std::string allowedNames[] = {"type","solver","maxnumpoints","boundBoxLL","boundBoxUR","id"};
    GvsParseAllowedNames allowedTypes[] = {{gp_string_string,0}, // type
                                           {gp_string_string,0}, // solver
                                           {gp_string_int,1},    // maxnumpoints
                                           {gp_string_double,4}, // boundBoxLL
                                           {gp_string_double,4}, // boundBoxUR
                                           {gp_string_string,0}  // id
                                          };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,6);
    args = gvsParser->parse(args);

    std::string raygenType;
    gvsParser->getParameter("type",raygenType);

    if (raygenType == "RayGenSimple") {
        gvsP_init_raygenSimple(gvsParser);
    } else {
        fprintf(stderr,"Ray generator %s is unknown.\n",raygenType.c_str());
    }

    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtRayGen"));
    return R;
}


/**
 * @brief gvsP_init_raygenSimple
 * @param gP
 */
void gvsP_init_raygenSimple (GvsParseScheme* gP) {
    GvsGeodSolver* currSolver;

    std::string solverID;
    if (gP->getParameter("solver",solverID))  {

        gpTypeIDptr = gpTypeID.find(solverID);
        if (gpTypeIDptr != gpTypeID.end()) {
            currSolver = gpSolver[(gpTypeIDptr->second).vectorID];
        }
        else {
            if (gpSolver.size()>=1) {
                getIDptr(gP,"RayGenSimple","Solver","solver",gtGeodSolver);
                currSolver = gpSolver[(gpTypeIDptr->second).vectorID];
            }
            else {
                scheme_error("RayGen: no solver available!");
            }
        }
    }
    else {
        if (gpSolver.empty()) {
            scheme_error("RayGen: no solver available!");
        }
        else if (gpSolver.size()>1) {
            getIDptr(gP,"RayGenSimple","Solver","solver",gtGeodSolver);
            currSolver = gpSolver[(gpTypeIDptr->second).vectorID];
        }
        else
            currSolver = gpSolver[0];
#ifdef GVS_VERBOSE
        printf("RayGenSimple: verwende eingegebenen GeodSolver\n");
#endif
    }

    GvsRayGen* currRayGen = new GvsRayGen(currSolver);

    int maxNumPoints = 3000;
    if (gP->getParameter("maxnumpoints",maxNumPoints)) currRayGen->setMaxNumPoints(maxNumPoints);

    double boundBoxLL[4] = {-DBL_MAX,-50.0,-50.0,-50.0};
    double boundBoxUR[4] = { DBL_MAX, 50.0, 50.0, 50.0};

    bool haveBoxLL = gP->getParameter("boundBoxLL",&boundBoxLL[0]);
    bool haveBoxUR = gP->getParameter("boundBoxUR",&boundBoxUR[0]);
    if (haveBoxLL || haveBoxUR)
    {
        currRayGen->setBoundBox(GvsBoundBox4D(m4d::vec4(boundBoxLL[0],boundBoxLL[1],boundBoxLL[2],boundBoxLL[3]),
                m4d::vec4(boundBoxUR[0],boundBoxUR[1],boundBoxUR[2],boundBoxUR[3])));
    }

    gpRayGen.push_back(currRayGen);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-raygen: ID already assigned!");
    }

    GvsTypeID tid = {gtRayGen,static_cast<int>(gpRayGen.size())-1,gpRayGen[gpRayGen.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}

/**
 * @brief gvsP_calc_ray
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_calc_ray ( scheme *sc, pointer args )
{
    if (args == sc->NIL) scheme_error("calc-ray: no arguments");
    if (!is_pair(args)) scheme_error("calc-ray: less arguments");

    std::string allowedNames[5] = {"filename","raygen","locted","pos","dir"};
    GvsParseAllowedNames allowedTypes[5] = {{gp_string_string,0}, // filename
                                            {gp_string_string,0}, // raygen
                                            {gp_string_string,0}, // locted
                                            {gp_string_double,4}, // pos
                                            {gp_string_double,2}  // dir  (ksi,chi)
                                           };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,5);
    args = gvsParser->parse(args);

    // set ray generator
    GvsRayGen* rayGen;
    std::string rayGenID;
    if (gpRayGen.size()>1) {
        if (!gvsParser->getParameter("raygen",rayGenID))
            scheme_error("calc-ray: there are more raygen available; ID is missing");

        gpTypeIDptr = gpTypeID.find(rayGenID);
        std::string msg;
        if (gpTypeIDptr == gpTypeID.end()) {
            msg = "raygen with ID: ";
            msg.append(rayGenID);
            msg.append(" is not available!");
            scheme_error(msg);
        }

        if ( (gpTypeIDptr->second).gvsType != gtRayGen ) {
            msg = rayGenID;
            msg.append(" is no raygen!");
            scheme_error(msg);
        }
        rayGen = gpRayGen[(gpTypeIDptr->second).vectorID];
    }
    else {
        if (gpRayGen.empty()) {
            scheme_error("calc-ray: raygen is missing\n");
        }
#ifdef GVS_VERBOSE
        printf("calc-ray: verwende eingegebenen RayGen\n");
#endif
        rayGen = gpRayGen[0];
    }

    // set local tetrad
    GvsLocalTetrad* locTed;
    std::string locTedID;
    if (gpLocalTetrad.size()>1) {
        if (!gvsParser->getParameter("locted",locTedID))
            scheme_error("calc-ray: there are more locted availabe; ID is missing.");

        gpTypeIDptr = gpTypeID.find(locTedID);
        std::string msg;
        if (gpTypeIDptr == gpTypeID.end()) {
            msg = "locted with ID: ";
            msg.append(locTedID);
            msg.append(" is not available!");
            scheme_error(msg);
        }
        if ( (gpTypeIDptr->second).gvsType != gtLocTed) {
            msg = locTedID;
            msg.append(" is no locted!");
            scheme_error(msg);
        }
        locTed = gpLocalTetrad[(gpTypeIDptr->second).vectorID];
    }
    else {
        if (gpLocalTetrad.empty()) {
            scheme_error("calc-ray: locted is missing!\n");
        }
        locTed = gpLocalTetrad[0];
    }

    std::string outFileName;
    if (!gvsParser->getParameter("filename",outFileName))
        scheme_error("calc-ray: filename is missing!");


    double pos[4] = {0.0,0.0,0.0,0.0};
    double dir[2] = {0.0,0.0};
    gvsParser->getParameter("pos",&pos[0]);
    m4d::vec4 startPos = m4d::vec4(pos[0],pos[1],pos[2],pos[3]);

    gvsParser->getParameter("dir",&dir[0]);
    double ksi = dir[0] * GVS_deg2rad;
    double chi = dir[1] * GVS_deg2rad;
    m4d::vec4 startDirLoc = m4d::vec4(-1.0, sin(chi)*cos(ksi), sin(chi)*sin(ksi), cos(chi));
    m4d::vec4 startDir;

    //m4d::Metric*   metric = rayGen->getMetric();
    //metric->localToCoord(startPos,startDirLoc,startDir);
    startDir = locTed->localToCoord(startDirLoc);


    int num=0;
    m4d::vec4* polyray = NULL;
    rayGen->calcPolyline(startPos,startDir,polyray, num);

    std::ofstream out(outFileName.c_str());
    for (int i=0; i<num; i++) {
        polyray[i].printF(out);
    }
    out.close();

    delete [] polyray;

    delete gvsParser;
    return sc->NIL;
}


void coordDirTransf (m4d::Metric* cMetric, const m4d::vec4 &Pos, const m4d::vec4 &Dir, m4d::vec4 &newPos, m4d::vec4 &newDir) //To Cartesian
{
    m4d::TransCoordinates  transf;

    switch (cMetric->getCoordType()) {
        case m4d::enum_coordinate_spherical: {
            transf.transCoordSphCart( Pos, Dir, newPos, newDir );
            break;
        }

        case m4d::enum_coordinate_cylinder: {
            transf.transCoordCylCart( Pos, Dir, newPos, newDir );
            break;
        }
        default: std::cerr << "CoordTransformation not implemented yet - Only spherical,cylindrical and cartesian possible. \n";
    }

}

/**
 * @brief gvsP_calc_proj_ray
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_calc_proj_ray ( scheme *sc, pointer args )
{
    if (args == sc->NIL) scheme_error("calc-proj-ray: no arguments");
    if (!is_pair(args)) scheme_error("calc-proj-ray: less arguments");

    if (gpRayGen.size()<1) scheme_error("calc-proj-ray: no ray generator available");

    std::string allowedNames[] = {"filename","raygen","projector","dir","cartesian"};
    GvsParseAllowedNames allowedTypes[] = {{gp_string_string,0}, // filename
                                           {gp_string_string,0}, // raygen
                                           {gp_string_string,0}, // projector
                                           {gp_string_double,3}, // dir
                                           {gp_string_bool,0}    // in cartesian coordinates
                                          };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,5);
    args = gvsParser->parse(args);


    /* -----------------------------
     *   set ray generator
     * ----------------------------- */
    GvsRayGen* rayGen;
    std::string rayGenID;
    if (gpRayGen.size()>1) {
        if (!gvsParser->getParameter("raygen",rayGenID)) {
            scheme_error("calc-ray: several ray generator available; RayGenID missing!");
        }
        gpTypeIDptr = gpTypeID.find(rayGenID);

        std::string msg;
        if (gpTypeIDptr == gpTypeID.end()) {
            msg = "RayGen with ID: ";
            msg.append(rayGenID);
            msg.append(" not available!");
            scheme_error(msg);
        }

        if ( (gpTypeIDptr->second).gvsType != gtRayGen ) {
            msg = rayGenID;
            msg.append(" is no RayGen!");
            scheme_error(msg);
        }

        rayGen = gpRayGen[(gpTypeIDptr->second).vectorID];
    }
    else {
        if (gpRayGen.empty()) {
            scheme_error("calc-proj-ray: RayGen is missing\n");
        }
#ifdef GVS_VERBOSE
        printf("calc-poly-ray: use given RayGen\n");
#endif
        rayGen = gpRayGen[0];
    }

    GvsRayVisual eyeRay(rayGen);

    /* ----------------------------
    *     read projector
    * ---------------------------- */
    std::string projID;
    GvsProjector* proj;
    if (gvsParser->getParameter("projector",projID))
    {
        if (projID=="gtProjector")
            proj = gpProjector[gpProjector.size()-1];
        else
        {
            if (gpProjector.size()>=1)
            {
                getIDptr(gvsParser,"calc-proj-ray","Projektor","projector",gtProjector);
                proj = gpProjector[(gpTypeIDptr->second).vectorID];
            }
        }
    }
    else
    {
        std::cerr << "calc-poly-ray: no projector defined!" << std::endl;
        delete gvsParser;
        return sc->NIL;
    }

    /* ----------------------------
     *     calc ray
     * ---------------------------- */
    GvsLocalTetrad* locTed = proj->getLocalTetrad();

    double direction[3] = {-1.0,0.0,0.0};
    gvsParser->getParameter("dir",direction);

    m4d::vec4 base0 = locTed->getE(0);
    m4d::vec4 base1 = locTed->getE(1);
    m4d::vec4 base2 = locTed->getE(2);
    m4d::vec4 base3 = locTed->getE(3);

    m4d::vec4 rayDir = -base0 + direction[0]*base1 + direction[1]*base2 + direction[2]*base3;
    m4d::vec4 rayOrigin = locTed->getPosition();
    eyeRay.recalc( rayOrigin, rayDir, locTed );


    /* ----------------------------
     *     output
     * ---------------------------- */
    bool inCartesian = false;
    gvsParser->getParameter("cartesian",inCartesian);


    std::string outFileName;
    if (!gvsParser->getParameter("filename",outFileName))
        scheme_error("calc-proj-ray: filename is missing!");

    FILE *out;
#ifdef _WIN32
	fopen_s(&out, outFileName.c_str(), "w");
#else
    out = fopen(outFileName.c_str(),"w");
#endif

    double val[25];
    m4d::vec4 pos,cpos;
    m4d::vec4 vel,cvel;
    GvsLocalTetrad lt;
    double norm;


    int num = eyeRay.getNumPoints();
    for (int i=0; i<num; i++)
    {
        pos = eyeRay.getPoint(i);
        vel = eyeRay.getTangente(i);
        lt  = eyeRay.getTetrad(i);

        lt.getMetric()->calcProduct(pos,vel,vel,norm);
        norm=sqrt(abs(norm));

        if (inCartesian)
            //lt.getMetric()->coordDirTransf(lt.getMetric()->getCoordType(),pos,vel,m4d::enum_coordinate_cartesian,cpos,cvel);
            coordDirTransf(lt.getMetric(),pos,vel,cpos,cvel);
        else
        {
            cpos = pos;
            cvel = vel;
        }


        m4d::vec4 base;
        for (int j=0; j<4; j++)
        {
            val[j]   = cpos.x(j);
            val[j+5] = cvel.x(j);
            coordDirTransf(lt.getMetric() ,pos,lt.getE(j),cpos,base);
            for (int k=0; k<4; k++)
                val[9+j*4+k] = base.x(k);
        }
        val[4] = 0;//pos.x(4);

        for (int j=0; j<25; j++)
        {
            fprintf(out,"%14.8f ",val[j]);
        }
        fprintf(out,"   %e\n",norm);
    }

    fclose(out);

    delete gvsParser;
    return sc->NIL;
}

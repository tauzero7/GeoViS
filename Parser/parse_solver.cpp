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
    Initializing a solver can be done in the following way. Arguments in brackets [ ] are optional.
    @verbatim
    (init-solver '(type "SolverName")
               [ '(geodType "lightlike") ]
               [ '(geodDir "forward")    ]
               [ '(step_ctrl #t)         ]
               [ '(step_size <double>)   ]
               [ '(max_step <double>)    ]
               [ '(eps_abs <double>)     ]
               [ '(eps_rel <double>)     ]
               [ '(id "solver")          ]
    )@endverbatim

    - The solver type depends on the Motion4D library. Possible values are 'GSL_RK_Cash-Karp',...
    - The geodesic type (geodType) can be either 'lightlike' or 'timelike'.
    - The direction can only be 'forward' or 'backward'. If the solver is used for raytracing, then
      the direction is automatically set to 'backward'.
*/

#include "Parser/parse_solver.h"
#include "Parser/parse_helper.h"
#include "GvsParseScheme.h"

#include <metric/m4dMetric.h>
#include <motion/m4dMotionDatabase.h>

#include "Utils/GvsGeodSolver.h"

extern std::vector<GvsGeodSolver*>  gpSolver;
extern std::vector<Gvsm4dMetricDummy*>  gpMetric;

extern std::map<std::string,GvsTypeID>           gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;

/**
 * @brief gvsP_init_solver
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_solver (scheme *sc, pointer args) {
#ifdef GVS_VERBOSE
    std::cerr << "\n.....gvsP_init_solver.....\n";
#endif

    if (args == sc->NIL) scheme_error("init-solver: no arguments");
    if (!is_pair(args)) scheme_error("init-solver: less arguments");

    std::string allowedNames[] = {
        "type","metric","geodtype","geoddir","step_ctrl","step_size","max_step",
        "eps_abs","eps_rel","boundboxll","boundboxur","id"};

    GvsParseAllowedNames allowedTypes[] = {{gp_string_string,0},  // type
                                           {gp_string_string,0},  // metric
                                           {gp_string_string,0},  // geodtype
                                           {gp_string_string,0},  // geoddir
                                           {gp_string_bool,0},    // step_ctrl
                                           {gp_string_double,1},  // step_size
                                           {gp_string_double,1},  // max_step
                                           {gp_string_double,1},  // eps_abs
                                           {gp_string_double,1},  // eps_rel
                                           {gp_string_double,4},  // boundBoxLL
                                           {gp_string_double,4},  // boundBoxUR
                                           {gp_string_string,0}   // id
                                          };

    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,12);
    args = gvsParser->parse(args);
    gvsParser->testParamNames("init-solver");

    GvsGeodSolver* currSolver = nullptr;
    m4d::Metric*   currMetric = nullptr;

    currMetric = readMetric("init-solver: ",gvsParser);
    if (currMetric == nullptr) {
        exit(-1);
    }

    std::string solverName;
    if (!gvsParser->getParameter("type",solverName)) {
        scheme_error("init-solver: type is missing!");
    }

    //m4d::IntegratorDatabase* IntDB = m4d::IntegratorDatabase::getInstance();
    m4d::IntegratorDatabase intDB;

    m4d::enum_integrator solverID = intDB.getIntegratorNr(solverName.c_str());
    if (solverID==m4d::gsUnknown) scheme_error("Solver is not in the m4d database!");
    
    // Initialize solver
#ifdef GVS_VERBOSE
    printf("\n-->Initialize solver...\n");
#endif

    if (solverID!=m4d::gsUnknown){
        currSolver = new GvsGeodSolver(currMetric, solverID);
    }

    // Set geodesic type
    std::string geodType;
    if (gvsParser->getParameter("geodtype",geodType)) {
        if (geodType == "lightlike") currSolver->setGeodType(m4d::enum_geodesic_lightlike);
        else if (geodType == "timelike") currSolver->setGeodType(m4d::enum_geodesic_timelike);
        else if (geodType == "spacelike") currSolver->setGeodType(m4d::enum_geodesic_spacelike);  // not tested
    } else {
        currSolver->setGeodType(m4d::enum_geodesic_lightlike);
    }

    // Set geodesic direction
    std::string geodDir;
    if (gvsParser->getParameter("geoddir",geodDir)) {
        if (geodDir == "forward") currSolver->setTimeDir(m4d::enum_time_forward);
        else if (geodDir == "backward") currSolver->setTimeDir(m4d::enum_time_backward);
        else scheme_error("init-solver: wrong time direction!");
    }

    bool step_control;
    if (!gvsParser->getParameter("step_ctrl",step_control)) step_control = true;
    currSolver->setStepSizeControl(step_control);

    double step_size;
    if (!gvsParser->getParameter("step_size",step_size)) step_size = 0.01;
    currSolver->setStepsize(step_size);

    double maxStep;
    if (!gvsParser->getParameter("max_step",maxStep)) maxStep = DEF_MAX_STEPSIZE;
    currSolver->setMaxStepsize(maxStep);


    double eps_abs,eps_rel;
    if (!gvsParser->getParameter("eps_abs",eps_abs)) eps_abs = 1.0e-4;
    if (!gvsParser->getParameter("eps_rel",eps_rel)) eps_rel = 0.0;
    currSolver->setEpsilons(eps_abs,eps_rel);

    // Set bounding box
    double boundBoxLL[4] = {-DBL_MAX,-50.0,-50.0,-50.0};
    double boundBoxUR[4] = { DBL_MAX, 50.0, 50.0, 50.0};

    bool haveBoxLL = gvsParser->getParameter("boundboxll",&boundBoxLL[0]);
    bool haveBoxUR = gvsParser->getParameter("boundboxur",&boundBoxUR[0]);
    if (haveBoxLL || haveBoxUR) {
        currSolver->setBoundingBox(boundBoxLL,boundBoxUR);
    }

    gpSolver.push_back(currSolver);

#ifdef GVS_VERBOSE
    currSolver->Print();
    printf("\n");
#endif

    std::string idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-solver: ID already assigned!");
    }

    GvsTypeID tid = {gtGeodSolver,static_cast<int>(gpSolver.size())-1,gpSolver[gpSolver.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));    
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtGeodSolver"));
    return R;
}

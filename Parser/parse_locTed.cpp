/**
 * @file    parse_locTed.h
 * @author  Thomas Mueller
 *
 *  This file is part of GeoViS.
 */
#include "Parser/parse_locTed.h"
#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Parser/parse_helper.h"

#include "GvsParseScheme.h"

extern std::vector<GvsLocalTetrad*> gpLocalTetrad;
extern std::vector<Gvsm4dMetricDummy*> gpMetric;

extern std::map<std::string, GvsTypeID> gpTypeID;
extern std::map<std::string, GvsTypeID>::iterator gpTypeIDptr;

pointer gvsP_local_tetrad(scheme* sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_local_tetrad..........\n";
#endif
    if (args == sc->NIL)
        scheme_error("localTetrad: no arguments");
    if (!is_pair(args))
        scheme_error("localTetrad: less arguments");

    double pos[4] = { 0, 10, 1.5707963, 0 };
    double e0[4] = { 1, 0, 0, 0 };
    double e1[4] = { 0, 1, 0, 0 };
    double e2[4] = { 0, 0, 1, 0 };
    double e3[4] = { 0, 0, 0, 1 };
    bool inCoords = false;

    double vel[4] = { 1.0, 0.0, 0.0, 0.0 };
    double locVel[3] = { 0.0, 0.0, 0.0 };
    // bool haveVel     = false;
    bool haveLocVel = false;

    m4d::Metric* currMetric = nullptr;
    std::string allowedNames[11]
        = { "id", "pos", "vel", "localvel", "e0", "e1", "e2", "e3", "incoords", "metric", "localframe" };
    GvsParseAllowedNames allowedTypes[11] = {
        { gp_string_string, 0 }, // id
        { gp_string_double, 4 }, // pos
        { gp_string_double, 4 }, // vel
        { gp_string_double, 3 }, // localvel
        { gp_string_double, 4 }, // e0
        { gp_string_double, 4 }, // e1
        { gp_string_double, 4 }, // e2
        { gp_string_double, 4 }, // e3
        { gp_string_bool, 0 }, // incoords
        { gp_string_string, 1 }, // metric
        { gp_string_int, 0 } // localframe
    };

    GvsParseScheme* gvsParser = new GvsParseScheme(sc, allowedNames, allowedTypes, 11);
    args = gvsParser->parse(args);
    gvsParser->testParamNames("local-tetrad");
    // gvsParser->print();

    // Read positions, base std::vectors and representation
    gvsParser->getParameter("pos", pos);
    gvsParser->getParameter("e0", e0);
    gvsParser->getParameter("e1", e1);
    gvsParser->getParameter("e2", e2);
    gvsParser->getParameter("e3", e3);
    gvsParser->getParameter("incoords", inCoords);

    // haveVel    = gvsParser->getParameter("vel",vel);
    gvsParser->getParameter("vel", vel);
    haveLocVel = gvsParser->getParameter("localvel", locVel);

    // Read metric
    currMetric = readMetric("local-tetrad", gvsParser);
    if (currMetric == nullptr) {
        exit(-1);
    }
    // currMetric->print();

    GvsLocalTetrad* locTed = new GvsLocalTetrad(currMetric, m4d::vec4(e0[0], e0[1], e0[2], e0[3]),
        m4d::vec4(e1[0], e1[1], e1[2], e1[3]), m4d::vec4(e2[0], e2[1], e2[2], e2[3]),
        m4d::vec4(e3[0], e3[1], e3[2], e3[3]), m4d::vec4(pos[0], pos[1], pos[2], pos[3]), inCoords);
    // locTed->Print();

    int locFrame = -1;
    if (gvsParser->getParameter("localframe", locFrame)) {
        if (locFrame >= 0 && locFrame < (int)m4d::NUM_ENUM_NAT_TETRAD_TYPES) {
            // std::cerr << m4d::stl_nat_tetrad_types[locFrame] << std::endl;
            locTed->setInCoords(false, (m4d::enum_nat_tetrad_type)locFrame);
        }
    }

    // Read velocity
    if (haveLocVel) {
        // NOTE: velocity wrt. natural local tetrad !!!
        locTed->setLocalVel(m4d::vec3(locVel[0], locVel[1], locVel[2]));
    }

    gpLocalTetrad.push_back(locTed);

    std::string idname = "unknown";
    if (!gvsParser->getParameter("id", idname)) {
        appendNum(idname, static_cast<int>(gpTypeID.size()));
    }
    else if (gpTypeID.find(idname) != gpTypeID.end()) {
        scheme_error("local-tetrad: ID already in use!");
    }

    GvsTypeID tid = { gtLocTed, static_cast<int>(gpLocalTetrad.size()) - 1, gpLocalTetrad[gpLocalTetrad.size() - 1] };
    gpTypeID.insert(std::pair<std::string, GvsTypeID>(idname, tid));

    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtLocTed"));
    return R;
}

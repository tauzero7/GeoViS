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

#include "parse_meshObjects.h"
#include "parse_helper.h"
#include "GvsParseScheme.h"

#include "GvsGlobalDefs.h"
#include "Obj/MeshObj/GvsOBJMesh.h"

#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Obj/STMotion/GvsStMotion.h"

#include "metric/m4dMetric.h"
#include "math/Mat.h"

extern std::vector<GvsLocalTetrad*>    gpLocalTetrad;
extern std::vector<Gvsm4dMetricDummy*> gpMetric;
extern std::vector<GvsShader*>         gpShader;
extern std::vector<GvsSceneObj*>       gpSceneObj;
extern std::vector<GvsStMotion*>       gpMotion;

extern std::map<std::string,GvsTypeID>           gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;

/**
 * @brief gvsP_init_OBJmesh
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_OBJmesh ( scheme *sc, pointer args )
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_init_OBJmesh..........\n";
#endif

    if (args == sc->NIL) scheme_error("mesh-obj: no arguments");
    if (!is_pair(args)) scheme_error("mesh-obj: less arguments");


    GvsSceneObj*     meshOBJ;
    GvsShader*       currShader;
    m4d::Metric*     currMetric;
    int objType = inCoords;

    std::string allowedNames[] = {
        "objtype","id","filename","pathname","shader","metric","transform","rotate","motion","chart"
    };
    GvsParseAllowedNames allowedTypes[] = {{gp_string_int,1},    // objtype
                                           {gp_string_string,0}, // id
                                           {gp_string_string,0}, // filename
                                           {gp_string_string,0}, // pathname
                                           {gp_string_string,0}, // shader
                                           {gp_string_string,0}, // metric
                                           {gp_string_matrix,0}, // transform
                                           {gp_string_double,4}, // rotate
                                           {gp_string_string,0}, // motion
                                           {gp_string_int,1}     // chart
                                          };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,10);

    args = gvsParser->parse(args);
    gvsParser->testParamNames("mesh-obj");
    gvsParser->getParameter("objtype",&objType);

    std::string msg;
    std::string objFilename, objPathname;
    if (!gvsParser->getParameter("filename",objFilename) || !gvsParser->getParameter("pathname",objPathname)) {
        msg = "mesh-obj";
        msg.append(": missing or incorrect file and path names: ");
        scheme_error(msg);
    }

    currShader = readShader("mesh-obj",gvsParser);
    currMetric = readMetric("mesh-obj",gvsParser);

    meshOBJ = new GvsOBJMesh(objPathname.c_str(),objFilename.c_str(),(GvsSurfaceShader*)currShader, currMetric,(GvsObjType)objType);
    //meshOBJ->Print();

    int chart = 0;
    if (gvsParser->getParameter("chart",&chart)) {
        meshOBJ->setChart(chart);
    }

    double rot[4];
    if (gvsParser->getParameter("rotate",&rot[0])) {
        m4d::vec3 rotAxis = m4d::vec3(rot[0],rot[1],rot[2]);
        rotAxis.normalize();

        meshOBJ->rotate(rotAxis,rot[3]*GVS_deg2rad);
    }

    m4d::Matrix<double,3,4> transMat3D;
    if (!gvsParser->getParameter("transform",&transMat3D)) {
        transMat3D.setIdent();
    }
    meshOBJ->transform(transMat3D);

    bool haveMotion = false;
    GvsStMotion* currMotion;
    std::string motionID;
    if (gvsParser->getParameter("motion",motionID)) {

        gpTypeIDptr = gpTypeID.find(motionID);
        if (gpTypeIDptr != gpTypeID.end()) {
            currMotion = gpMotion[(gpTypeIDptr->second).vectorID];
            haveMotion = true;
        } else {
            msg = "mesh-obj";
            msg.append(": object with ID \"");
            msg.append(motionID);
            msg.append("\" is no motion!");
            scheme_error(msg);
        }
    }

    if (haveMotion)  {
        if (currMotion->getMotionType() == gvsMotionConstVelocity)
            meshOBJ->setMotion(currMotion);
    }

    gpSceneObj.push_back(meshOBJ);
    // gpSceneObj[0]->print(cerr);

    std::string idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname, (int)gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("mesh-obj: ID is already assigned!");
    }

    GvsTypeID tid = {gtSceneObj,static_cast<int>(gpSceneObj.size())-1,gpSceneObj[gpSceneObj.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtSceneObj"));
    return R;
}


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
 *        (csg-object   '(csgtype ,CSGTYPE )
 *                      '(obj1   "obj1" )
 *                      '(obj2   "obj2" )
 *                      '(id    "CSGObjID")
 *        )
 *
 */

#include "Parser/GvsParseScheme.h"
#include "Parser/parse_csgObj.h"
#include "Parser/parse_helper.h"

#include <Obj/SolidObj/GvsSolidObj.h>
#include <Obj/SolidObj/GvsSolidCSGObj.h>
#include <Obj/SolidObj/GvsSolidDifferObj.h>
#include <Obj/SolidObj/GvsSolidIntersecObj.h>
#include <Obj/SolidObj/GvsSolidUnifiedObj.h>
#include <Shader/GvsShader.h>

#include <GvsGlobalDefs.h>

extern std::vector<GvsSceneObj*>    gpSceneObj;
extern std::vector<GvsSolidObj*>    gpSolidObj;
extern std::vector<GvsShader*>      gpShader;

extern std::map<std::string,GvsTypeID>           gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;

//----------------------------------------------------------------------------
//         gvsP_init_solidbox
//----------------------------------------------------------------------------
pointer gvsP_init_csg_obj (scheme *sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_init_csg_obj..........\n";
#endif

    if (args == sc->NIL) scheme_error("csg-obj: no arguments");
    if (!is_pair(args)) scheme_error("csg-obj: less arguments");

    std::string allowedNames[4] = {"csgType","obj1","obj2","id"};
    GvsParseAllowedNames allowedTypes[4] = {{gp_string_int,1},    // csgType
                                            {gp_string_string,0}, // obj1
                                            {gp_string_string,0}, // obj2
                                            {gp_string_string,0}  // id
                                           };

    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,4);
    args = gvsParser->parse(args);

    GvsSolidObj* child1;
    GvsSolidObj* child2;
    GvsSceneObj* csgObj;
    int          csgType = CSG_Obj;
    std::string CSGTYPE;

    gvsParser->getParameter("csgType",&csgType);

    std::string objID1, objID2;
    gvsParser->getParameter("obj1",objID1);
    gvsParser->getParameter("obj2",objID2);

    //std::cerr << "CSG_Obj1 = " << objID1 << "\tCSG_Obj2 = " << objID2 << std::endl;

    int num = 0;
    if (gpSceneObj.size()>=1) {
        getIDptr(gvsParser,"CSGDifferObject","Object","obj1",gtSceneObj,num);
        child1 = (GvsSolidObj*)gpSceneObj[(gpTypeIDptr->second).vectorID];
    }
    else {
        scheme_error("comp-object: no scene object available!\n");
    }

    if (gpSceneObj.size()>=1) {
        getIDptr(gvsParser,"CSGDifferObject","Object","obj2",gtSceneObj,num);
        child2 = (GvsSolidObj*)gpSceneObj[(gpTypeIDptr->second).vectorID];
    }
    else {
        scheme_error("comp-object: no scene object available!\n");
    }

    if (csgType == CSG_Obj) {
        csgObj  = new GvsSolidDifferObj(child1,child2);
        CSGTYPE = "gpCSGDifferObj";
        std::cerr << "Error: csgtype = " << csgType
                  << ". Invalid CSG_ObjType. CSG_ObjType was set to gpCSGDifferObj."
                  << " Next time choose gpCSGDifferObj = 1, gpCSGUnifiedObj = 2, gpCSGIntersecObj = 3"
                  << std::endl;
        exit(0);
    }

    else if (csgType == CSG_differObj) {
        csgObj = new GvsSolidDifferObj(child1,child2);
        CSGTYPE = "gpCSGDifferObj";
        //      std::cerr << "Error: csgtype = " << CSGTYPE << std::endl;
    }

    else if (csgType == CSG_unifiedObj) {
        csgObj = new GvsSolidUnifiedObj(child1,child2);
        CSGTYPE = "gpCSGUnifiedObj";
        //      std::cerr << "Error: csgtype = " << CSGTYPE << std::endl;
    }

    else if (csgType == CSG_intersecObj) {
        csgObj = new GvsSolidIntersecObj(child1,child2);
        CSGTYPE = "gpCSGIntersecObj";
        //      std::cerr << "Error: csgtype = " << CSGTYPE << std::endl;
    }

    else {
        //      csgObj = new GvsSolidDifferObj(child1,child2);
        std::cerr << "Error: csgtype = " << csgType
                  << ". Invalid CSG_ObjType."
                  << " Next time choose gpCSGDifferObj = 1, gpCSGUnifiedObj = 2, gpCSGIntersecObj = 3"
                  << std::endl;
        CSGTYPE = "gpCSGDifferObj";

        //*/
        exit(0);
    }

    std::string idname;

    gpSceneObj.push_back(csgObj);
    // gpSceneObj[0]->print(cerr);

    idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname, (int)gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("csg-obj: ID already assigned!");
    }

    GvsTypeID tid = {gtSceneObj,static_cast<int>(gpSceneObj.size())-1,gpSceneObj[gpSceneObj.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtSceneObj"));
    return R;
}

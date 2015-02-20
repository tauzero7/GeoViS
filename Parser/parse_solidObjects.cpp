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
        (solid-box   `(objType ,gpObjTypeInCoords )
                     '(cornerLL  #(-2.0 -2.0 -2.0))
                     '(cornerUR  #( 2.0  2.0  2.0))
                     '(shader    "shader")
                     '(id  "ID")
        )

        (solid-ellipsoid   '(objType     0)
                           '(center   #( 0.0 0.0 0.0))
                           '(axlen    #( 2.0 2.0 2.0))
                           '(shader   "shader")
                           '(id  "ID")
        )


        (solid-background  '(objType     0)
                           '(center   #( 0.0 0.0 0.0))
                           '(axlen    #( 2.0 2.0 2.0))
                           '(shader   "shader")
                           '(id  "ID")
        )


        (solid-cylinder   '(objType     0)
                           '(base     #( 0.0 0.0 0.0))
                           '(top      #( 2.0 2.0 2.0))
                           '(radii    #( 2.0 2.0))
                           '(shader   "shader")
                           '(id  "ID")
        )
         
        (solid-ring   '(objType     0)
                      '(base     #( 0.0 0.0 0.0))
                      '(top      #( 2.0 2.0 2.0))
                      '(radii    #( 2.0 2.0))
                      '(innerradii #( 0.5 0.5 ))
                      '(shader   "shader")
                      '(id  "ID")
        )
 */

#include "parse_solidObjects.h"
#include "parse_helper.h"
#include "GvsParseScheme.h"

#include "GvsGlobalDefs.h"
#include "Obj/SolidObj/GvsSolBox.h"
#include "Obj/SolidObj/GvsSolCylinder.h"
#include "Obj/SolidObj/GvsSolEllipsoid.h"
#include "Obj/SolidObj/GvsSolBackground.h"
#include "Obj/SolidObj/GvsSolRing.h"

#include "Obj/STMotion/GvsLocalTetrad.h"
#include "Obj/STMotion/GvsStMotion.h"

#include <metric/m4dMetric.h>
#include "math/Mat.h"

extern std::vector<GvsLocalTetrad*>    gpLocalTetrad;
extern std::vector<Gvsm4dMetricDummy*> gpMetric;
extern std::vector<GvsShader*>         gpShader;
extern std::vector<GvsSceneObj*>       gpSceneObj;
extern std::vector<GvsStMotion*>       gpMotion;

extern std::map<std::string,GvsTypeID>           gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;


/**
 * @brief gvsP_init_solidbox
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_solidbox (scheme *sc, pointer args) {
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_solid-box..........\n";
#endif

    if (args == sc->NIL) scheme_error("solid-box: no arguments");
    if (!is_pair(args)) scheme_error("solid-box: less arguments");

    double cornerLL[3] = { 0.0, 0.0, 0.0 };
    double cornerUR[3] = { 1.0, 1.0, 1.0 };

    GvsSceneObj*  solBox;
    GvsShader*    currShader;
    m4d::Metric*  currMetric;
    int objType = inCoords;

    std::string allowedNames[] = {
        "objtype","id","cornerll","cornerur","shader","metric","transform","motion","chart"
    };
    GvsParseAllowedNames allowedTypes[] = {{gp_string_int,1},    // objtype
                                           {gp_string_string,0}, // id
                                           {gp_string_double,3}, // cornerLL
                                           {gp_string_double,3}, // cornerUR
                                           {gp_string_string,0}, // shader
                                           {gp_string_string,0}, // metric
                                           {gp_string_matrix,0}, // transform
                                           {gp_string_string,0}, // motion
                                           {gp_string_int,1}     // chart
                                          };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,9);

    args = gvsParser->parse(args);
    gvsParser->testParamNames("solid-box");
    gvsParser->getParameter("objtype",&objType);

    gvsParser->getParameter("cornerll",&cornerLL[0]);
    gvsParser->getParameter("cornerur",&cornerUR[0]);

    currShader = readShader("solid-box",gvsParser);
    currMetric = readMetric("solid-box",gvsParser);

    solBox = new GvsSolBox(m4d::vec3(cornerLL[0],cornerLL[1],cornerLL[2]),m4d::vec3(cornerUR[0],cornerUR[1],cornerUR[2]),
            (GvsSurfaceShader*)currShader, currMetric, (GvsObjType)objType);

    int chart = 0;
    if (gvsParser->getParameter("chart",&chart)) {
        solBox->setChart(chart);
    }

    m4d::Matrix<double,3,4> transMat3D;
    if (!gvsParser->getParameter("transform",&transMat3D)) {
        transMat3D.setIdent();
    }
    solBox->transform(transMat3D);


    // read motion: only ConstVelocity is valid
    bool haveMotion = false;
    GvsStMotion* currMotion;
    std::string motionID;

    if (gvsParser->getParameter("motion",motionID)) {        
        gpTypeIDptr = gpTypeID.find(motionID);
        if (gpTypeIDptr != gpTypeID.end()) {
            currMotion = gpMotion[(gpTypeIDptr->second).vectorID];
            haveMotion = true;
        }
        else {
            if ( (gvsParser->getParameter("motion",motionID)) && (gpMotion.size()>0) )  {
                getIDptr(gvsParser,"solid-box","Motion","motion",gtMotion);
                currMotion = gpMotion[(gpTypeIDptr->second).vectorID];
                haveMotion = true;
            }
        }
    }

    if (haveMotion) {
        if (currMotion->getMotionType() == gvsMotionConstVelocity)
            solBox->setMotion(currMotion);
    }

    gpSceneObj.push_back(solBox);
    //gpSceneObj[0]->Print();

    std::string idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("solid-box: ID is already assigned!");
    }

    GvsTypeID tid = {gtSceneObj,static_cast<int>(gpSceneObj.size())-1,gpSceneObj[gpSceneObj.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtSceneObj"));
    return R;
}


/**
 * @brief gvsP_init_solidellipsoid
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_solidellipsoid ( scheme *sc, pointer args )
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_solid-ellipsoid..........\n";
#endif

    if (args == sc->NIL) scheme_error("solid-ellipsoid: no arguments");
    if (!is_pair(args)) scheme_error("solid-ellipsoid: less arguments");

    double center[3] = { 0.0, 0.0, 0.0 };
    double axlen[3]  = { 1.0, 1.0, 1.0 };

    GvsSceneObj*   solEllipsoid;
    GvsShader*     currShader;
    m4d::Metric*   currMetric;
    int objType = inCoords;

    std::string allowedNames[] = {
        "objtype","id","center","axlen","shader","metric","transform","motion","chart"
    };
    GvsParseAllowedNames allowedTypes[] = {{gp_string_int,1},    // objtype
                                           {gp_string_string,0}, // id
                                           {gp_string_double,3}, // center
                                           {gp_string_double,3}, // axlen
                                           {gp_string_string,0}, // shader
                                           {gp_string_string,0}, // metric
                                           {gp_string_matrix,0}, // transform
                                           {gp_string_string,0}, // motion
                                           {gp_string_int,1}     // chart
                                          };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,9);
    args = gvsParser->parse(args);

    gvsParser->testParamNames("solid-ellipsoid");
    gvsParser->getParameter("objtype",&objType);
    gvsParser->getParameter("center",&center[0]);
    gvsParser->getParameter("axlen",&axlen[0]);

    currShader = readShader("solid-ellipsoid",gvsParser);
    currMetric = readMetric("solid-ellipsoid",gvsParser);

    solEllipsoid = new GvsSolEllipsoid(m4d::vec3(center[0],center[1],center[2]),
            m4d::vec3(axlen[0],axlen[1],axlen[2]),
            (GvsSurfaceShader*)currShader, currMetric, (GvsObjType)objType);

    int chart = 0;
    if (gvsParser->getParameter("chart",&chart)) {
        solEllipsoid->setChart(chart);
    }
    //solEllipsoid->Print();


    m4d::Matrix<double,3,4> transMat3D;
    if (!gvsParser->getParameter("transform",&transMat3D)) {
        transMat3D.setIdent();
    }
    solEllipsoid->transform(transMat3D);


    // Read motion - only const velocity is permitted
    bool haveMotion = false;
    GvsStMotion* currMotion;
    std::string  motionID;
    if (gvsParser->getParameter("motion",motionID)) {
        if (motionID=="gtMotion") {
            currMotion = gpMotion[gpMotion.size()-1];
            haveMotion = true;
        }
    }
    if ( (gvsParser->getParameter("motion",motionID)) &&
         (gpMotion.size()>0) )
    {
        getIDptr(gvsParser,"local-comp-object","Motion","motion",gtMotion);
        currMotion = gpMotion[(gpTypeIDptr->second).vectorID];
        haveMotion = true;
    }
    if (haveMotion)
        if (currMotion->getMotionType() == gvsMotionConstVelocity)
            solEllipsoid->setMotion(currMotion);

    gpSceneObj.push_back(solEllipsoid);
    // gpSceneObj[0]->print(cerr);

    std::string idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("solid-ellipsoid:  ID already assigned!");
    }

    GvsTypeID tid = {gtSceneObj,static_cast<int>(gpSceneObj.size())-1,gpSceneObj[gpSceneObj.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));

    delete gvsParser;
    pointer R = ((sc->vptr->mk_symbol)(sc, "gtSceneObj"));
    return R;
}


/**
 * @brief gvsP_init_solidbackground
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_solidbackground ( scheme *sc, pointer args )
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_solid-background..........\n";
#endif

    if (args == sc->NIL) scheme_error("solid-background: no arguments");
    if (!is_pair(args)) scheme_error("solid-background: less arguments");

    GvsSceneObj*     solBackground;
    GvsShader*       currShader;
    m4d::Metric*     currMetric;
    int objType = inCoords;

    std::string allowedNames[] = {"objtype","id","shader","metric","transform"};
    GvsParseAllowedNames allowedTypes[] = {{gp_string_int,1},    // objtype
                                           {gp_string_string,0}, // id
                                           {gp_string_string,0}, // shader
                                           {gp_string_string,0}, // metric
                                           {gp_string_matrix,0}  // transform
                                          };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,5);

    // Einlesen der Argumente der Metrik
    args = gvsParser->parse(args);
    gvsParser->testParamNames("solid-background");
    gvsParser->getParameter("objtype",&objType);

    currShader = readShader("solid-background",gvsParser);
    currMetric = readMetric("solid-background",gvsParser);

    solBackground = new GvsSolBackground((GvsSurfaceShader*)currShader, currMetric, (GvsObjType)objType);

    // Transformation
    m4d::Matrix<double,3,4> transMat3D;
    bool haveTransformation = gvsParser->getParameter("transform",&transMat3D);
    if (!haveTransformation)
        transMat3D.setIdent();
    solBackground->transform(transMat3D);

    gpSceneObj.push_back(solBackground);
    // gpSceneObj[0]->print(cerr);

    std::string idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("solid-background: ID already assigned!");
    }

    GvsTypeID tid = {gtSceneObj,static_cast<int>(gpSceneObj.size())-1,gpSceneObj[gpSceneObj.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtSceneObj"));
    return R;
}

/**
 * @brief gvsP_init_solidcylinder
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_solidcylinder ( scheme *sc, pointer args )
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_solidcylinder..........\n";
#endif

    if (args == sc->NIL) scheme_error("solid-cylinder: no arguments");
    if (!is_pair(args)) scheme_error("solid-clinder: less arguments");

    //Hier werden veraenderungen noetig
    double base[3] = { 0.0, 0.0, 0.0 };
    double top[3]  = { 0.0, 0.0, 1.0 };
    double radii[2] = {1.0, 1.0};

    GvsSceneObj*     solCylinder;
    GvsShader*       currShader;
    m4d::Metric*     currMetric;
    int objType = inCoords;

    std::string allowedNames[] = {
        "objtype","id","top","base","shader","metric","chart","transform","motion","radii"
    };
    GvsParseAllowedNames allowedTypes[] = {{gp_string_int,1},    // objtype
                                            {gp_string_string,0}, // id
                                            {gp_string_double,3}, // top
                                            {gp_string_double,3}, // base
                                            {gp_string_string,0}, // shader
                                            {gp_string_string,0}, // metric
                                            {gp_string_int,1},    // chart
                                            {gp_string_matrix,0}, // transform
                                            {gp_string_string,0}, // motion
                                            {gp_string_double,2}  // radii
                                           };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,10);



    args = gvsParser->parse(args);
    gvsParser->testParamNames("solid-cylinder");
    gvsParser->getParameter("objtype",&objType);
    gvsParser->getParameter("radii",&radii[0]);
    gvsParser->getParameter("top",&top[0]);
    gvsParser->getParameter("base",&base[0]);

    currShader = readShader("solid-cylinder",gvsParser);
    currMetric = readMetric("solid-cylinder",gvsParser);

    solCylinder = new GvsSolCylinder(m4d::vec3(base[0],base[1],base[2]),
            m4d::vec3(top[0],top[1],top[2]),
            m4d::vec2(radii[0],radii[1]),
            (GvsSurfaceShader*)currShader,
            currMetric,
            (GvsObjType)objType);

    int chart = 0;
    if (gvsParser->getParameter("chart",&chart)) {
        solCylinder->setChart(chart);
    }

    // Transformation
    m4d::Matrix<double,3,4> transMat3D;
    if (!gvsParser->getParameter("transform",&transMat3D)) {
        transMat3D.setIdent();
    }
    solCylinder->transform(transMat3D);

    // Read motion: only ConstVelocity is valid.
    GvsStMotion* currMotion = NULL;
    std::string motionID;
    if (gvsParser->getParameter("motion",motionID)) {
        if (motionID=="gtMotion") {
            currMotion = gpMotion[gpMotion.size()-1];
        }
    }
    if ( (gvsParser->getParameter("motion",motionID)) &&
         (gpMotion.size()>0) ) {
        getIDptr(gvsParser,"solid-cylinder","Motion","motion",gtMotion);
        currMotion = gpMotion[(gpTypeIDptr->second).vectorID];
    }
    if (currMotion!=NULL) {
        //currMotion->Print();
        if (currMotion->getMotionType() == gvsMotionConstVelocity)
            solCylinder->setMotion(currMotion);
    }

    gpSceneObj.push_back(solCylinder);
    //solCylinder->Print();

    std::string idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("solid-cylinder: ID already assigned!");
    }

    GvsTypeID tid = {gtSceneObj,static_cast<int>(gpSceneObj.size())-1,gpSceneObj[gpSceneObj.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtSceneObj"));
    return R;
}

/**
 * @brief gvsP_init_solidring
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_solidring( scheme *sc, pointer args ) {

#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_solidring..........\n";
#endif

    if (args == sc->NIL) scheme_error("solid-ring: no arguments");
    if (!is_pair(args)) scheme_error("solid-ring: less arguments");

    double base[3] = { 0.0, 0.0, 0.0 };
    double top[3]  = { 0.0, 0.0, 1.0 };
    double radii[2] = {1.0, 1.0};
    double innerRadii[2] = {0.5,0.5};

    GvsSceneObj*  solRing;
    GvsShader*    currShader;
    m4d::Metric*  currMetric;
    int objType = inCoords;

    std::string allowedNames[] = {
        "objtype","id","top","base","shader","metric","chart","transform","motion","radii","innerradii"
    };
    GvsParseAllowedNames allowedTypes[] = {{gp_string_int,1},    // objtype
                                           {gp_string_string,0}, // id
                                           {gp_string_double,3}, // top
                                           {gp_string_double,3}, // base
                                           {gp_string_string,0}, // shader
                                           {gp_string_string,0}, // metric
                                           {gp_string_int,1},    // chart
                                           {gp_string_matrix,0}, // transform
                                           {gp_string_string,0}, // motion
                                           {gp_string_double,2}, // radii
                                           {gp_string_double,2} // innerradii
                                          };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,11);



    args = gvsParser->parse(args);
    gvsParser->testParamNames("solid-ring");
    gvsParser->getParameter("objtype",&objType);
    gvsParser->getParameter("radii",&radii[0]);
    gvsParser->getParameter("innerradii",&innerRadii[0]);
    gvsParser->getParameter("top",&top[0]);
    gvsParser->getParameter("base",&base[0]);

    currShader = readShader("solid-ring",gvsParser);
    currMetric = readMetric("solid-ring",gvsParser);

    solRing = new GvsSolRing(m4d::vec3(base[0],base[1],base[2]),
                             m4d::vec3(top[0],top[1],top[2]),
                             m4d::vec2(radii[0],radii[1]),
                             m4d::vec2(innerRadii[0],innerRadii[1]),
                             (GvsSurfaceShader*)currShader,
                             currMetric,
                             (GvsObjType)objType);

    int chart = 0;
    if (gvsParser->getParameter("chart",&chart)) {
        solRing->setChart(chart);
    }

    // Transformation
    m4d::Matrix<double,3,4> transMat3D;
    if (!gvsParser->getParameter("transform",&transMat3D)) {
        transMat3D.setIdent();
    }
    solRing->transform(transMat3D);

    // Read motion: only ConstVelocity is valid.
    GvsStMotion* currMotion = NULL;
    std::string motionID;
    if (gvsParser->getParameter("motion",motionID)) {
        if (motionID=="gtMotion") {
            currMotion = gpMotion[gpMotion.size()-1];
        }
    }
    if ( (gvsParser->getParameter("motion",motionID)) &&
         (gpMotion.size()>0) ) {
        getIDptr(gvsParser,"solid-cylinder","Motion","motion",gtMotion);
        currMotion = gpMotion[(gpTypeIDptr->second).vectorID];
    }
    if (currMotion!=NULL) {
        //currMotion->Print();
        if (currMotion->getMotionType() == gvsMotionConstVelocity)
            solRing->setMotion(currMotion);
    }

    gpSceneObj.push_back(solRing);
    //solRing->Print();

    std::string idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("solid-ring: ID already assigned!");
    }

    GvsTypeID tid = {gtSceneObj,static_cast<int>(gpSceneObj.size())-1,gpSceneObj[gpSceneObj.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtSceneObj"));
    return R;
}

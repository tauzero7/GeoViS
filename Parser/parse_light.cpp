/**
 * @file    parse_light.cpp
 * @author  Thomas Mueller
 *
 *  This file is part of GeoViS.
 */
#include "parse_light.h"
#include "parse_helper.h"

#include "Img/GvsColor.h"
#include "Light/GvsFlashLight.h"
#include "Light/GvsLightSrc.h"
#include "Light/GvsLightSrcMgr.h"
#include "Light/GvsPointLight.h"

#include <GvsGlobalDefs.h>

#include "Parser/GvsParseScheme.h"

extern std::vector<GvsLightSrcMgr*> gpLightMgr;
extern std::vector<GvsLightSrc*> gpLight;

extern std::map<std::string, GvsTypeID> gpTypeID;
extern std::map<std::string, GvsTypeID>::iterator gpTypeIDptr;

//----------------------------------------------------------------------------
//         gvsP_init_lightmgr
//----------------------------------------------------------------------------
pointer gvsP_init_lightmgr(scheme* sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n.....gvsP_init_lightmgr.....\n";
#endif
    if (args == sc->NIL)
        scheme_error("init-light-mgr: no arguments");
    if (!is_pair(args))
        scheme_error("init-light-mgr: less arguments");

    std::string allowedNames[5] = { "ambient", "diffuse", "light", "shadows", "id" };
    GvsParseAllowedNames allowedTypes[5] = {
        { gp_string_double, 3 }, // ambient
        { gp_string_double, 3 }, // diffuse
        { gp_string_string, 0 }, // light
        { gp_string_bool, 0 }, // shadows
        { gp_string_string, 0 }, // id
    };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc, allowedNames, allowedTypes, 5);
    args = gvsParser->parse(args);

    GvsLightSrcMgr* lightMgr = new GvsLightSrcMgr;

    bool withShadowRays = false;
    gvsParser->getParameter("shadows", withShadowRays);
    lightMgr->setWithShadowRays(withShadowRays);

    double ambient[3] = { 1.0, 1.0, 1.0 };
    if (gvsParser->getParameter("ambient", ambient)) {
        lightMgr->setAmbientLight(GvsColor(ambient[0], ambient[1], ambient[2]));
    }

    std::string lightID;

    GvsLightSrc* light;
    int num = 0;
    do {
        if (gvsParser->getParameter("light", lightID, num)) {
            if (lightID == "gtLight") {
                light = gpLight[gpLight.size() - 1];
                lightMgr->append(light);
            }
            else {
                if (gpLight.size() >= 1) {
                    getIDptr(gvsParser, "LightMgr", "light", "light", gtLight, num);
                    light = gpLight[(gpTypeIDptr->second).vectorID];
                    lightMgr->append(light);
                }
                else
                    scheme_error("LightMgr: no light source available!\n");
            }
        }

        num++;
    } while (num < gvsParser->getNumParam());

    gpLightMgr.push_back(lightMgr);

    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtLightMgr"));
    return R;
}

//----------------------------------------------------------------------------
//         gvsP_init_light
//----------------------------------------------------------------------------
pointer gvsP_init_light(scheme* sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n.....gvsP_init_light.....\n";
#endif
    if (args == sc->NIL)
        scheme_error("init-light: no arguments");
    if (!is_pair(args))
        scheme_error("init-light: less arguments");

    std::string allowedNames[3] = { "type", "id" };
    GvsParseAllowedNames allowedTypes[2] = {
        { gp_string_string, 0 }, // type
        { gp_string_string, 0 } // id
    };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc, allowedNames, allowedTypes, 2);
    args = gvsParser->parse(args);

    std::string lightType;
    gvsParser->getParameter("type", lightType);

    if (lightType == "PointLight")
        gvsP_init_pointlight(gvsParser);
    else if (lightType == "FlashLight")
        gvsP_init_flashlight(gvsParser);

    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtLight"));
    return R;
}

//----------------------------------------------------------------------------
//         gvsP_init_pointlight
//----------------------------------------------------------------------------
void gvsP_init_pointlight(GvsParseScheme* gP)
{
    GvsPointLight* pLight = new GvsPointLight;

    double lightPos[4] = { 0.0, 6.0, 0.0, 0.0 };
    double col[3] = { 1.0, 1.0, 1.0 };
    // double att[3] = {0.2, 0.0, 0.0};

    gP->setAllowedName("pos", gp_string_double, 4);
    gP->setAllowedName("color", gp_string_double, 3);
    gP->setAllowedName("atten", gp_string_double, 3);

    // Position einlesen
    if (gP->getParameter("pos", lightPos))
        pLight->setPosition(m4d::vec4(lightPos[0], lightPos[1], lightPos[2], lightPos[3]));

    if (gP->getParameter("color", col))
        pLight->setColor(GvsColor(col[0], col[1], col[2]));

    // if (gP->getParameter("atten",att))
    //    pLight->setDistAttenCoeff(m4d::vec3(att[0],att[1],att[2]));

    gpLight.push_back(pLight);

    std::string idname = "unknown";
    if (!gP->getParameter("id", idname)) {
        appendNum(idname, (int)gpTypeID.size());
    }
    else if (gpTypeID.find(idname) != gpTypeID.end()) {
        scheme_error("init-light: ID already assigned!");
    }

    GvsTypeID tid = { gtLight, static_cast<int>(gpLight.size()) - 1, gpLight[gpLight.size() - 1] };
    gpTypeID.insert(std::pair<std::string, GvsTypeID>(idname, tid));
}

//----------------------------------------------------------------------------
//         gvsP_init_flashlight
//----------------------------------------------------------------------------
void gvsP_init_flashlight(GvsParseScheme* gP)
{
    GvsFlashLight* fLight = new GvsFlashLight;

    double lightPos[5] = { 0.0, 6.0, 0.0, 0.0, 0.0 };
    double col[3] = { 1.0, 1.0, 1.0 };
    // double att[3] = {0.2, 0.0, 0.0};

    double lon, loff, synch;
    bool multi;
    std::string pattern;

    gP->setAllowedName("pos", gp_string_double, 5);
    gP->setAllowedName("color", gp_string_double, 3);
    gP->setAllowedName("atten", gp_string_double, 3);
    gP->setAllowedName("pattern", gp_string_string, 0);
    gP->setAllowedName("lengthon", gp_string_double, 1);
    gP->setAllowedName("lengthoff", gp_string_double, 1);
    gP->setAllowedName("multi", gp_string_bool, 0);
    gP->setAllowedName("synchronize", gp_string_double, 1);

    // Position einlesen
    if (gP->getParameter("pos", lightPos))
        fLight->setPosition(m4d::vec4(lightPos[0], lightPos[1], lightPos[2], lightPos[3]));

    if (gP->getParameter("color", col))
        fLight->setColor(GvsColor(col[0], col[1], col[2]));

    // if (gP->getParameter("atten",att))
    //    fLight->setDistAttenCoeff(m4d::vec3(att[0],att[1],att[2]));

    if (gP->getParameter("pattern", pattern))
        fLight->setFlashPattern(pattern);

    bool haveOn = gP->getParameter("lengthon", lon);
    bool haveOff = gP->getParameter("lengthoff", loff);
    if (haveOn || haveOff)
        fLight->setFlashLengths(lon, loff);

    multi = false;
    if (gP->getParameter("multi", multi)) {
        if (multi)
            fLight->setFlashType(gvsFTmultiFlash);
        else
            fLight->setFlashType(gvsFTsingleFlash);
    }

    if (gP->getParameter("synchronize", synch))
        fLight->setFlashStartTime(synch);

    // fLight->print(cerr);
    gpLight.push_back(fLight);

    std::string idname = "unknown";
    if (!gP->getParameter("id", idname)) {
        appendNum(idname, static_cast<int>(gpTypeID.size()));
    }
    else if (gpTypeID.find(idname) != gpTypeID.end()) {
        scheme_error("init-light: ID already assigned!");
    }

    GvsTypeID tid = { gtLight, static_cast<int>(gpLight.size()) - 1, gpLight[gpLight.size() - 1] };
    gpTypeID.insert(std::pair<std::string, GvsTypeID>(idname, tid));
}

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
        (init-shader '(type  "SurfShader")
                     '(objcolor <texture> )
                     '(ambient   <double> )
                     '(diffuse   <double> )
                     '(id "ID")
        )
 */

#include "parse_shader.h"
#include "parse_helper.h"

#include <GvsGlobalDefs.h>

#include "Img/GvsColor.h"
#include "Parser/GvsParseScheme.h"
#include "Shader/GvsShader.h"
#include "Shader/Surface/GvsSurfaceShader.h"
#include "Texture/GvsTexture.h"

extern std::vector<GvsShader*>     gpShader;
extern std::vector<GvsTexture*>    gpTexture;

extern std::map<std::string,GvsTypeID>  gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;


/**
 * 
 */
pointer gvsP_init_shader (scheme *sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n.....gvsP_init_shader.....\n";
#endif
    if (args == sc->NIL) scheme_error("init-shader: no arguments");
    if (!is_pair(args)) scheme_error("init-shader: less arguments");

    std::string allowedNames[3] = {"type","id","color"};
    GvsParseAllowedNames allowedTypes[3] = {{gp_string_string,1}, // type
                                            {gp_string_string,0}, // id
                                            {gp_string_double,3}, // color
                                           };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,3);
    args = gvsParser->parse(args);

    std::string shaderType;
    gvsParser->getParameter("type",shaderType);
    if (shaderType =="SurfShader") gvsP_init_surfaceShd(gvsParser);

    delete gvsParser;
    pointer R = ((sc->vptr->mk_symbol)(sc, "gtShader"));
    return R;
}


/**
 * 
 */
void gvsP_init_surfaceShd(GvsParseScheme* gP) {
    GvsSurfaceShader* shader = new GvsSurfaceShader;

    std::string apNames[] = {"objcolor","normalmap","ambient","diffuse"};
    GvsParseAllowedNames allowedTypes[] = {{gp_string_string,0}, // objcolor
                                           {gp_string_string,0}, // normalmap
                                           {gp_string_double,1}, // ambient
                                           {gp_string_double,1}, // diffuse
                                          };
    for (int i=0;i<4;i++) {
        gP->setAllowedName(apNames[i],allowedTypes[i].paramType,allowedTypes[i].paramDim);
    }


    GvsTexture* currTexture = NULL;
    std::string textureID;

    if (gP->getParameter("objcolor",textureID)) {
        if (textureID=="gtTexture") {
            currTexture = gpTexture[gpTexture.size()-1];
        } else {
            if (gpTexture.size()>=1) {
                getIDptr(gP,"init-texture","Texture","objcolor",gtTexture);
                currTexture = gpTexture[(gpTypeIDptr->second).vectorID];
            } else {
                scheme_error("init-texture: no texture available!\n");
            }
        }
    } else {
        if (gpTexture.empty()) scheme_error("init-texture: no texture available!\n");
        else if (gpTexture.size()>1)
            scheme_error("init-texture: textur-ID is missing!\n");
        else
            currTexture = gpTexture[(gpTypeIDptr->second).vectorID];
#ifdef GVS_VERBOSE
        printf("init-shader: use proper texture\n");
#endif
    }
    if (currTexture!=NULL) {
        shader->setPrimitiveColor(currTexture);
    }

    
    GvsTexture* currNormalMap = NULL;
    if (gP->getParameter("normalmap",textureID)) {
        if (textureID=="gtTexture") {
            currNormalMap = gpTexture[gpTexture.size()-1];
        } else {
            if (gpTexture.size()>=1) {
                getIDptr(gP,"init-texture","Texture","normalmap",gtTexture);
                currNormalMap = gpTexture[(gpTypeIDptr->second).vectorID];
            } else {
                scheme_error("init-texture: no texture available!\n");
            }
        }

        if (currNormalMap!=NULL) {
            shader->setNormalMap(currNormalMap);
        }
    }
    
    // request special parameters
    double ambient, diffuse;
    if (!gP->getParameter("ambient",ambient))  ambient = 1.0;
    if (!gP->getParameter("diffuse",diffuse))  diffuse = 0.0;

    shader->setKambient(ambient);
    shader->setKdiffuse(diffuse);

    gpShader.push_back(shader);
    //shader->Print();

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-shader: ID already assigned!");
    }

    GvsTypeID tid = {gtShader,static_cast<int>(gpShader.size())-1,gpTexture[gpTexture.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}


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
        (init-texture '(type  "UniTex")
                      '(color #(double double double) )
                    [ '(colorgray 0.3) ]
        )

        (init-texture '(type  "CheckerT2D")
                      '(texture "texID")
                      '(texture "texID")
                      `(transform ,(scale-obj #(2.0 2.0)))
                      '(id "tex1")
        )

        (init-texture '(type "DieT2D")
                      '(texture "texID")
                      '(texture "texID")
                      '(id "tex1")
        )

        (init-texture '(type  "ChequeredT2D")
                      '(texture "texID")
                      '(texture "texID")
                      '(width  0.1)
                      `(transform ,(scale-obj #(2.0 2.0)))
                      '(id "tex1")
        )

        (init-texture '(type "Image")
                      '(file "filename")
                      `(transform ,(scale-obj #(2.0 2.0)))
                      '(id "imageTex")
        )

        (init-texture '(type "ColorGradTex")
                      '(color1 #(double double double))
                      '(color2 #(double double double))
                    [ '(start #(double double)) ]
                    [ '(end   #(double double)) ]
                    [ '(gradtype "linear") ]
                      '(id "colorGrad")
        )
 */


#include "parse_texture.h"
#include "parse_helper.h"

#include <GvsGlobalDefs.h>

#include "Img/GvsColor.h"
#include "Img/GvsChannelImg2D.h"
#include "Img/GvsPicIOEnvelope.h"
#include "Parser/GvsParseScheme.h"
#include "Texture/GvsTexture.h"
#include "Texture/GvsUniTex.h"
#include <Texture/GvsImg2DSampler.h>

#include <Texture/GvsCheckerT2D.h>
#include <Texture/GvsDieT2D.h>
#include <Texture/GvsChequeredT2D.h>
//#include <Texture/2D/GvsChequeredTimeT2D.h>
#include <Texture/GvsColorGradTex.h>

#include "math/Mat.h"


extern std::vector<GvsTexture*>        gpTexture;
extern std::vector<GvsChannelImg2D*>   gpChannelImg2D;

extern std::map<std::string,GvsTypeID>  gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;

/**
 * @brief gvsP_init_texture
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_texture (scheme *sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n.....gvsP_init_texture.....\n";
#endif
    if (args == sc->NIL) scheme_error("init-texture: no arguments");
    if (!is_pair(args)) scheme_error("init-texture: less arguments");

    std::string allowedNames[] = {"type","id","color","colorgray","transform"};
    GvsParseAllowedNames allowedTypes[] = {{gp_string_string,0}, // type
                                           {gp_string_string,0}, // id
                                           {gp_string_double,3}, // color
                                           {gp_string_double,1}, // colorgray
                                           {gp_string_matrix,0}, // transform
                                          };
    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,5);

    // Einlesen der Argumente der Textur
    args = gvsParser->parse(args);

    // Durchsuche Parameter nach dem Typ der Texture
    std::string textureType;
    gvsParser->getParameter("type",textureType);

    if      ( textureType == "UniTex" )      gvsP_init_unitex(gvsParser);
    else if ( textureType == "CheckerT2D" )  gvsP_init_checkerT2D(gvsParser);
    else if ( textureType == "DieT2D" )      gvsP_init_dieT2D(gvsParser);
    else if ( textureType == "ChequeredT2D" )      gvsP_init_chequeredT2D(gvsParser);
    //else if ( textureType == "ChequeredTimeT2D" )  gvsP_init_chequeredTimeT2D(gvsParser);
    else if ( textureType == "Image"        )      gvsP_init_image2dsampler(gvsParser);
    else if ( textureType == "ColorGradTex")  gvsP_init_colorGradTex(gvsParser);

    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtTexture"));
    return R;
}

/**
 * @brief gvsP_init_unitex
 * @param gP
 */
void gvsP_init_unitex (GvsParseScheme* gP) {

    GvsUniTex* unitex;
    double     col[3] = { 0.0, 0.0, 0.0 };

    if (gP->getParameter("colorgray",col)) {
        unitex = new GvsUniTex(col[0]);
    }

    if (gP->getParameter("color",col)) {
        unitex = new GvsUniTex(GvsColor(col[0],col[1],col[2]));
    }
    gpTexture.push_back(unitex);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-texture: ID already assigned!");
    }

    GvsTypeID tid = {gtTexture,static_cast<int>(gpTexture.size())-1,gpTexture[gpTexture.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}

/**
 * @brief gvsP_init_checkerT2D
 * @param gP
 */
void gvsP_init_checkerT2D (GvsParseScheme* gP) {
    GvsCheckerT2D* texT2D;
    GvsTexture* currTex[2];

    gP->setAllowedName("texture",gp_string_string,0);

    int num = 0;
    int count = 0;
    bool texFound = false;
    std::string textureID;
    std::string msg;

    do {
        texFound = gP->getParameter("texture",textureID,num);
        if (texFound) {
            if (textureID=="gtTexture") {
                currTex[count] = gpTexture[gpTexture.size()-1];
            }
            else {
                if (gpTexture.size()>=1) {
                    getIDptr(gP,"CheckerT2D","Texture","texture",gtTexture,num);
                    currTex[count] = gpTexture[(gpTypeIDptr->second).vectorID];
                }
                else
                    scheme_error("init-shader: no texture available!");
            }
        }
        else {
            if (gpTexture.empty()) scheme_error("init-shader: no texture available!");
            else if (gpTexture.size()>1)
            {
                scheme_error("init-shader: texture-ID is missing!\n");
            }
            else
                currTex[count] = gpTexture[0];
#ifdef GVS_VERBOSE
            printf("init-shader: verwende eingegebene Textur\n");
#endif
        }
        //    currTex[count]->print(cerr);
        count++;
        num++;
    }
    while ( (count<2) );

    texT2D = new GvsCheckerT2D(currTex[0],currTex[1]);

    // Transformation
    m4d::Matrix<double,2,3> texMat2D; // = ScaleMat2D(2.0,2.0);
    if (!gP->getParameter("transform",&texMat2D)) texMat2D.setIdent();
    texT2D->setTransformation(texMat2D);

    gpTexture.push_back(texT2D);
     //texT2D->Print();

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-texture: ID already assigned!");
    }

    GvsTypeID tid = {gtTexture,static_cast<int>(gpTexture.size())-1,gpTexture[gpTexture.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}


void gvsP_init_dieT2D( GvsParseScheme* gP ) {
    GvsDieT2D* texT2D;
    GvsTexture* currTex[2];

    gP->setAllowedName("texture",gp_string_string,0);

    int num = 0;
    int count = 0;
    bool texFound = false;
    std::string textureID;
    std::string msg;

    do {
        texFound = gP->getParameter("texture",textureID,num);
        if (texFound) {
            if (textureID=="gtTexture") {
                currTex[count] = gpTexture[gpTexture.size()-1];
            }
            else {
                if (gpTexture.size()>=1) {
                    getIDptr(gP,"CheckerT2D","Texture","texture",gtTexture,num);
                    currTex[count] = gpTexture[(gpTypeIDptr->second).vectorID];
                }
                else
                    scheme_error("init-shader: no texture available!");
            }
        }
        else {
            if (gpTexture.empty()) scheme_error("init-shader: no texture available!");
            else if (gpTexture.size()>1)
            {
                scheme_error("init-shader: texture-ID is missing!\n");
            }
            else
                currTex[count] = gpTexture[0];
#ifdef GVS_VERBOSE
            printf("init-shader: verwende eingegebene Textur\n");
#endif
        }
        //    currTex[count]->print(cerr);
        count++;
        num++;
    }
    while ( (count<2) );

    texT2D = new GvsDieT2D(currTex[0],currTex[1]);

    // Transformation
    m4d::Matrix<double,2,3> texMat2D; // = ScaleMat2D(2.0,2.0);
    if (!gP->getParameter("transform",&texMat2D)) texMat2D.setIdent();
    texT2D->setTransformation(texMat2D);

    gpTexture.push_back(texT2D);
     //texT2D->Print();

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-texture: ID already assigned!");
    }

    GvsTypeID tid = {gtTexture,static_cast<int>(gpTexture.size())-1,gpTexture[gpTexture.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}


void gvsP_init_chequeredT2D( GvsParseScheme* gP ) {
    GvsChequeredT2D* texT2D;
    GvsTexture* currTex[2];

    gP->setAllowedName("width",gp_string_double,1);
    gP->setAllowedName("texture",gp_string_string,0);

    int num = 0;
    int count = 0;
    bool texFound = false;
    std::string textureID;

    double width = 0.0;
    gP->getParameter("width",width);

    //std::string msg;

    do {
        texFound = gP->getParameter("texture",textureID,num);
        if (texFound) {
            if (textureID=="gtTexture") {
                currTex[count] = gpTexture[gpTexture.size()-1];
            }
            else {
                if (gpTexture.size()>=1) {
                    getIDptr(gP,"CheckerT2D","Texture","texture",gtTexture,num);
                    currTex[count] = gpTexture[(gpTypeIDptr->second).vectorID];
                }
                else {
                    scheme_error("init-shader: no texture available!");
                }
            }
        }
        else {
            if (gpTexture.empty()) scheme_error("init-shader: no texture available!");
            else if (gpTexture.size()>1) {
                scheme_error("init-shader: texture-ID is missing!\n");
            }
            else
                currTex[count] = gpTexture[0];
#ifdef GVS_VERBOSE
            printf("init-shader: verwende eingegebene Textur\n");
#endif
        }
        //    currTex[count]->print(cerr);
        count++;
        num++;
    }
    while ( (count<2) );

    texT2D = new GvsChequeredT2D(currTex[0],currTex[1],width);

    // Transformation
    m4d::Matrix<double,2,3> texMat2D; // = ScaleMat2D(2.0,2.0);
    if (!gP->getParameter("transform",&texMat2D)) texMat2D.setIdent();
    texT2D->setTransformation(texMat2D);

    gpTexture.push_back(texT2D);
     //texT2D->Print();

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-texture: ID already assigned!");
    }

    GvsTypeID tid = {gtTexture,static_cast<int>(gpTexture.size())-1,gpTexture[gpTexture.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}


//----------------------------------------------------------------------------
//         gvsP_init_chequeredT2D
//----------------------------------------------------------------------------
/*
void gvsP_init_chequeredTimeT2D (GvsParseScheme* gP)
{
  GvsChequeredTimeT2D* texTimeT2D;
  GvsTexture* currTex[2];

  gP->setAllowedName("texture",gp_string_string,0);
  gP->setAllowedName("projector",gp_string_string,0);
  gP->setAllowedName("deltatbehaviour", gp_string_string,0);
  gP->setAllowedName("colorbehaviour",  gp_string_string,0);
  gP->setAllowedName("timebehaviour",   gp_string_string,0);
  gP->setAllowedName("deltat",gp_string_double,1);
  gP->setAllowedName("stretch", gp_string_double,1);


  int num = 0;
  int count = 0;
  bool texFound = false;
  string textureID;
  string msg;

  do
  {
    texFound = gP->getParameter("texture",textureID,num);

    if (texFound)
    {
      if (textureID=="gtTexture")
      {
        currTex[count] = gpTexture[gpTexture.size()-1];
      }
      else
      {
        if (gpTexture.size()>=1)
        {
          getIDptr(gP,"CheckerT2D","Texture","texture",gtTexture,num);
          currTex[count] = gpTexture[(gpTypeIDptr->second).vectorID];
        }
        else
          scheme_error("init-shader: keine Textur vorhanden!");
      }
    }
    else if (gpTexture.size()>1)
    {
      getIDptr(gP,"ChequeredT2D","Texture","texture",gtTexture,num);
      currTex[count] = gpTexture[(gpTypeIDptr->second).vectorID];
    }
    else
    {
      if (gpTexture.empty()) scheme_error("init-shader: keine Textur vorhanden!");
#ifdef GVS_VERBOSE
      printf("init-shader: verwende eingegebene Textur\n");
#endif
      currTex[count] = gpTexture[0];
    }
    //    currTex[count]->print(cerr);
    count++;
    num++;
  }
  while ( (count<2) );


  double deltat;
  if (!gP->getParameter("deltat",deltat)) deltat = 0.5;

  GvsProjector *proj = NULL;
  gpTypeIDptr = getIDptr(gP,"Device","Projector","projector",gtProjector);
  proj = gpProjector[(gpTypeIDptr->second).vectorID];


  texTimeT2D = new GvsChequeredTimeT2D(currTex[0],currTex[1],deltat, proj);

  string behaviour;
  if (gP->getParameter("deltatbehaviour",behaviour))
  {
    if (behaviour == string("deltat"))
      texTimeT2D->setBehaviour(useDeltaT);
    else if (behaviour == string("deltatlin"))
      texTimeT2D->setBehaviour(useDeltaTLin);
    else if (behaviour == string("scale"))
      texTimeT2D->setBehaviour(useScale);
    else
      scheme_error("init-texture: deltatbehaviour-Wert unbekannt!");
  }
  if (gP->getParameter("colorbehaviour",behaviour))
  {
    if (behaviour == string("specrend"))
      texTimeT2D->setBehaviour(useSpecrend);
    else if (behaviour == string("textures"))
      texTimeT2D->setBehaviour(useTextures);
    else
      scheme_error("init-texture: colorbehaviour-Wert unbekannt!");
  }
  if (gP->getParameter("timebehaviour",behaviour))
  {
    if (behaviour == string("coordtime"))
      texTimeT2D->setBehaviour(coordTime);
    else if (behaviour == string("localobstime"))
      texTimeT2D->setBehaviour(localObsTime);
    else if (behaviour == string("localintersectime"))
      texTimeT2D->setBehaviour(localIntersecTime);
    else
      scheme_error("init-texture: timebehaviour-Wert unbekannt!");
  }

  double stretch;
  if ( gP->getParameter("stretch",stretch) )
    texTimeT2D->setStretch(stretch);



  // Transformation
  m4d::Matrix<double,2,3> texMat2D; // = ScaleMat2D(2.0,2.0);
  if (!gP->getParameter("transform",&texMat2D)) texMat2D.setIdent();
  texTimeT2D->setTransformation(texMat2D);

  gpTexture.push_back(texTimeT2D);

  string idname = "unknown";
  if (!gP->getParameter("id",idname))
  {
    appendNum(idname,gpTypeID.size());
  }
  else if (gpTypeID.find(idname)!=gpTypeID.end())
  {
    scheme_error("init-texture: ID schon vergeben!");
  }
  gpTypeID.insert(pair<string,GvsTypeID>(idname,(GvsTypeID)
  {
    gtTexture,gpTexture.size()-1,gpTexture[gpTexture.size()-1]
  }
                                        ));
}
*/


//----------------------------------------------------------------------------
//         gvsP_init_image2dsampler
//----------------------------------------------------------------------------
void gvsP_init_image2dsampler ( GvsParseScheme* gP )
{
    GvsPicIOEnvelope* imgFile = new GvsPicIOEnvelope;
    GvsChannelImg2D*  chanImg = new GvsChannelImg2D;
    assert (chanImg!=NULL);

    gP->setAllowedName("file",gp_string_string,0);

    bool fileRead = false;
    std::string filename,msg;
    if (gP->getParameter("file",filename))
    {
        char file[256];
#ifdef _WIN32
		strcpy(file, filename.c_str());
#else
        strcpy(file,filename.c_str());
#endif
        fileRead = imgFile->readChannelImg(*chanImg,file);

        if (!fileRead) {
            msg = "Could not read file ";
            msg.append(filename);
            scheme_error(msg);
        }
    }

    GvsImg2DSampler* imgSampler = new GvsImg2DSampler(chanImg);

    m4d::Matrix<double,2,3> texMat2D;
    if (!gP->getParameter("transform",&texMat2D)) texMat2D.setIdent();
    imgSampler->setTransformation(texMat2D);


    gpTexture.push_back(imgSampler);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-texture: ID already assigned!");
    }

    GvsTypeID tid = {gtTexture,static_cast<int>(gpTexture.size())-1,gpTexture[gpTexture.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
    delete imgFile;
}


void gvsP_init_colorGradTex( GvsParseScheme* gP ) {
    GvsColorGradTex* texT2D;

    gP->setAllowedName("color1",gp_string_double,3);
    gP->setAllowedName("color2",gp_string_double,3);
    gP->setAllowedName("start",gp_string_double,2);
    gP->setAllowedName("end",gp_string_double,2);
    gP->setAllowedName("gradtype",gp_string_string,0);

    double col1[3] = { 0.0, 0.0, 0.0 };
    double col2[3] = { 0.0, 0.0, 0.0 };

    texT2D = new GvsColorGradTex();
    if (gP->getParameter("color1",col1)) {
        texT2D->SetColor1(GvsColor(col1[0],col1[1],col1[2]));
    }
    if (gP->getParameter("color2",col2)) {
        texT2D->SetColor2(GvsColor(col2[0],col2[1],col2[2]));
    }

    double pos1[2] = { 0.0, 0.5 };
    double pos2[2] = { 1.0, 0.5 };
    if (gP->getParameter("start",pos1)) {
        texT2D->SetStartPoint(m4d::vec2(pos1[0],pos1[1]));
    }

    if (gP->getParameter("end",pos2)) {
        texT2D->SetEndPoint(m4d::vec2(pos2[0],pos2[1]));
    }

    std::string gType = "linear";
    if (gP->getParameter("gradtype",gType)) {
        texT2D->SetGradType(gType);
    }
    //texT2D->Print();

    gpTexture.push_back(texT2D);

    std::string idname = "unknown";
    if (!gP->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-texture: ID already assigned!");
    }

    GvsTypeID tid = {gtTexture,static_cast<int>(gpTexture.size())-1,gpTexture[gpTexture.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));
}





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

#include <unistd.h>
#include "Dev/GvsDevice.h"
#include "Parser/GvsParser.h"
#include "Texture/GvsTexture.h"

#include "metric/m4dMetric.h"
#include "motion/m4dGeodesic.h"

/// globales Scheme-Objekt
scheme sc;

std::vector<GvsCamera*>          gpCamera;
std::vector<GvsProjector*>       gpProjector;

std::vector<Gvsm4dMetricDummy*>  gpMetric;
std::vector<GvsGeodSolver*>      gpSolver;

std::vector<GvsLocalTetrad*>     gpLocalTetrad;
std::vector<GvsLocalCompObj*>    gpLocalCompObj;

std::vector<GvsSolidObj*>          gpSolidObj;
std::vector<GvsSolidCSGObj*>       gpSolidCSGObj;
std::vector<GvsSolidDifferObj*>    gpSolidDifferObj;
std::vector<GvsSolidIntersecObj*>  gpSolidIntersecObj;
std::vector<GvsSolidUnifiedObj*>   gpSolidUnifiedObj;

std::vector<GvsChannelImg2D*>    gpChannelImg2D;
std::vector<GvsDevice*>          gpDevice;
std::vector<GvsLightSrc*>        gpLight;
std::vector<GvsLightSrcMgr*>     gpLightMgr;
std::vector<GvsStMotion*>        gpMotion;
std::vector<GvsRayGen*>          gpRayGen;
std::vector<GvsSceneObj*>        gpSceneObj;
std::vector<GvsShader*>          gpShader;
std::vector<GvsTexture*>         gpTexture;

std::map<std::string,GvsTypeID>            gpTypeID;
std::map<std::string,GvsTypeID>::iterator  gpTypeIDptr;



GvsParser :: GvsParser() {
}

GvsParser :: ~GvsParser()
{
#ifdef GVS_VERBOSE
    std::cerr << "\n.....clear parser!!.....\n";
#endif
    deleteAll();
    scheme_deinit(&sc);
}

/**
 * @brief GvsParser::deleteAll
 */
void GvsParser::deleteAll() {
    register unsigned int i;

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all cameras...\n");
#endif
    for (i=0;i<gpCamera.size();i++) {
        delete gpCamera[i];
    }
    gpCamera.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all projectors...\n");
#endif
    for (i=0;i<gpProjector.size();i++) {
        delete gpProjector[i];
    }
    gpProjector.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all metrics...\n");
#endif
    for (i=0;i<gpMetric.size();i++) {
        if (gpMetric[i]!=NULL) {
            delete gpMetric[i];
            gpMetric[i] = NULL;
        }
    }
    gpMetric.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all solvers...\n");
#endif
    for (i=0;i<gpSolver.size();i++) {
        if (gpSolver[i]!=NULL) {
            delete gpSolver[i];
            gpSolver[i] = NULL;
        }
    }
    gpSolver.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all tetrads...\n");
#endif
    for (i=0;i<gpLocalTetrad.size();i++) {
        if (gpLocalTetrad[i]!=NULL) {
            delete gpLocalTetrad[i];
            gpLocalTetrad[i] = NULL;
        }
    }
    gpLocalTetrad.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all local objects...\n");
#endif
    for (i=0;i<gpLocalCompObj.size();i++) {
        if (gpLocalCompObj[i]!=NULL) {
            delete gpLocalCompObj[i];
            gpLocalCompObj[i] = NULL;
        }
    }
    gpLocalCompObj.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all motions...\n");
#endif
    for (i=0;i<gpMotion.size();i++) {
        if (gpMotion[i]!= NULL) {
            delete gpMotion[i];
            gpMotion[i] = NULL;
        }
    }
    gpMotion.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all ray generators...\n");
#endif
    for (i=0;i<gpRayGen.size();i++) {
        delete gpRayGen[i];
    }
    gpRayGen.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all scene objects...\n");
#endif
    for (i=0;i<gpSceneObj.size();i++) {
        if (gpSceneObj[i]!=NULL) {
            delete gpSceneObj[i];
            gpSceneObj[i] = NULL;
        }
    }
    gpSceneObj.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all shaders...\n");
#endif
    for (i=0;i<gpShader.size();i++) {
        if (gpShader[i]!=NULL) {
            delete gpShader[i];
            gpShader[i] = NULL;
        }
    }
    gpShader.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all textures...\n");
#endif
    for (i=0;i<gpTexture.size();i++) {
        if (gpTexture[i]!=NULL) {
            delete gpTexture[i];
            gpTexture[i] = NULL;
        }
    }
    gpTexture.clear();

#ifdef GVS_VERBOSE
    fprintf(stderr,"GvsParser: delete all light sources...\n");
#endif
    for (i=0;i<gpLightMgr.size();i++) {
        delete gpLightMgr[i];
        gpLightMgr[i] = NULL;
    }
    gpLightMgr.clear();

    /*
    fprintf(stderr,"GvsParser: delete all devices...\n");
    for (i=0;i<gpDevice.size();i++) {
        if (gpDevice[i]!=NULL) {
            delete gpDevice[i];
            gpDevice[i] = NULL;
        }
    }
    gpDevice.clear();
    */
}


/**
 * @brief GvsParser::getMetric
 * @param k
 * @return
 */
m4d::Metric* GvsParser::getMetric( unsigned int k ) {
    if (gpMetric.size()==0) scheme_error("No metric available!");

    assert ( k < gpMetric.size() );
    return gpMetric[k]->m4dMetric;
}

GvsGeodSolver* GvsParser::getSolver( unsigned int k ) {
    if (gpSolver.size()==0) scheme_error("No GeodSolver available!");

    assert ( k < gpSolver.size() );
    return gpSolver[k];
}


GvsCamera* GvsParser::getCamera( unsigned int k ) {
    if (gpCamera.size()==0) scheme_error("No camera available!");

    assert ( k < gpCamera.size() );
    return gpCamera[k];
}

GvsProjector* GvsParser :: getProjector ( unsigned int k ) {
  if (gpProjector.size()==0) scheme_error("No projector available!");

  assert ( k < gpProjector.size() );
  return gpProjector[k];
}

/**
 * @brief GvsParser::getRayGen
 * @param k
 * @return
 */
GvsRayGen*
GvsParser :: getRayGen ( unsigned int k )
{
  if (gpRayGen.size()==0) scheme_error("Kein Strahlgenerator vorhanden!");

  assert ( k < gpRayGen.size() );
  return gpRayGen[k];
}

/**
 * @brief GvsParser::getLightMgr
 * @param k
 * @return
 */
GvsLightSrcMgr*
GvsParser :: getLightMgr ( unsigned int k )
{
  if (gpLightMgr.size()==0) scheme_error("Keine Lichtquellen-Manager vorhanden!");

  assert ( k < gpLightMgr.size() );
  return gpLightMgr[k];
}

/**
 * @brief GvsParser::getLight
 * @param k
 * @return
 */
GvsLightSrc*
GvsParser :: getLight ( unsigned int k )
{
  if (gpLight.size()==0) scheme_error("Keine Lichtquelle vorhanden!");

  assert ( k < gpLight.size() );
  return gpLight[k];
}

/**
 * @brief GvsParser::getSceneObj
 * @param k
 * @return
 */
GvsSceneObj*
GvsParser :: getSceneObj ( unsigned int k )
{
  if (gpSceneObj.size()==0) scheme_error("Kein Szene-Objekt vorhanden!");

  assert ( k < gpSceneObj.size() );
  return gpSceneObj[k];
}

/**
 * @brief GvsParser::getMotion
 * @param k
 * @return
 */
GvsStMotion*
GvsParser :: getMotion ( unsigned int k )
{
  if (gpMotion.size()==0) scheme_error("Keine Bewegung vorhanden!");

  assert ( k < gpMotion.size() );
  return gpMotion[k];
}

/**
 * @brief GvsParser::getNumDevices
 * @return
 */
int GvsParser::getNumDevices() const {
    return gpDevice.size();
}

/**
 * @brief GvsParser::getDevice
 * @param device
 * @param k
 */
void GvsParser::getDevice( GvsDevice *device, unsigned int k ) {
    if (k >= gpDevice.size()) {
        fprintf(stderr,"Error: GvsParser::getDevice(): device '%d' exceeds number of available devices '%d'! Exiting now.\n",k,(int)gpDevice.size());
        exit(-1);
    }
    if (device==NULL) {
        fprintf(stderr,"Error: GvsParser::getDevice(): No device set! Exiting now.\n");
        exit(-1);
    }

    device->metric = gpDevice[k]->metric;
    //  device->solver = gpDevice[k]->solver;

    device->camera    = gpDevice[k]->camera;
    device->projector = gpDevice[k]->projector;

    device->lightSrcMgr = gpDevice[k]->lightSrcMgr;
    device->sceneGraph  = gpDevice[k]->sceneGraph;

    if (!device->mChangeObj.empty()) {
        device->mChangeObj.clear();
    }
    device->mChangeObj = gpDevice[k]->mChangeObj;
    device->isManual = gpDevice[k]->isManual;
    device->camEye = gpDevice[k]->camEye;
}

/**
 * @brief GvsParser::printAll
 * @param fptr
 */
void GvsParser::printAll ( FILE* fptr ) const {
    fprintf(fptr,"\n\nThe parser has read %d objects:\n",static_cast<int>(gpTypeID.size()));
    fprintf(fptr,"----------------------------------------------------------------\n");

    int i=0;
    gpTypeIDptr = gpTypeID.begin();
    do {
        fprintf(fptr,"#%3i:   ID-Name:%14s    (Typ:%14s) -->%3i\n",i,
                (gpTypeIDptr->first).c_str(),
                GvsTypeName[gpTypeIDptr->second.gvsType].c_str(),
                gpTypeIDptr->second.vectorID);
        gpTypeIDptr++;
        i++;
    }
    while (gpTypeIDptr!=gpTypeID.end());
    fprintf(fptr,"\n");
}

/**
 * @brief GvsParser::initStandard
 * @param device
 */
void
GvsParser :: initStandard ( GvsDevice *device ) {
    device->camera    = getCamera();
    device->projector = getProjector();

    device->metric = getMetric();
    //device->spacetime = getSpacetime();
    //  device->solver = getSolver();

    device->sceneGraph = getSceneObj(gpSceneObj.size()-1);
    device->lightSrcMgr = getLightMgr();

    //device->Print();
}

/**
 * @brief GvsParser::read_scene
 * @param name
 */
void GvsParser::read_scene(const char* name) {
    FILE *fin;
    FILE *fscm;
    std::string file_name = getFullPathname() + "Parser/init.scm";
    //std::cerr << file_name << std::endl;
    if (!scheme_init(&sc)) {
        fprintf(stderr,"Could not initialize!\n");
        exit(0);
    }

    scheme_set_input_port_file(&sc, stdin);
    scheme_set_output_port_file(&sc, stdout);

    // Init-File lesen
    fin=fopen(file_name.c_str(),"r");
    if (fin==0) {
        fprintf(stderr,"Could not open file %s\n",file_name.c_str());
        exit(-1);
    } else {
        scheme_load_file(&sc,fin);
    }

    // -------------------------
    //   Scheme definitions
    // -------------------------

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-metric"), mk_foreign_func(&sc, gvsP_init_metric));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-solver"), mk_foreign_func(&sc, gvsP_init_solver));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-camera"), mk_foreign_func(&sc, gvsP_init_camera));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "local-tetrad"),mk_foreign_func(&sc, gvsP_local_tetrad));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-projector"),mk_foreign_func(&sc, gvsP_init_projector));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-light-mgr"),mk_foreign_func(&sc, gvsP_init_lightmgr));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-light"),mk_foreign_func(&sc, gvsP_init_light));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-raygen"),mk_foreign_func(&sc, gvsP_init_raygen));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "calc-ray"),mk_foreign_func(&sc, gvsP_calc_ray));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "calc-proj-ray"),mk_foreign_func(&sc, gvsP_calc_proj_ray));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "comp-object"),mk_foreign_func(&sc, gvsP_compound_obj));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "local-comp-object"),mk_foreign_func(&sc, gvsP_local_comp_obj));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "add-object"),mk_foreign_func(&sc, gvsP_add_object));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-motion"), mk_foreign_func(&sc, gvsP_init_motion));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "gvs-print-motion"), mk_foreign_func(&sc, gvsP_print_motion));


    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "set-parameter"), mk_foreign_func(&sc, gvsP_set_parameter));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "get-parameter"), mk_foreign_func(&sc, gvsP_get_parameter));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "translate-obj"), mk_foreign_func(&sc, gvsP_translateObj));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "rotate-obj"),    mk_foreign_func(&sc, gvsP_rotateObj));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "scale-obj"),     mk_foreign_func(&sc, gvsP_scaleObj));


    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "solid-box"),       mk_foreign_func(&sc, gvsP_init_solidbox));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "solid-cylinder"),  mk_foreign_func(&sc, gvsP_init_solidcylinder));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "solid-ellipsoid"), mk_foreign_func(&sc, gvsP_init_solidellipsoid));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "solid-background"),mk_foreign_func(&sc, gvsP_init_solidbackground));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "solid-ring"),      mk_foreign_func(&sc, gvsP_init_solidring));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "plane-triangle"),mk_foreign_func(&sc, gvsP_init_planetriangle));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "plane-ring"),mk_foreign_func(&sc, gvsP_init_planering));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "mesh-obj"), mk_foreign_func(&sc, gvsP_init_OBJmesh));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "csg-object"),mk_foreign_func(&sc, gvsP_init_csg_obj));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-texture"),mk_foreign_func(&sc, gvsP_init_texture));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-shader"),mk_foreign_func(&sc, gvsP_init_shader));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "init-device"),mk_foreign_func(&sc, gvsP_init_device));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "vec3"), mk_foreign_func(&sc, gvsP_vec3));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "vec4"), mk_foreign_func(&sc, gvsP_vec4));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "set-changeobj"), mk_foreign_func(&sc, gvsP_set_changeObj));

    // -------- print --------
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "gvs-print"), mk_foreign_func(&sc, gvsP_print));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "m4d-metriclist"), mk_foreign_func(&sc, gvsP_m4d_metriclist));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "m4d-solverlist"), mk_foreign_func(&sc, gvsP_m4d_solverlist));

    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "getenv"), mk_foreign_func(&sc, gvsP_getenv));
    scheme_define(&sc, sc.global_env, mk_symbol(&sc, "exit"), mk_foreign_func(&sc, gvsP_exit));

    // read SDL file
    file_name = name;
    fscm = fopen(file_name.c_str(),"r");

    if ((fscm==0) || (file_name=="") ) {
        fprintf(stderr,"GvsParser :: Could not open file %s\n",file_name.c_str());
        exit(0);
    } else {
        scheme_load_file(&sc,fscm);
    }

    fclose(fscm);
    fclose(fin);
}


std::string GvsParser::getFullPathname() {
    std::string path;
    pid_t pid = getpid();
    char buf[20] = {0};
    sprintf(buf,"%d",pid);
    std::string _link = "/proc/";
    _link.append(buf);
    _link.append("/exe");
    char proc[512];
    int ch = readlink(_link.c_str(),proc,512);
    if (ch != 1) {
        proc[ch] = 0;
        path = proc;
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
        return path + std::string("/");
    }
    return std::string();
}

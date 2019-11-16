/**
 * @file    parse_helper.h
 * @author  Thomas Mueller
 *
    (gvs-print '(id "objectID"))

    (set-parameter '(id "idname")
                         '(paramName  paramValue)
    )

    (m4d-metriclist)        print metric list

    (m4d-solverlist)        print solver list

 *  This file is part of GeoViS.
 */
#ifndef GVS_PARSE_HELPER_H
#define GVS_PARSE_HELPER_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

#include "Obj/GvsBase.h"
#include "Shader/GvsShader.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "metric/m4dMetric.h"

class GvsParseScheme;

extern scheme sc;

enum GvsType {
    gtunknown = 0,
    gtCamera,
    gtDevice,
    gtGeodSolver,
    gtLightMgr,
    gtLight,
    gtLocTed,
    gtMetric,
    gtMotion,
    gtProjector,
    gtRayGen,
    gtSceneObj,
    gtShader,
    gtTexture
};

const std::string GvsTypeName[14] = { "gtUnknown", "gtCamera", "gtDevice", "gtGeodSolver", "gtLightMgr", "gtLight",
    "gtLocTed", "gtMetric", "gtMotion", "gtProjector", "gtRayGen", "gtSceneObj", "gtShader", "gtTexture" };

struct GvsTypeID {
    // string  name;
    GvsType gvsType;
    int vectorID;
    GvsBase* gvsObject;
};

struct GvsSetParamList // setparameter-Liste
{
    char* objectIDname;
    char* paramName;
    gvs_parameter param;
};

void scheme_error(const std::string& msg);

// ----------- String-Funktionen -------------
void lowCase(std::string& s);
std::string getLowCase(std::string s);
void appendNum(std::string& s, int num);

int searchCharArray(const char** stringArray, const int num, const char* searchItem);
int searchStringArray(const std::string* stringArray, const int num, const char* searchItem);

std::string getType(pointer arg); //!< type of scheme element

pointer gvsP_print(scheme* sc, pointer args);
pointer gvsP_m4d_metriclist(scheme* sc, pointer args);
pointer gvsP_m4d_solverlist(scheme* sc, pointer args);
pointer gvsP_set_parameter(scheme* sc, pointer args);
pointer gvsP_get_parameter(scheme* sc, pointer args);
pointer gvsP_exit(scheme* sc, pointer args);
pointer gvsP_getenv(scheme* sc, pointer args);

std::map<std::string, GvsTypeID>::iterator getIDptr(GvsParseScheme* gpSc, const std::string& objName,
    const std::string& suchName, const char* token, const GvsType gtype, int num = 0);

std::map<std::string, GvsTypeID>::iterator getOneOfIDptr(GvsParseScheme* gpSc, const std::string& objName,
    const std::string& suchName, const char* token, const GvsType gtype1, const GvsType gtype2, GvsType& gtypeFound,
    int num = 0);

m4d::Metric* readMetric(const std::string& name, GvsParseScheme* gP);

GvsShader* readShader(const std::string& name, GvsParseScheme* gP);
void readMetric(const std::string& name, GvsParseScheme* gP, m4d::Metric* currMetric);

GvsDevice* readDevice(const std::string& name, GvsParseScheme* gP);

void get_double(pointer s_x, double* x, const std::string msg = "");
void get_int(pointer s_x, int* x, const std::string msg = "");
void get_char(pointer s_c, char* tx, const std::string msg = "");
void get_string(pointer s_s, std::string& s, const std::string msg = "");

void get_int_vec(pointer s_vec, int dim, int p[], const std::string msg = "");
void get_double_vec(pointer s_vec, int dim, double p[], const std::string msg = "");

void get_matrix_size(pointer s_vec, int& n, int& m);

// void get_matrix(pointer s_vec, Matrix<mType, n, m> *mat, const string msg = "");

/**
 *   Read a transformation matrix from scheme
 */
template <class mType, int n, int m>
void get_matrix(pointer s_vec, m4d::Matrix<mType, n, m>* mat, const std::string& msg)
{
    double* tempV = new double[m];
    if (!is_vector(s_vec)) {
        char buf[1024];
#ifdef _WIN32
        sprintf_s(buf, "%s: get_matrix: wrong format of matrix", msg.c_str());
#else
        sprintf(buf, "%s: get_matrix: wrong format of matrix", msg.c_str());
#endif
        scheme_error(buf);
    }

    for (int i = 0; i < n; i++) {
        get_double_vec(((*sc.vptr->vector_elem)(s_vec, i)), m, tempV, "get_matrix: read matrix");
        for (int j = 0; j < m; j++) {
            mat->setElem(i, j, tempV[j]);
        }
    }
    delete[] tempV;
}

/**
 * Convert a m4d matrix into a scheme pointer
 */
template <class mType, int n, int m> void mk_matrix(pointer* retMat, m4d::Matrix<mType, n, m>* inMat, const std::string)
{
    // column vector of row vectors...

#ifdef _WIN32
    pointer tempPointer;
#else
    UNUSED_ATTR pointer tempPointer;
#endif
    // pointer tempPointer;

    *retMat = ((*sc.vptr->mk_vector)(&sc, n));
    for (int i = 0; i < n; i++) {
        pointer vecRowPointer;
        vecRowPointer = ((*sc.vptr->mk_vector)(&sc, m));
        for (int j = 0; j < m; j++) {
            // fprintf(stderr,"%12.6f ",inMat->getElem(i,j));
            tempPointer
                = ((*sc.vptr->set_vector_elem)(vecRowPointer, j, ((*sc.vptr->mk_real)(&sc, inMat->getElem(i, j)))));
        }
        // fprintf(stderr,"\n");
        tempPointer = ((*sc.vptr->set_vector_elem)(*retMat, i, vecRowPointer));
    }
}

pointer gvsP_vec3(scheme* sc, pointer args);
pointer gvsP_vec4(scheme* sc, pointer args);

#endif

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
          (gvs-print '(id "objectID"))

          (set-parameter '(id "idname")
                         '(paramName  paramValue)
          )

          (m4d-metriclist)        print metric list

          (m4d-solverlist)        print solver list
 */

#include "parse_helper.h"
#include "GvsParseScheme.h"
#include <algorithm>
#include <sstream>

#include "metric/m4dMetric.h"
#include "metric/m4dMetricDatabase.h"
#include "motion/m4dMotionDatabase.h"

extern std::map<std::string,GvsTypeID>  gpTypeID;
extern std::map<std::string,GvsTypeID>::iterator gpTypeIDptr;

extern std::vector<Gvsm4dMetricDummy*>  gpMetric;
extern std::vector<GvsShader*>          gpShader;
extern std::vector<GvsDevice*>          gpDevice;


void scheme_error(const std::string& msg) {
    printf("Scheme error: %s\n", msg.c_str());
    exit(0);
}

void lowCase(std::string &s) {
    size_t l = s.length();
    for (size_t i=0;i<l;i++) s[i] = tolower(s[i]);
    // transform(s.begin(),s.end(),s.begin(),tolower);
}

std::string getLowCase ( std::string s ) {
    std::string s1 = s;
    size_t l = s.length();
    for (size_t i=0;i<l;i++) s1[i] = tolower(s[i]);
    //transform(s1.begin(),s1.end(),s1.begin(),tolower);
    return s1;
}

void appendNum ( std::string &s, int num ) {
    std::ostringstream buf;
    buf << s << num;
    s = buf.str();
}

int searchCharArray ( const char** stringArray, const int num, const char* searchItem ) {
    assert(stringArray!=NULL);
    for (int n=0;n<num;n++) {
        if (strcmp(stringArray[n],searchItem)==0) return n;
    }
    return -1;
}

int searchStringArray ( const std::string* stringArray, const int num, const char* searchItem ) {
    for (int n=0;n<num;n++) {
        //???
        if (stringArray[n]==searchItem) return n;
    }
    return -1;
}



std::string getType ( pointer arg ) {
    std::string typeName;
    int flag = ((arg)->_flag)&T_MASKTYPE;
    //  std::cerr << "                       flag: " << flag << std::endl;
    switch (flag)
    {
        case T_STRING  :
            typeName = "T_STRING";
            break;
        case T_NUMBER  :
            typeName = "T_NUMBER";
            break;
        case T_SYMBOL  :
            typeName = "T_SYMBOL";
            break;
        case T_PROC    :
            typeName = "T_PROC";
            break;
        case T_PAIR    :
            typeName = "T_PAIR";
            break;
        case T_CLOSURE :
            typeName = "T_CLOSURE";
            break;
        case T_CONTINUATION :
            typeName = "T_CONTINUATION";
            break;
        case T_FOREIGN   :
            typeName = "T_FOREIGN";
            break;
        case T_CHARACTER :
            typeName = "T_CHARACTER";
            break;
        case T_PORT      :
            typeName = "T_PORT";
            break;
        case T_VECTOR    :
            typeName = "T_VECTOR";
            break;
        case T_MACRO     :
            typeName = "T_MACRO";
            break;
        case T_PROMISE   :
            typeName = "T_PROMISE";
            break;
        case T_ENVIRONMENT  :
            typeName = "T_ENVIRONMENT";
            break;
        default:
            typeName = "UNKNOWN TYPE";
            break;
    }
    return typeName;
}

/**
 * @brief gvsP_print
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_print (scheme *sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_print..........\n";
#endif
    if (args == sc->NIL) scheme_error("gvs-print: no arguments");
    if (!is_pair(args)) scheme_error("gvs-print: less arguments");

    std::string allowedNames[1] = {"id"};
    GvsParseAllowedNames allowedTypes[1] = {{gp_string_string,0}};

    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,1);
    args = gvsParser->parse(args);

    std::string idName,msg;
    if (!gvsParser->getParameter("id",idName)) {
        scheme_error("gvs-print: ID is missing!");
    }

    gpTypeIDptr = gpTypeID.find(idName);
    if (gpTypeIDptr == gpTypeID.end()) {
        // ... wenn nicht
        msg = "ID: ";
        msg.append(idName);
        msg.append(" not available!");
        scheme_error(msg);
    }

    GvsBase* gvsObject = (gpTypeIDptr->second).gvsObject;
    gvsObject->Print();
    delete gvsParser;
    return (sc->NIL);
}


/**
 * @brief gvsP_m4d_metriclist
 * @param sc
 * @return
 */
pointer gvsP_m4d_metriclist ( scheme *sc, pointer  ) {
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_m4d_metriclist..........\n";
#endif
    m4d::MetricDatabase* database = m4d::MetricDatabase::getInstance();
    database->printMetricList();
    return sc->NIL;
}

/**
 * @brief gvsP_m4d_solverlist
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_m4d_solverlist ( scheme *sc, pointer args ) {
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_m4d_solverlist..........\n";
#endif
    m4d::IntegratorDatabase* database = m4d::IntegratorDatabase::getInstance();
    database->printIntegratorList();
    return sc->NIL;
}

/**
 * @brief gvsP_set_parameter
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_set_parameter (scheme *sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_set_parameter..........\n";
#endif
    std::string msg;

    if (args == sc->NIL) scheme_error("set-parameter: no arguments");
    if (!is_pair(args)) scheme_error("set-parameter: less arguments");

    std::string allowedNames[] = {"id"};
    GvsParseAllowedNames allowedTypes[] = {{gp_string_string,0}};

    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,1);
    args = gvsParser->parse(args);

    std::string idName;
    if (!gvsParser->getParameter("id",idName))     {
        scheme_error("set-parameter: ID is missing!");
    }

    gpTypeIDptr = gpTypeID.find(idName);
    if (gpTypeIDptr == gpTypeID.end()) {
        //  if not
        msg = "ID \"";
        msg.append(idName);
        msg.append("\" not available!");
        scheme_error(msg);
    }

    // ... if yes
    int numParams = gvsParser->getNumParam();
    std::string paramName;

    GvsBase* gvsObject = (gpTypeIDptr->second).gvsObject;

    for (int i=1; i<numParams; i++) {
        paramName = gvsParser->getParamName(i);
        //dim = gvsParser->getParamDim(i);
#ifdef GVS_VERBOSE
        std::cerr << "\nSetze " << paramName << " vom Typ ";
#endif        
        GvsParseParamType pType = gvsParser->getParamType(i);
        switch (pType)
        {
            case gp_unknown:
            case gp_string_gvs:
            case gp_string_matrix:
            case gp_string_string:
            case gp_string_bool:
            case gp_string_setparamlist:
                break;
            case gp_string_int:
            {
#ifdef GVS_VERBOSE
                std::cerr << "<integer>" << std::endl;
#endif
                int ival;
                if (gvsParser->getParameter(paramName.c_str(),ival))
                {
                    gvsObject->SetParam(paramName,ival);
                }
                else
                {
                    msg = "set-parameter: parameter ";
                    msg.append(paramName);
                    msg.append(" not available!");
                    scheme_error(msg);
                }
                break;
            }
            case  gp_string_double :
            {
#ifdef GVS_VERBOSE
                std::cerr << "<double>" << std::endl;
#endif

                if (!gvsObject->IsValidParamName(paramName))
                {
                    msg = "set-parameter: parameter ";
                    msg.append(paramName);
                    msg.append(" not available!");
                    scheme_error(msg);
                }

                GvsDataType dataType = gvsObject->GetDataType(paramName);

                double dval;
                double* ddval = new double[4];
                int dim = gvsParser->getParamDim(paramName.c_str());
                //cerr << "dim: " << dim << std::endl;
                if (dim==1)
                {
                    if (gvsParser->getParameter(paramName.c_str(),dval))
                    {
                        gvsObject->SetParam(paramName,dval);
                    }
                }
                else if (dim==3)
                {
                    if (gvsParser->getParameter(paramName.c_str(),ddval))
                    {
                        std::cerr << "DT:" << GvsDataTypeName[dataType] << std::endl;
                        if (dataType==gvsDT_VEC3)
                            gvsObject->SetParam(paramName,m4d::vec3(ddval[0],ddval[1],ddval[2]));
                    }
                }
                else if (dim==4)
                {
                    if (gvsParser->getParameter(paramName.c_str(),ddval))
                    {
                        if (dataType==gvsDT_VEC4)
                            gvsObject->SetParam(paramName,m4d::vec4(ddval[0],ddval[1],ddval[2],ddval[3]));
                    }
                }
                else
                {
                    msg = "set-parameter: parameter ";
                    msg.append(paramName);
                    msg.append(" has wrong dimension!");
                    scheme_error(msg);
                }

                break;
            }
        }
    }

    delete gvsParser;
    return sc->NIL;
}


/**
 * @brief gvsP_get_parameter
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_get_parameter (scheme *sc, pointer args)
{
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_get_parameter..........\n";
#endif
    std::string msg;

    if (args == sc->NIL) scheme_error("get-parameter: no argumente");
    if (!is_pair(args)) scheme_error("get-parameter: less arguments");

    std::string allowedNames[] = {"id"};
    GvsParseAllowedNames allowedTypes[] = {{gp_string_string,0}};

    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,1);
    args = gvsParser->parse(args);

    std::string idName;
    if (!gvsParser->getParameter("id",idName))
    {
        scheme_error("get-parameter: ID is missing!");
    }

    // Suche, ob ID in der Liste ist ...
    gpTypeIDptr = gpTypeID.find(idName);

    if (gpTypeIDptr == gpTypeID.end())
    {
        // ... wenn nicht
        msg = "ID: ";
        msg.append(idName);
        msg.append(" not available!");
        scheme_error(msg);
    }

    // ... wenn ja
    GvsBase* gvsObject = (gpTypeIDptr->second).gvsObject;

    std::string paramName;
    GvsDataType datType;
    int numParams = gvsObject->GetNumParams();

    for (int i=0; i<numParams; i++)
    {
        paramName = gvsObject->GetParamName(i);
        datType   = gvsObject->GetDataType(paramName);
        fprintf(stderr,"%20s : %s\n",paramName.c_str(),GvsDataTypeName[datType].c_str());
    }

    delete gvsParser;
    return sc->NIL;
}


pointer gvsP_exit( scheme *sc, pointer ) {
    exit(1);
}


pointer gvsP_getenv( scheme *sc, pointer args ) {
    if (args == sc->NIL) scheme_error("getenv: no argumente");
    if (!is_pair(args)) scheme_error("getenv: less arguments");

    std::string envName;
    get_string(pair_car(args),envName);
    //std::cerr << getenv(envName.c_str()) << std::endl;

#ifdef _WIN32
	char* buf = nullptr;
	size_t sz = 0;
	if (_dupenv_s(&buf, &sz, envName.c_str()) == 0 && buf != nullptr) {
		printf("EnvVarName = %s\n", buf);
		std::string ename = std::string(buf);
		free(buf);
		return  (sc->vptr->mk_string)(sc, ename.c_str());
	}
	return sc->NIL;
#else
    return (sc->vptr->mk_string)(sc,getenv(envName.c_str()));
#endif
}


//  Bsp: getIDptr( gvsParser, "GeodSolver", "Metrik", "metric", gtMetric );
//
std::map<std::string,GvsTypeID>::iterator  getIDptr ( GvsParseScheme* gpSc,
                                                      const std::string objName,
                                                      const std::string suchName,
                                                      const char* token,
                                                      const GvsType gtype,
                                                      int num)
{
    // std::cerr << "getIDptr: " << token << std::endl;
    std::string idname,msg;
    if (!gpSc->getParameter(token,idname,num)) {
        msg = objName;
        msg.append(": several objects of type \"");
        msg.append(suchName);
        msg.append("\" available; ID is missing!");
        scheme_error(msg);
    }

    // Search if idname is in the list...
    gpTypeIDptr = gpTypeID.find(idname);
    if (gpTypeIDptr == gpTypeID.end()) {
        // ... if not
        msg = objName;
        msg.append(": ");
        msg.append(suchName);
        msg.append(" with ID \"");
        msg.append(idname);
        msg.append("\" not available!");
        scheme_error(msg);
    }

    if ( (gpTypeIDptr->second).gvsType != gtype )
    {
        msg = objName;
        msg.append(": object with ID \"");
        msg.append(idname);
        msg.append("\" is not of type \"");
        msg.append(suchName);
        msg.append("\"");
        scheme_error(msg);
    }
    return gpTypeIDptr;
}


std::map<std::string,GvsTypeID>::iterator  getOneOfIDptr ( GvsParseScheme* gpSc,
                                                           const std::string objName,
                                                           const std::string suchName,
                                                           const char* token,
                                                           const GvsType gtype1, const GvsType gtype2, GvsType &gtypeFound,
                                                           int num)
{
    // std::cerr << "getOneOfIDptr: " << token << std::endl;
    std::string idname,msg;
    if (!gpSc->getParameter(token,idname,num))
    {
        std::cerr << num << " " << idname << std::endl;
        msg = objName;
        msg.append(": several objects of type: ");
        msg.append(suchName);
        msg.append(" exist; ID is missing!");
        scheme_error(msg);
    }
    //  std::cerr << "ID-NAME: " << idname << std::endl;

    // Suche, ob idname in der Liste ist ...
    gpTypeIDptr = gpTypeID.find(idname);

    if (gpTypeIDptr == gpTypeID.end())
    {
        // ... if not
        msg = objName;
        msg.append(": ");
        msg.append(suchName);
        msg.append(" with ID \"");
        msg.append(idname);
        msg.append("\" not available!");
        scheme_error(msg);
    }

    gtypeFound = (gpTypeIDptr->second).gvsType;
    if ( ((gpTypeIDptr->second).gvsType != gtype1) && ((gpTypeIDptr->second).gvsType != gtype2) )
    {
        msg = objName;
        msg.append(": object with ID \"");
        msg.append(idname);
        msg.append("\" is not of type \"");
        msg.append(suchName);
        msg.append("\"");
        scheme_error(msg);
    }
    return gpTypeIDptr;
}


//----------------------------------------------------------------------------
//         readMetric
//----------------------------------------------------------------------------
m4d::Metric* readMetric ( const std::string name, GvsParseScheme* gP ) {
    m4d::Metric* currMetric;
    std::string msg, metricID;

    if (gP->getParameter("metric",metricID)) {
       // std::cerr << metricID << std::endl;

        gpTypeIDptr = gpTypeID.find(metricID);
        //if (metricID=="gtMetric") {
        if (gpTypeIDptr != gpTypeID.end()) {
            //gtypeFound = (gpTypeIDptr->second).gvsType;
            //std::cerr << gtypeFound << std::endl;
            currMetric = gpMetric[(gpTypeIDptr->second).vectorID]->m4dMetric;
        } else {
            msg = name;
            msg.append(": object with ID \"");
            msg.append(metricID);
            msg.append("\" is no metric!");
            scheme_error(msg);
        }
    } else if (gpMetric.size()>1) {
        getIDptr(gP,name,"Metric","metric",gtMetric);
        currMetric = gpMetric[(gpTypeIDptr->second).vectorID]->m4dMetric;
    }
    else
    {
        if (gpMetric.empty()) {
            msg = name;
            msg.append(": metric is missing\n");
            scheme_error(msg);
        }
#ifdef GVS_VERBOSE
        std::cerr << name << ": use entered metric\n";
#endif
        currMetric = gpMetric[0]->m4dMetric;
    }
    //

    return currMetric;
}

//----------------------------------------------------------------------------
//         readDevice
//----------------------------------------------------------------------------
GvsDevice* readDevice( const std::string name, GvsParseScheme* gP ) {
    GvsDevice* currDevice = NULL;
    std::string msg, deviceID;

    if (gP->getParameter("device",deviceID)) {

        gpTypeIDptr = gpTypeID.find(deviceID);
        if (gpTypeIDptr != gpTypeID.end()) {
            currDevice = gpDevice[(gpTypeIDptr->second).vectorID];
        } else {
            msg = name;
            msg.append(" device with ID \"");
            msg.append(deviceID);
            msg.append("\" is no device!");
            scheme_error(msg);
        }
    }
    else if (gpDevice.size()>1) {
        getIDptr(gP,name,"Device","device",gtDevice);
        currDevice = gpDevice[(gpTypeIDptr->second).vectorID];
    }
    else {
        if (gpDevice.empty()) {
            msg = name;
            msg.append(": device is missing\n");
            scheme_error(msg);
        }
        currDevice = gpDevice[0];
    }
    return currDevice;
}


//----------------------------------------------------------------------------
//         readShader
//----------------------------------------------------------------------------
GvsShader* readShader ( const std::string name, GvsParseScheme* gP ) {
    GvsShader* currShader;

    std::string msg, shaderID;
    if (gP->getParameter("shader",shaderID)) {
        if (shaderID=="gtShader") {
            currShader = gpShader[gpShader.size()-1];
        }
        else {
            if (gpShader.size()>=1) {
                getIDptr(gP,name,"Shader","shader",gtShader);
                currShader = gpShader[(gpTypeIDptr->second).vectorID];
            }
            else {
                msg = name;
                msg.append(": no shader available!");
                scheme_error(msg);
            }
        }
    }
    else {
        if (gpShader.empty()) {
            msg = name;
            msg.append(": no shader available!");
            scheme_error(msg);
        }
        else if (gpShader.size()>1) {
            msg = name;
            msg.append(": Shader-ID is missing!");
            scheme_error(msg);
        }
        else {
            currShader = gpShader[0];
        }
#ifdef GVS_VERBOSE
        std::cerr << name << ": use entered shader\n";
#endif
    }

    return currShader;
}


/**
 * @brief get_double
 * @param s_x
 * @param x
 * @param msg
 */
void get_double ( pointer s_x, double* x, const std::string msg )
{
    if (!is_number(s_x)) {
        std::string outmsg = msg;
        outmsg.append(": double expected! ");
        outmsg.append(getType(s_x));
        outmsg.append(" found.");
        scheme_error(outmsg);
    }
    *x = rvalue(s_x);
}


//----------------------------------------------------------------------------
//       get_int
//----------------------------------------------------------------------------
void get_int ( pointer s_x, int* x, const std::string msg )
{
    if (!is_number(s_x)) scheme_error(msg + ": integer expected");
    *x = ivalue(s_x);
}


//----------------------------------------------------------------------------
//       get_char
//----------------------------------------------------------------------------
void get_char ( pointer s_c, char* tx, const std::string msg )
{
    if (!is_character(s_c)) scheme_error(msg + ": character expected");
    tx = string_value(s_c);
    //  std::cerr << tx << std::endl;
}


//----------------------------------------------------------------------------
//       get_string
//----------------------------------------------------------------------------
void get_string ( pointer s_s, std::string& s, const std::string msg )
{
    if (!is_string(s_s)) scheme_error(msg + ": string expected");
    s = string_value(s_s);
}


//----------------------------------------------------------------------------
//       get_int_vec
//----------------------------------------------------------------------------
void get_int_vec ( pointer s_vec, int dim, int p[], const std::string msg )
{
    if (!is_vector(s_vec)) scheme_error(msg + ": vector value expected");
    if ((*sc.vptr->vector_length)(s_vec) != dim) scheme_error(msg + ": point is of wrong dimension");

    for (int i=0; (i<dim)&(i<4); i++)
    {
        pointer elem = (*sc.vptr->vector_elem)(s_vec,i);
        if (is_number(elem))  p[i] = ivalue(elem);
        else scheme_error(msg + ": point has non-numeric element");
    }
}

//----------------------------------------------------------------------------
//       get_double_vec
//----------------------------------------------------------------------------
void get_double_vec ( pointer s_pnd, int dim, double p[], const std::string msg )
{
    if (!is_vector(s_pnd)) scheme_error(msg + ": vector value expected");
    if ((*sc.vptr->vector_length)(s_pnd) != dim) scheme_error(msg + ": point is of wrong dimension");

    for (int i=0; (i<dim)&&(i<4); i++)
    {
        pointer elem = (*sc.vptr->vector_elem)(s_pnd,i);
        if (is_number(elem))  p[i] = rvalue(elem);
        else scheme_error(msg + ": point has non-numeric element");
    }
}

//----------------------------------------------------------------------------
//       get_matrix_size
//----------------------------------------------------------------------------
void get_matrix_size ( pointer s_vec, int &n, int &m )
{
    if (!is_vector(s_vec)) scheme_error("get_matrix_size: no matrix");

    n = (*sc.vptr->vector_length)(s_vec);
    pointer s_vec_elem = (*sc.vptr->vector_elem)(s_vec,0);

    if (!is_vector(s_vec_elem)) scheme_error("get_matrix_size: no matrix");
    m = (*sc.vptr->vector_length)(s_vec_elem);
}



pointer gvsP_vec3 (scheme *sc, pointer args) {
    pointer s_pnd = pair_car(args);
    args = pair_cdr(args);

    double p[3];
    get_double_vec(s_pnd, 3, p, "read VEC3");
    return sc->NIL;
}


pointer gvsP_vec4 (scheme *sc, pointer args) {
    pointer s_pnd = pair_car(args);
    args = pair_cdr(args);

    double p[4];
    get_double_vec(s_pnd, 4, p, "read VEC4");
    return sc->NIL;
}

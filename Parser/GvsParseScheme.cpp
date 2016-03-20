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

#include "Parser/GvsParseScheme.h"
#include "Parser/parse_helper.h"
#include <sstream>

//extern


GvsParseScheme :: GvsParseScheme(scheme* s) {
    sc = s;
}

GvsParseScheme :: GvsParseScheme(scheme* s, std::string* apNames, GvsParseAllowedNames* apType, int numNames ) {
    sc = s;
    for (int i=0;i<numNames;i++) {
        allowedParamNames.insert(std::pair<std::string,GvsParseAllowedNames>(apNames[i],apType[i]));
    }
}

GvsParseScheme :: ~GvsParseScheme() {
    delParamList();
    allowedParamNames.clear();
}

//----------------------------------------------------------------------------
//       delParamList
//----------------------------------------------------------------------------
void
GvsParseScheme :: delParamList ( void )
{
    numParameter = (int)paramListe.size();
    GvsParseParamType gpType;

    for (int i=0; i<numParameter; i++)
    {
        gpType = paramListe[i]->type;


        if (gpType==gp_string_int)
        {
            delete [] (int*)paramListe[i]->val;
        }
        else if (gpType==gp_string_double)
        {
            delete [] (double*)paramListe[i]->val;
        }
        else if (gpType==gp_string_bool)
        {
            delete [] (bool*)paramListe[i]->val;
        }
        else if (gpType==gp_string_matrix)
        {
            int dim = paramListe[i]->dim;
            int n = dim / 10;
            int m = dim % 10;

            if ((n==2) && (m==3))
                delete (m4d::Matrix<double,2,3>*)paramListe[i]->val;

            if ((n==3) && (m==4))
                delete (m4d::Matrix<double,3,4>*)paramListe[i]->val;
        }
        else if (gpType==gp_string_setparamlist)
        {
            delete (GvsSetParamList*)paramListe[i]->val;
        }

        delete paramListe[i];
    }


    paramListe.clear();
    numParameter = 0;
}


/**
 * @brief GvsParseScheme::testArgType
 * Every parameter pair consists of a name for the parameter type
 * and a parameter value; it will be written to the parameter list.
 *
 * testArgType() searches in the parameter list if the parameter with
 * name exists and returns the position within the list.
 * @param name
 * @param num
 * @return
 */
bool GvsParseScheme :: testArgType  ( const char* name, int &num ) {
    if (numParameter == 0) {
        std::cerr << "No parameter available!" << std::endl;
        return false;
    }
    char* arg_name;

    std::map<std::string,GvsParseAllowedNames>::iterator p;

    std::string msg;
    if (!allowedParamNames.empty()) {
        if (!allowedName(name)) {
            msg = name;
            msg.append(" is no valid parameter name!");
            std::cerr << "\n---------->Use parameter:\n";
            std::string pName;

            p = allowedParamNames.begin();
            do {
                pName = p->first;
                printf("\t%s\n",pName.c_str());
                p++;
            }
            while (p != allowedParamNames.end());

            scheme_error(msg);
        }
    }

    do {
        arg_name = (char*)(paramListe[num]->name);
        //std::cerr << num << " " << arg_name << std::endl;
#ifdef _WIN32
        if (strcmpi(arg_name, name) == 0) return true;
#else
        if (strcasecmp(arg_name,name)==0) return true;
#endif
        num++;
    }
    while (num<numParameter);

    return false;
}


void GvsParseScheme :: setAllowedName ( const std::string name,
                                        const GvsParseParamType pType, const int dim ) {
    GvsParseAllowedNames allowedName = {pType, dim};
    allowedParamNames.insert(std::pair<std::string,GvsParseAllowedNames>(getLowCase(name),allowedName));
}

GvsParseParamType GvsParseScheme :: getAllowedParamType ( const std::string name ) {
    allowedParamNamesPtr = allowedParamNames.find(name);
    return (allowedParamNamesPtr->second).paramType;
}

int GvsParseScheme :: getAllowedParamDim  ( const std::string name ) {
    allowedParamNamesPtr = allowedParamNames.find(name);
    return (allowedParamNamesPtr->second).paramDim;
}


bool GvsParseScheme :: allowedName  ( const char* name ) {
    //cerr << "allowedName\n";
    allowedParamNamesPtr = allowedParamNames.find(name);
    if (allowedParamNamesPtr != allowedParamNames.end()) {
        return true;
    }
    return false;
}


//  Test all input parameters if they are valid for the function 'name':
//
void GvsParseScheme :: testParamNames( const char* name ) {
    std::string msg;

    bool validName;
    //std::string validity[2] = {"non valid","valid"};

    for (int i=0; i<numParameter; i++) {
        char* pName = (char*)(paramListe[i]->name);
        validName = allowedName(pName);
#ifdef GVS_VERBOSE
        //fprintf(stderr,"%20s : %s\n",pName,validity[(int)validName].c_str());
#endif
        if (!validName) {
            msg = name;
            msg.append(": ");
            msg.append((char*)paramListe[i]->name);
            msg.append(" is no valid parameter name!");
            scheme_error(msg);
        }
    }
#ifdef GVS_VERBOSE
    //fprintf(stderr,"\n");
#endif
}


void GvsParseScheme :: printAllowedNames() {
    printf("\nAllowed Names:\n");
    /*
  for(uint i=0; i<allowedParamNames.size(); i++) {
    printf("\t%s\n",allowedParamNames[i].c_str());
  }
  */

    std::map<std::string,GvsParseAllowedNames>::iterator p;

    std::string pName;

    p = allowedParamNames.begin();
    do {
        pName = p->first;
        printf("\t%s\n",pName.c_str());
        p++;
    }
    while (p != allowedParamNames.end());
}


bool GvsParseScheme :: getParameter ( const char* name, std::string &sval ) {
    int num = 0;
    return getParameter( name, sval, num );
}

bool GvsParseScheme :: getParameter ( const char* name, std::string &sval, int &num ) {
    // std::cerr << "GvsParseScheme :: getParameter...string\n";
    if ( num >= numParameter )
        return false;
    //scheme_error("getParameter(): Parameter-Nummer > Parameter-Anzahl");

    std::string msg;
    if (testArgType(name,num))
    {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            return false;
            /*
        msg = name;
        msg.append(": String/GeoViS-Obj erwarted!");
        scheme_error(msg);
      */
        }
        else
        {
            sval = (char*)(paramListe[num]->val);
            return true;
        }
    }
    return false;
}

bool
GvsParseScheme :: getParameter ( const char* name, char*   sval )
{
    //  std::cerr << "GvsParseScheme :: getParameter... char*\n";
    int num = 0;
    if (testArgType(name,num))
    {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            return false;
        }
        else
        {
            sval = (char*)(paramListe[num]->val);
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------
//       getParameter  (int)
//----------------------------------------------------------------------------
bool
GvsParseScheme :: getParameter ( const char* name, int &ival )
{
    int num = 0;
    int* arg_val;
    std::string msg;

    ival = -1;
    if (testArgType(name,num))
    {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            msg = name;
            msg.append(": integer erwarted!");
            scheme_error(msg);
        }
        else
        {
            arg_val =  (int*)(paramListe[num]->val);
            if ((paramListe[num]->dim) != 1) return false;
            ival =arg_val[0];

            return true;
        }
    }
    return false;
}

bool
GvsParseScheme :: getParameter ( const char* name, int* ival )
{
    int dim, dimExpected = 0;
    int* arg_val;
    std::string msg;

    int num = 0;
    if (testArgType(name,num))
    {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            msg = name;
            msg.append(": integer erwarted!");
            scheme_error(msg);
        }
        else
        {
            arg_val =  (int*)(paramListe[num]->val);

            dimExpected = getAllowedParamDim(name);
            dim = paramListe[num]->dim;

            if (dim != dimExpected)
            {
                msg = name;
                msg.append(": Wrong dimension! Expected dim=");
                std::ostringstream buf;
                buf << msg << dimExpected;
                msg = buf.str();
                scheme_error(msg);
                //	    std::cerr << msg << std::endl;
                return false;
            }

            for (int k=0; k<dim; k++)
                ival[k] =arg_val[k];

            return true;
        }
    }
    return false;
}



//----------------------------------------------------------------------------
//       getParameter  (double)
//----------------------------------------------------------------------------
bool
GvsParseScheme :: getParameter ( const char* name, double &dval )
{
    int num = 0;
    double* arg_val;
    std::string msg;

    if (testArgType(name,num))
    {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            msg = name;
            msg.append(": double expected!");
            scheme_error(msg);
        }
        else
        {
            arg_val =  (double*)(paramListe[num]->val);
            if ( (paramListe[num]->dim) != 1) return false;
            dval = arg_val[0];

            return true;
        }
    }
    return false;
}

bool GvsParseScheme :: getParameter ( const char* name, double* dval ) {
    // fuer dimExpected < 0 gibt es eine Alternative
    int dim, dimExpected = 0;

    double* arg_val;
    std::string msg;

    int num = 0;
    if (testArgType(name,num))
    {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            std::cerr << "hier: " <<  getAllowedParamType(name) << " " << name << " " << paramType << std::endl;
            msg = name;
            msg.append(": double expected! ");
            msg.append(GvsParseParamTypeName[paramType]);
            msg.append(" found.");
            scheme_error(msg);
        }
        else
        {
            arg_val =  (double*)(paramListe[num]->val);

            dimExpected = getAllowedParamDim(name);
            dim = paramListe[num]->dim;

            if (dim == abs(dimExpected))
            {
                for (int k=0; k<dim; k++)
                    dval[k] =arg_val[k];
                return true;
            }

            if ((dim != abs(dimExpected)) && (dimExpected<0))
                return false;

            if ((dim != abs(dimExpected)) && (dimExpected>0))
            {
                msg = name;
                msg.append(": Wrong dimension! Expected dim=");
                std::ostringstream buf;
                buf << msg << abs(dimExpected);
                msg = buf.str();
                scheme_error(msg);
                // std::cerr << msg << std::endl;
                return false;
            }

        }
    }
    return false;
}

bool
GvsParseScheme :: getParameter  ( const char* name, double* dval, int &num, bool fixpos )
{
    int dim, dimExpected = 0;

    double* arg_val;
    std::string msg;

    GvsParseParamType paramType;
    if (fixpos)
    {
        paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType))
            return false;

        if (strcmp(name,(char*)paramListe[num]->name)!=0)
            return false;

        arg_val =  (double*)(paramListe[num]->val);

        dimExpected = getAllowedParamDim(name);
        dim = paramListe[num]->dim;

        if (dim == abs(dimExpected))
        {
            for (int k=0; k<dim; k++)
                dval[k] =arg_val[k];
            return true;
        }

        if ((dim != abs(dimExpected)) && (dimExpected<0))
            return false;

        if ((dim != abs(dimExpected)) && (dimExpected>0))
        {
            msg = name;
            msg.append(": Wrong dimension! Expected dim=");
            std::ostringstream buf;
            buf << msg << abs(dimExpected);
            msg = buf.str();
            scheme_error(msg);
            //cerr << msg << std::endl;
            return false;
        }
    }

    if (testArgType(name,num))
    {
        paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            msg = name;
            msg.append(": double expected!");
            scheme_error(msg);
        }
        else
        {
            arg_val =  (double*)(paramListe[num]->val);

            dimExpected = getAllowedParamDim(name);
            dim = paramListe[num]->dim;

            if (dim == abs(dimExpected))
            {
                for (int k=0; k<dim; k++)
                    dval[k] =arg_val[k];
                return true;
            }

            if ((dim != abs(dimExpected)) && (dimExpected<0))
                return false;

            if ((dim != abs(dimExpected)) && (dimExpected>0))
            {
                msg = name;
                msg.append(": Wrong dimension! Expected dim=");
                std::ostringstream buf;
                buf << msg << abs(dimExpected);
                msg = buf.str();
                scheme_error(msg);
                //cerr << msg << std::endl;
                return false;
            }

        }
    }
    return false;
}


bool
GvsParseScheme :: getParameter ( const char* name, bool &bval )
{
    int num = 0;
    std::string msg;

    if (testArgType(name,num))
    {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            msg = name;
            msg.append(": bool erwarted!");
            scheme_error(msg);
        }
        else
        {
            bool* bvalp = (bool*)(paramListe[num]->val);
            bval = (*bvalp);
            return true;
        }
    }
    return false;
}


bool
GvsParseScheme :: getParameter (const char* name, m4d::Matrix<double,2,3> *mat )
{
    int num = 0;
    m4d::Matrix<double,2,3>* arg_val;
    std::string msg;

    if (testArgType(name,num))
    {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            msg = name;
            msg.append(": matrix expected!");
            scheme_error(msg);
        }
        else
        {
            arg_val =  (m4d::Matrix<double,2,3>*)(paramListe[num]->val);

            int dimX = (paramListe[num]->dim) / 10;
            int dimY = (paramListe[num]->dim) % 10;
            //cerr << "dimXY: " << dimX << " " << dimY << std::endl;

            for (int n=0; n<dimX; n++)
                for (int m=0; m<dimY; m++)
                    mat->setElem(n,m,arg_val->getElem(n,m));

            return true;
        }
    }
    return false;
}


bool
GvsParseScheme :: getParameter ( const char* name, m4d::Matrix<double,3,4>* mat )
{
    int num = 0;
    m4d::Matrix<double,3,4>* arg_val;
    std::string msg;

    if (testArgType(name,num)) {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs)) {
            msg = name;
            msg.append(": matrix expected!");
            scheme_error(msg);
        }
        else {
            arg_val =  (m4d::Matrix<double,3,4>*)(paramListe[num]->val);

            int dimX = (paramListe[num]->dim) / 10;
            int dimY = (paramListe[num]->dim) % 10;
            //cerr << "dimXY: " << dimX << " " << dimY << std::endl;

            for (int n=0; n<dimX; n++) {
                for (int m=0; m<dimY; m++) {
                    mat->setElem(n,m,arg_val->getElem(n,m));
                }
            }
            return true;
        }
    }
    return false;
}


bool GvsParseScheme :: getParameter ( const char* name, GvsSetParamList* splist, int &num ) {
    //  std::cerr << "GvsParseScheme :: getParameter()...\n";
    std::string msg;

    if (testArgType(name,num))
    {
        GvsParseParamType paramType = paramListe[num]->type;
        if ((getAllowedParamType(name)!=paramType) && (paramType!=gp_string_gvs))
        {
            msg = name;
            msg.append(": setparamliste expected!");
            scheme_error(msg);
        }
        else
        {
            GvsSetParamList* pl = static_cast<GvsSetParamList*>(paramListe[num]->val);
            splist->objectIDname = pl->objectIDname;
            splist->paramName    = pl->paramName;
            (splist->param).type = (pl->param).type;
            (splist->param).val  = (pl->param).val;

            // std::cerr << splist->objectIDname << " " << splist->paramName << " "
            //      << GvsDataTypeName[(splist->param).type] << std::endl;
            return true;
        }
    }
    return false;
}


std::string GvsParseScheme :: getParamName ( const int k ) const {
    assert (k < numParameter);
    std::string name = (char*)(paramListe[k]->name);
    return name;
}

//----------------------------------------------------------------------------
//        getParamDim
//----------------------------------------------------------------------------
//  Gibt die Dimension des Arguments zurueck:
//     dim(string) = 0
//     dim(number) = 1
//     dim(vector) = Dimension des Vektors
int
GvsParseScheme :: getParamDim ( const int k ) const
{
    assert (k < numParameter);
    return (paramListe[k]->dim);
}


int GvsParseScheme :: getParamDim ( const char* name ) const {
    int i=-1;
    do {
        i++;
    }
    while ((i<numParameter) && (strcmp(name,(char*)paramListe[i]->name)!=0));

    if (strcmp(name,(char*)paramListe[i]->name)==0) {
        return (paramListe[i]->dim);
    }
    return -1;
}

//----------------------------------------------------------------------------
//       getParamType
//----------------------------------------------------------------------------
//  Gibt den Typ des Parameters zurueck
//    gp_string_string fuer z.B.  '(Metrik  "Schwarzschild")
//    gp_string_int    fuer z.B.  '(res #(640 480))
//    gp_string_double fuer z.B.  '(dir #(-1.0 0.0 0.0))
//    gp_string_bool   fuer z.B.  '(incoords #f)
//    gp_string_matrix fuer z.B.  '(transform (scale-obj #(1.3 2.0)) )
//
GvsParseParamType
GvsParseScheme :: getParamType ( const int k ) const
{
    assert (k < numParameter);
    return paramListe[k]->type;
}

GvsParseParamType
GvsParseScheme :: getParamType ( const char* name ) const
{
    int i=-1;
    do
    {
        i++;
    }
    while ((i<numParameter) && (strcmp(name,(char*)paramListe[i]->name)!=0));

    if (strcmp(name,(char*)paramListe[i]->name)==0)
        return (paramListe[i]->type);
    else
        return gp_unknown;
}


//----------------------------------------------------------------------------
//         parse
//----------------------------------------------------------------------------
//  Verbindung zwischen tinyscheme und GeoViS
//
//    Argumente einer Liste werden eingelesen und in einer Parameterliste
//    gespeichert. Dabei wird das erste Element(Symbol) als String gespeichert.
//    das zweite Element wird auf seinen Typ hin untersucht und entsprechend
//    gecastet.
//
pointer
GvsParseScheme :: parse ( pointer a )
{
#ifdef GVS_VERBOSE
    std::cerr << "GvsParseScheme :: parse\n";
#endif
    pointer argument;

    pointer arg_type;
    char*   arg_type_name;

    pointer arg_value;
    pointer elem;

    int     vecDim;
    int*    ival;
    double* dval;
    char*   sval;


    std::string msg;

    GvsParseParameter* par;

    do {
        // pointer to element x in the argument list, e.g. (type "name")
        argument = pair_car(a);
        arg_type = pair_car(argument);
        if (!is_pair(pair_cdr(argument))) {
            arg_type_name = arg_type->_object._string._svalue;
            std::cerr << "TypeName: " << arg_type_name << std::endl;
            scheme_error("argument is missing!");
        }

        if (is_symbol(arg_type)) {
            // convert symbol to string
            arg_type_name = (pair_car(arg_type))->_object._string._svalue;
#ifdef GVS_VERBOSE
            std::cerr << "TypeName: " << arg_type_name << std::endl;
#endif
            // pointer to parameter value
            arg_value = pair_car(pair_cdr(argument));
            if (arg_value==NULL) {
                scheme_error("no valid argument!");
            }
#ifdef GVS_VERBOSE
            std::cerr << " argType: " << getType(arg_value) << "\t";
#endif

            // -------------------------------------
            //   vector element
            // -------------------------------------
            if (is_vector(arg_value)) {

                // test first element of vector which type it is: int or double
                elem = ((sc->vptr)->vector_elem)(arg_value,0);

                // ------------------
                //  vector in vector
                // ------------------
                if (is_vector(elem)) {
#ifdef GVS_VERBOSE
                    std::cerr << "---->matrix: " << std::endl;
#endif
                    int n,m;
                    get_matrix_size(arg_value,n,m);
                    par = new GvsParseParameter;
                    par->type = gp_string_matrix;
                    par->name = (char*)arg_type_name;
                    par->dim  = n*10+m;

                    if ((n==2) && (m==3)) {
                        m4d::Matrix<double,2,3>* mat2d = new m4d::Matrix<double,2,3>;
                        get_matrix(arg_value,mat2d,"get-double-matrix");
                        par->val  = mat2d;
                        // mat2d->print(cerr);
                    }
                    else if ((n==3) && (m==4)) {
                        m4d::Matrix<double,3,4>* mat3d = new m4d::Matrix<double,3,4>;
                        get_matrix(arg_value,mat3d,"get-double-matrix");
                        par->val  = mat3d;
                        // mat3d->print(cerr);
                    }
                    else {
                        scheme_error("matrix format cannot be read.");
                    }

                    paramListe.push_back(par);
                }
                else {
#ifdef GVS_VERBOSE
                    std::cerr << "---->vector: ( ";
#endif
                    // Determine length of vector
                    vecDim = ((sc->vptr)->vector_length)(arg_value);

                    // ------------------
                    //  integer in vector
                    // ------------------
                    if (is_integer(elem)) {
                        // read integer value
                        //std::cerr << "int" << std::endl;
                        ival = new int[vecDim];
                        get_int_vec(arg_value,vecDim,ival,"get-int-vector");

                        par = new GvsParseParameter;
                        par->type = gp_string_int;
                        par->name = (char*)arg_type_name;
                        par->dim  = vecDim;
                        par->val  = (int*)ival;
#ifdef GVS_VERBOSE
                        for (int i=0; i<vecDim; i++) {
                            std::cerr << ival[i] << " ";
                        }
                        std::cerr << ")" << std::endl;
#endif
                        paramListe.push_back(par);
                    }
                    else if (is_real(elem)) {
                        // read double value
                        //std::cerr << "double" << std::endl;
                        dval = new double[vecDim];
                        get_double_vec(arg_value,vecDim,dval,"get-double-vector");

                        par = new GvsParseParameter;
                        par->type = gp_string_double;
                        par->name = (char*)arg_type_name;
                        par->dim  = vecDim;
                        par->val  = (double*)dval;
#ifdef GVS_VERBOSE
                        for (int i=0; i<vecDim; i++)
                        {
                            std::cerr << dval[i] << " ";
                        }
                        std::cerr << ")" << std::endl;
#endif
                        paramListe.push_back(par);
                    }
                }
            }
            // -------------------------------------
            //   string element
            // -------------------------------------
            else if (is_string(arg_value))
            {
                // String
                sval = arg_value->_object._string._svalue;
#ifdef GVS_VERBOSE
                std::cerr << "---->string: " << sval << std::endl;
#endif
                par = new GvsParseParameter;
                par->type = gp_string_string;
                par->name = (char*)arg_type_name;
                par->dim  = 0;
                par->val  = (char*)sval;

                paramListe.push_back(par);
            }
            // -------------------------------------
            //   number element
            // -------------------------------------
            else if (is_number(arg_value)) {
                // Zahl
                //cerr << "number\n";
                par = new GvsParseParameter;
                par->name = (char*)arg_type_name;
                par->dim  = 1;

                if (is_real(arg_value)) {
                    double* dbl_val = new double[1];
                    dbl_val[0] = rvalue(arg_value);
                    par->type = gp_string_double;
                    par->val  = dbl_val;
#ifdef GVS_VERBOSE
                    std::cerr << "---->real: " << *dbl_val << std::endl;
#endif
                }
                else {
                    int* int_val = new int[1];
                    int_val[0] = ivalue(arg_value);
                    par->type = gp_string_int;
                    par->val  = int_val;
#ifdef GVS_VERBOSE
                    std::cerr << "---->int: " << *int_val << std::endl;
#endif
                }
                paramListe.push_back(par);
            }
            // -------------------------------------
            //   character element
            // -------------------------------------
            else if (is_character(arg_value)) {
                // character
                std::cerr << "---->character\n";
            }
            else if (is_string(arg_value)) {
                std::cerr << "---->string\n";
            }
            // -------------------------------------
            //   symbol
            // -------------------------------------
            else if (is_symbol(arg_value)) {
                // symbol
#ifdef GVS_VERBOSE
                std::cerr << "---->symbol: ";
#endif
                par = new GvsParseParameter;
                par->type = gp_string_gvs;
                par->name = (char*)arg_type_name;
                par->dim  = 0;
                par->val  = (char*)(pair_car(arg_value))->_object._string._svalue;
#ifdef GVS_VERBOSE
                std::cerr << (pair_car(arg_value))->_object._string._svalue << std::endl;
#endif
                paramListe.push_back(par);
            }
            // -------------------------------------
            //   boolean element
            // -------------------------------------
            else if ( (arg_value == sc->T) || (arg_value == sc->F) ) {
                // boolean
                bool* bval = new bool[1];
                bval[0]=false;
                if (arg_value == sc->T) bval[0]=true;

                par = new GvsParseParameter;
                par->type = gp_string_bool;
                par->name = (char*)arg_type_name;
                par->dim  = 0;
                par->val  = bval;

                paramListe.push_back(par);
            }
            // -------------------------------------
            //   pair
            // -------------------------------------
            else if (is_pair(arg_value)) {
                // pair
#ifdef GVS_VERBOSE
                std::cerr << "---->pair:\n";
#endif
                //	    std::cerr << getType(pair_car(arg_value)) << std::endl;
                //	    std::cerr << (pair_car(arg_value))->_object._string._svalue << std::endl;

                par = new GvsParseParameter;
                par->type = gp_string_setparamlist;
                par->name = (char*)arg_type_name;
                par->dim   = 0;

                char* oid = (pair_car(arg_value))->_object._string._svalue;
                char* pn  = (pair_car(pair_cdr(arg_value)))->_object._string._svalue;
#ifdef GVS_VERBOSE
                std::cerr << oid << "  " << pn << std::endl;
#endif
                GvsSetParamList* spl = new GvsSetParamList;
                spl->objectIDname = oid;
                spl->paramName = pn;
#ifdef GVS_VERBOSE
                std::cerr << spl->paramName << " = ( ";
#endif
                pointer pval = (pair_car(pair_cdr(pair_cdr(arg_value))));

                if (is_number(pval)) {
                    // std::cerr << "is-number\n";
                    if (is_real(pval))
                    {
                        double* dbl_val = new double(rvalue(pval));
                        (spl->param).type = gvsDT_DOUBLE;
                        (spl->param).val = dbl_val;
#ifdef GVS_VERBOSE
                        std::cerr << *((double*)(spl->param).val) << " ";
                        std::cerr << ")" << std::endl;
#endif
                    }
                    else {
                        int* int_val = new int(ivalue(pval));
                        (spl->param).type = gvsDT_INT;
                        (spl->param).val = int_val;
#ifdef GVS_VERBOSE
                        std::cerr << *((int*)(spl->param).val) << " ";
                        std::cerr << ")" << std::endl;
#endif
                    }
                }
                else if (is_string(pval)) {
                    const char* cc = new char[strlen(pval->_object._string._svalue)+1];
                    memcpy((void*)cc,pval->_object._string._svalue,strlen(pval->_object._string._svalue)+1);
                    (spl->param).type = gvsDT_STRING;
                    (spl->param).val = (void*)cc;
#ifdef GVS_VERBOSE
                        std::cerr << cc << " ";
                        std::cerr << ")" << std::endl;
#endif
                }
                else if (is_vector(pval)) {
                    // cerr << "is-vector\n";
                    vecDim = ((sc->vptr)->vector_length)(pval);
                    if (vecDim<2 || vecDim>4) {
                        scheme_error("vector dim must be 2,3, or 4!");
                    }
                    elem = ((sc->vptr)->vector_elem)(pval,0);
                    if (is_vector(elem))
                    {
#ifdef GVS_VERBOSE
                        std::cerr << "---->matrix: " << std::endl;
#endif
                        int n,m;
                        get_matrix_size(pval,n,m);
                        if ((n==2) && (m==3))  {
                            (spl->param).type = gvsDT_MAT2D;
                            m4d::Matrix<double,2,3>* mat2d = new m4d::Matrix<double,2,3>;
                            get_matrix(pval,mat2d,"get-double-matrix");
                            (spl->param).val  = mat2d;
                            // mat2d->print(cerr);
                        }
                        else if ((n==3) && (m==4)) {
                            (spl->param).type = gvsDT_MAT3D;
                            m4d::Matrix<double,3,4>* mat3d = new m4d::Matrix<double,3,4>;
                            get_matrix(pval,mat3d,"get-double-matrix");
                            (spl->param).val  = mat3d;
                            //  mat3d->print(std::abortcerr);
                        }
                        else  {
                            scheme_error("matrix format cannot be read");
                        }
                    }
                    else if (is_integer(elem))  // ++++++++++++++ read integer values
                    {
                        //   std::cerr << "---->integer" << std::endl;
                        ival = new int[4];
                        get_int_vec(pval,vecDim,ival,"get-int-vector");
                        if (vecDim==2) {
                            (spl->param).type = gvsDT_IVEC2;
                            m4d::ivec2* pt = new m4d::ivec2;
                            for(int i=0; i<vecDim; i++) {
                                pt->setX(i,ival[i]);
                            }
                            (spl->param).val = pt;
                        }
                        else if (vecDim==3) {
                            (spl->param).type = gvsDT_IVEC3;
                            m4d::ivec3* pt = new m4d::ivec3;
                            for (int i=0;i<vecDim;i++) {
                                pt->setX(i,ival[i]);
                            }
                            (spl->param).val = pt;
                        }
                        else if (vecDim==4) {
                            (spl->param).type = gvsDT_IVEC4;
                            m4d::ivec4* pt = new m4d::ivec4;
                            for (int i=0;i<4;i++) {
                                pt->setX(i,ival[i]);
                            }
                            (spl->param).val = pt;
                        }

#ifdef GVS_VERBOSE
                        for (int i=0; i<vecDim; i++)
                        {
                            std::cerr << ival[i] << " ";
                        }
                        std::cerr << ")" << std::endl;
#endif
                        delete [] ival;
                    }
                    else if (is_real(elem))  //  ++++++++++++++ read double values
                    {
                        // std::cerr << "---->double" << std::endl;
                        dval = new double[4];
                        get_double_vec(pval,vecDim,dval,"get-double-vector");
                        if (vecDim==2) {
                            (spl->param).type = gvsDT_VEC2;
                            m4d::vec2* pt = new m4d::vec2;
                            for(int i=0; i<vecDim; i++) {
                                pt->setX(i,dval[i]);
                            }
                            (spl->param).val = pt;
                        }
                        else if (vecDim==3) {
                            (spl->param).type = gvsDT_VEC3;
                            m4d::vec3* pt = new m4d::vec3;
                            for (int i=0;i<vecDim;i++) {
                                pt->setX(i,dval[i]);
                            }
                            (spl->param).val = pt;
                        }
                        else if (vecDim==4)
                        {
                            (spl->param).type = gvsDT_VEC4;
                            m4d::vec4* pt = new m4d::vec4;
                            for (int i=0;i<4;i++) //Nur 4 Komponenten
                            {
                                pt->setX(i,dval[i]);
                            }
                            (spl->param).val = pt;
                        }

#ifdef GVS_VERBOSE
                        for (int i=0; i<vecDim; i++)
                        {
                            std::cerr << dval[i] << " ";
                        }
                        std::cerr << ")" << std::endl;
#endif
                        delete [] dval;
                    }
                    else
                    {
                        scheme_error("unknown std::vector element");
                    }
                }
                else
                {
                    scheme_error("pointer flag unknown!\n");
                }

                par->val = spl;
                paramListe.push_back(par);
            }
            /*
            else if (getType(arg_value)=="T_GEOVIS")
            {
                //cerr << "GeoViS\n";
                par = new GvsParseParameter;
                par->type = gp_string_gvs;
                par->name = (char*)arg_type_name;
                par->dim  = 0;
                par->val  = (char*)(arg_value)->_object._string._svalue;

                paramListe.push_back(par);
            }
            */
            else {
                // ?
                std::cerr << getType(arg_value) << std::endl;
                arg_type_name = (pair_car(arg_type))->_object._string._svalue;
                std::cerr << "TypeName: " << arg_type_name << std::endl;
                msg = arg_type_name;
                msg.append(": parameter is missing");
                scheme_error(msg);
            }
        }
        a = pair_cdr(a);
    }
    while (a != sc->NIL);

    numParameter = (int)paramListe.size();
#ifdef GVS_VERBOSE
    std::cerr << "done.\n";
#endif

    return a;
}


bool GvsParseScheme :: transType ( const GvsDataType datType, GvsParseParamType &pType, int &dim ) {
    switch (datType) {
        case gvsDT_MAT2D:
        case gvsDT_MAT3D: {
            break;
        }
        case gvsDT_INT: {
            pType = gp_string_int;
            dim = 1;
            return true;
        }
        case gvsDT_FLOAT: {
            return false;
        }
        case gvsDT_DOUBLE: {
            pType = gp_string_double;
            dim = 1;
            return true;
        }
        case gvsDT_VEC2: {
            pType = gp_string_double;
            dim = 2;
            return true;
        }
        case gvsDT_VEC3: {
            pType = gp_string_double;
            dim = 3;
            return true;
        }
        case gvsDT_VEC4: {
            pType = gp_string_double;
            dim = 4;
            return true;
        }
        case gvsDT_IVEC2: {
            pType = gp_string_int;
            dim = 2;
            return true;
        }
        case gvsDT_IVEC3: {
            pType = gp_string_int;
            dim = 3;
            return true;
        }
        case gvsDT_IVEC4: {
            pType = gp_string_int;
            dim = 4;
            return true;
        }
        case gvsDT_STRING: {
            pType = gp_string_string;
            dim = 0;
            return true;
        }
        case gvsDT_UNKNOWN:
        case gvsDT_VOID: {
            pType = gp_unknown;
            dim = 0;
            return true;
        }
    }
    return false;
}


//----------------------------------------------------------------------------
//         print
//----------------------------------------------------------------------------
//  Ausgabe aller eingelesenen Parameter mit ihren Werten
//
void GvsParseScheme :: print () const {
    printf("\n\nParseScheme read the following values:\n");
    printf("--------------------------------------\n");
    printf("Number of parameters: %i\n",numParameter);
    for (int i=0; i<numParameter; i++) {
        int dim = paramListe[i]->dim;
        printf("Parameter %2i:  name: %13s   type: %8s   dim: %2i",
               i,(char*)paramListe[i]->name,GvsParseParamTypeName[paramListe[i]->type].c_str(),dim);
        if (dim>=0) {
            int* ival;
            double* dval;
            bool bval;
            std::string* str;
            for (int k=0;k<dim;k++) {
                if (paramListe[i]->type==gp_string_int) {
                    ival=(int*)(paramListe[i]->val);
                    printf("  Val(%i)=%i",k,ival[k]);
                }
                else if (paramListe[i]->type==gp_string_double) {
                    dval=(double*)(paramListe[i]->val);
                    printf("  val(%i)=%7.3f",k,dval[k]);
                }
            }

            if ( (paramListe[i]->type==gp_string_string) || (paramListe[i]->type==gp_string_gvs)) {
                str = (std::string*)(paramListe[i]->val);
                printf("  name=%s",str->c_str());
            }
            else if (paramListe[i]->type==gp_string_bool) {
                bval = (*((bool*)paramListe[i]->val));
                printf("  boolean=%i",bval);
            }
            printf("\n");
        } else {
            printf("\n");
        }
    }
}

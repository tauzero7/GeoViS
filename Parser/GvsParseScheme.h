#ifndef GVS_PARSE_SCHEME_H
#define GVS_PARSE_SCHEME_H


#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>
#include <cassert>
#include <map>
#include "GvsGlobalDefs.h"
#include <Parser/parse_helper.h>


#include "scheme-private.h"
#include "scheme.h"


#ifndef T_MASKTYPE
#define T_MASKTYPE      31
#endif




enum GvsParseParamType
{
    gp_unknown = 0,
    gp_string_int,
    gp_string_double,
    gp_string_string,
    gp_string_bool,
    gp_string_gvs,
    gp_string_matrix,
    gp_string_setparamlist
};

static const std::string GvsParseParamTypeName[] = { 
    "unknown", "int", "double", "string", "bool", "gvs", "matrix", "splist" };

struct GvsParseParameter
{
    GvsParseParamType type;   // Parametertyp
    void* name;               // Parameter-Name: char*, symbol, ...
    int   dim;                // Dimension des Parameterwerts bei int,double
    void* val;                // Parameter-Wert: char*, int[], double[], ...
};


struct GvsParseAllowedNames
{
    GvsParseParamType  paramType;
    int                paramDim;   // fuer string verwende dim=0;
};


class GvsParseScheme
{
public:
    GvsParseScheme(scheme* s);
    GvsParseScheme(scheme* s, std::string* apNames, GvsParseAllowedNames* apType, int numNames);
    virtual ~GvsParseScheme();

    virtual pointer parse ( pointer a );  //!< Read parameters into a list

    void read_string (pointer a, const char*);

    virtual void delParamList ( void );   //!< Clear parameter list


    int   getNumParam  ( void );
    bool  getParameter ( const char* name, char*   sval );
    bool  getParameter ( const char* name, std::string  &sval );
    bool  getParameter ( const char* name, std::string  &sval, int &num);
    bool  getParameter ( const char* name, int     &ival);
    bool  getParameter ( const char* name, int*    ival );
    bool  getParameter ( const char* name, double  &dval);
    bool  getParameter ( const char* name, double* dval );
    bool  getParameter ( const char* name, double* dval, int &num, bool fixpos = true );

    bool  getParameter ( const char* name, bool    &bval );

    bool  getParameter ( const char* name, m4d::Matrix<double,2,3>* mat );
    bool  getParameter ( const char* name, m4d::Matrix<double,3,4>* mat );

    bool  getParameter ( const char* name, GvsSetParamList* splist, int &num );

    std::string        getParamName ( const int k ) const;
    int                getParamDim  ( const int k ) const;
    int                getParamDim  ( const char* name ) const;
    GvsParseParamType  getParamType ( const int k ) const;
    GvsParseParamType  getParamType ( const char* name ) const;


    // Suche in Parameterliste nach dem Parameter 'name'
    bool  testArgType  ( const char* name, int &num );

    void               setAllowedName      ( const std::string name, const GvsParseParamType pType, const int dim);
    GvsParseParamType  getAllowedParamType ( const std::string name );
    int                getAllowedParamDim  ( const std::string name );

    bool  allowedName       ( const char*  name );
    void  testParamNames    ( const char* name );
    void  printAllowedNames ( );

    bool  transType ( const GvsDataType datType, GvsParseParamType &pType, int &dim );

    void  print () const;

private:
    scheme* sc;      //!< pointer to scheme
    pointer args;    //!< pointer to current argument

    int  numParameter;  //!< number of parameters
    std::vector<GvsParseParameter*> paramListe;   //!< parameter list

    std::map<std::string,GvsParseAllowedNames>           allowedParamNames;
    std::map<std::string,GvsParseAllowedNames>::iterator allowedParamNamesPtr;
};


inline int GvsParseScheme :: getNumParam ( void ) {
    return numParameter;
}


#endif

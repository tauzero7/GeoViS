

#include <iostream>
#include "GvsGlobalDefs.h"
#include "Texture/GvsUniTex.h"

//----------------------------------------------------------------------------
//         constructor, destructor
//----------------------------------------------------------------------------
GvsUniTex :: GvsUniTex ( const GvsColor &unicol )
    : uniColor ( unicol )
{
    uniValue = unicol.luminance();
    AddParam("color",gvsDT_VEC3);
}

GvsUniTex :: GvsUniTex ( double unival )
    : uniColor ( unival )
{
    uniValue = unival;
    AddParam("color",gvsDT_VEC3);
}


double GvsUniTex :: sampleValue( GvsSurfIntersec & ) const {
    return uniValue;
}


GvsColor GvsUniTex :: sampleColor( GvsSurfIntersec & ) const {
    return uniColor;
}


GvsColor GvsUniTex :: color ( ) const {
    return uniColor;
}


void GvsUniTex :: setColor ( const GvsColor &col ) {
    uniColor = col;
    uniValue = col.luminance();
}

int GvsUniTex::SetParam( std::string pName, m4d::vec2 pt ) {
    int isOkay = GvsBase::SetParam(pName,pt);
    if (isOkay >= gvsSetParamNone && pName == "color") {
        setColor(GvsColor(pt.x(0),pt.x(1),pt.x(2)));
    }
    return isOkay;
}

//----------------------------------------------------------------------------
//         print
//----------------------------------------------------------------------------
void GvsUniTex::Print( FILE* fptr ) {
    fprintf(fptr,"UniTex {\n");
    fprintf(fptr,"\tcolor: %6.3f %6.3f %6.3f\n",uniColor.red,uniColor.green,uniColor.blue);
    fprintf(fptr,"}\n");
}

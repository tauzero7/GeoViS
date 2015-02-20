
#include <iostream>
#include <Ray/GvsSurfIntersec.h>
#include "Texture/GvsColorGradTex.h"

void clamp( double &x ) {
    if (x<0.0) x = 0.0;
    if (x>1.0) x = 1.0;
}

GvsColorGradTex::GvsColorGradTex() {
    uniColor1 = RgbBlack;
    uniColor2 = RgbWhite;
    this->startPoint = m4d::vec2(0.0,0.5);
    this->endPoint = m4d::vec2(1.0,0.5);
    this->gradDir = this->endPoint - this->startPoint;

    AddParam("color1",gvsDT_VEC3);
    AddParam("color2",gvsDT_VEC3);
    AddParam("startPoint",gvsDT_VEC2);
    AddParam("endPoint",gvsDT_VEC2);
    AddParam("gradType",gvsDT_STRING);
}

GvsColorGradTex::GvsColorGradTex( const GvsColor &unicol1, const GvsColor &unicol2 )
    : uniColor1(unicol1), uniColor2(unicol2) {
    this->startPoint = m4d::vec2(0.0,0.5);
    this->endPoint = m4d::vec2(1.0,0.5);
    this->gradDir = this->endPoint - this->startPoint;

    AddParam("color1",gvsDT_VEC3);
    AddParam("color2",gvsDT_VEC3);
    AddParam("startPoint",gvsDT_VEC2);
    AddParam("endPoint",gvsDT_VEC2);
    AddParam("gradType",gvsDT_STRING);
}


GvsColorGradTex::GvsColorGradTex( const GvsColor &unicol1, const GvsColor &unicol2,
                                  const m4d::vec2 startPoint, const m4d::vec2 endPoint,
                                  std::string gType )
    : uniColor1(unicol1), uniColor2(unicol2) {
    this->startPoint = startPoint;
    this->endPoint = endPoint;
    this->gradDir = this->endPoint - this->startPoint;
    SetGradType(gType);

    AddParam("color1",gvsDT_VEC3);
    AddParam("color2",gvsDT_VEC3);
    AddParam("startPoint",gvsDT_VEC2);
    AddParam("endPoint",gvsDT_VEC2);
    AddParam("gradType",gvsDT_STRING);
}

void GvsColorGradTex::SetColor1( const GvsColor &col ) {
    uniColor1 = col;
}

void GvsColorGradTex::SetColor2( const GvsColor &col ) {
    uniColor2 = col;
}

void GvsColorGradTex::SetColors( const GvsColor &col1, const GvsColor &col2 ) {
    uniColor1 = col1;
    uniColor2 = col2;
}

void GvsColorGradTex::SetStartPoint( const m4d::vec2 point ) {
    startPoint = point;
    gradDir = endPoint - startPoint;
    gradDirLength = gradDir.normalize();
}

void GvsColorGradTex::SetEndPoint( const m4d::vec2 point ) {
    endPoint = point;
    gradDir = endPoint - startPoint;
    gradDirLength = gradDir.normalize();
}

void GvsColorGradTex::SetGradType( std::string gType ) {
    if (gType=="linear") {
        gradType = gvsColorGrad_linear;
    }
    else if (gType=="radial") {
        gradType = gvsColorGrad_radial;
    }
    else if (gType=="hsv") {
        gradType = gvsColorGrad_hsv;
    }
    else if (gType=="texCoords") {
        gradType = gvsColorGrad_texCoords;
    }
}


double GvsColorGradTex::sampleValue( GvsSurfIntersec &insec ) const {
    return (sampleColor(insec)).luminance();
}


GvsColor GvsColorGradTex::sampleColor( GvsSurfIntersec &insec ) const {
    m4d::vec2 q = texTransformation * insec.texUVParam();

    GvsColor col;
    switch (gradType) {
        case gvsColorGrad_linear: {
            m4d::vec2 dv = (q - startPoint);
            double lambda = (dv|gradDir) / gradDirLength;
            clamp(lambda);
            col = uniColor1*(1.0-lambda) + uniColor2*lambda;
            break;
        }
        case gvsColorGrad_radial: {
            double lambda = (q - startPoint).getNorm() / gradDirLength;
            clamp(lambda);
            col = uniColor1*(1.0-lambda) + uniColor2*lambda;
            break;
        }
        case gvsColorGrad_hsv: {
            m4d::vec2 dv = (q - startPoint);
            double lambda = (dv|gradDir) / gradDirLength;
            clamp(lambda);
            col.setHSV(lambda*360.0,1.0,1.0);
            break;
        }
        case gvsColorGrad_texCoords: {
            col = GvsColor(q[0],q[1],0.0);
            break;
        }
    }
    return col;
}


int GvsColorGradTex::SetParam( std::string pName, m4d::vec2 pt ) {
    int isOkay = GvsBase::SetParam(pName,pt);
    if (isOkay >= gvsSetParamNone && pName == "startPoint") {
        startPoint = pt;
    }
    else if (isOkay >= gvsSetParamNone && pName == "endPoint") {
        endPoint = pt;
    }
    return isOkay;
}


int GvsColorGradTex::SetParam( std::string pName, m4d::vec3 pt ) {
    int isOkay = GvsBase::SetParam(pName,pt);
    if (isOkay >= gvsSetParamNone && pName == "color1") {
        uniColor1 = GvsColor(pt.x(0),pt.x(1),pt.x(2));
    }
    else if (isOkay >= gvsSetParamNone && pName == "color2") {
        uniColor2 = GvsColor(pt.x(0),pt.x(1),pt.x(2));
    }
    return isOkay;
}


int GvsColorGradTex::SetParam ( std::string pName, std::string gType ) {
    int isOkay = GvsBase::SetParam(pName,gType);
    if (isOkay >= gvsSetParamNone && pName == "gradType") {
        SetGradType(gType);
    }
    return isOkay;
}

void GvsColorGradTex::Print( FILE* fptr ) {
    fprintf(fptr,"ColorGradTex {\n");
    fprintf(fptr,"\tcolor1: %6.3f %6.3f %6.3f\n",uniColor1.red,uniColor1.green,uniColor1.blue);
    fprintf(fptr,"\tcolor2: %6.3f %6.3f %6.3f\n",uniColor2.red,uniColor2.green,uniColor2.blue);
    fprintf(fptr,"\tstart:  %5.2f %5.2f\n",startPoint[0],startPoint[1]);
    fprintf(fptr,"\tend:    %5.2f %5.2f\n",endPoint[0],endPoint[1]);
    fprintf(fptr,"}\n");
}

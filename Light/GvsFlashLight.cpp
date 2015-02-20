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

#include "GvsFlashLight.h"
#include <iostream>
#include <Dev/GvsDevice.h>
#include <Ray/GvsSurfIntersec.h>


GvsFlashLight :: GvsFlashLight () {
    lightColor = GvsColor( 1, 1, 1 );
    flashType    = gvsFTmultiFlash;
    flashPattern = "10";
    flashLengthOn  = 1.0;
    flashLengthOff = 1.0;

    calcFlashPeriod();
}


GvsFlashLight :: GvsFlashLight( const m4d::vec4 pos, const GvsColor col ) {
    lightPosition = pos;
    lightColor = col;

    flashType      = gvsFTmultiFlash;
    flashStartTime = 0.0;
    flashPattern   = "10";
    flashLengthOn  = 1.0;
    flashLengthOff = 1.0;

    calcFlashPeriod();
}

GvsFlashLight :: GvsFlashLight( const m4d::vec4 pos, const GvsColor col,
                                const GvsFlashType flType, const double flStart,
                                const std::string flPattern,
                                const double flOn, const double flOff ) {
    lightPosition = pos;
    lightColor = col;

    flashType      = flType;
    flashStartTime = flStart;
    flashPattern   = flPattern;
    flashLengthOn  = flOn;
    flashLengthOff = flOff;

    calcFlashPeriod();
}

GvsFlashLight :: ~GvsFlashLight() {
    //  delAllParam();
}

void GvsFlashLight :: setFlashType( const GvsFlashType flType ) {
    flashType = flType;
}

void GvsFlashLight :: setFlashLengths( const double flOn, const double flOff ) {
    flashLengthOn  = flOn;
    flashLengthOff = flOff;
    calcFlashPeriod();
}

void GvsFlashLight :: setFlashPattern( const std::string pattern ) {
    flashPattern = pattern;
    calcFlashPeriod();
}

void GvsFlashLight :: setFlashStartTime ( const double flStart ) {
    flashStartTime = flStart;
}

void GvsFlashLight :: calcFlashPeriod () {
    flashPeriod = 0.0;
    flash.clear();

    int len = flashPattern.size();

    int n = 0;
    for (int i=0; i<len; i++) {
        std::string sub = flashPattern.substr(i,1);
        bool onOrOff = false;
        if (sub == "1" ) onOrOff = true;

        if (flash.size()>0) {
            if (flash[n-1].onoff == onOrOff) {
                if (onOrOff) {
                    flash[n-1].max += flashLengthOn;
                    flashPeriod += flashLengthOn;
                }
                else {
                    flash[n-1].max += flashLengthOff;
                    flashPeriod += flashLengthOff;
                }
            }
            else {
                gvs_flashlight fl;
                fl.min   = flashPeriod;
                fl.max   = flashPeriod;
                if (onOrOff) {
                    fl.max += flashLengthOn;
                    flashPeriod += flashLengthOn;
                }
                else {
                    fl.max += flashLengthOff;
                    flashPeriod += flashLengthOff;
                }
                fl.onoff = onOrOff;
                flash.push_back(fl);
                n++;
            }
        }
        else {
            gvs_flashlight fl;
            fl.min   = flashPeriod;
            fl.max   = flashPeriod;
            if (onOrOff) {
                fl.max += flashLengthOn;
                flashPeriod += flashLengthOn;
            }
            else {
                fl.max += flashLengthOff;
                flashPeriod += flashLengthOff;
            }
            fl.onoff = onOrOff;
            flash.push_back(fl);
            n++;
        }
    }
}

GvsFlashType GvsFlashLight :: getFlashType () const {
    return flashType;
}

double GvsFlashLight :: getFlashStartTime() const {
    return flashStartTime;
}

void GvsFlashLight :: getFlashLengths  ( double &flOn, double &flOff ) {
    flOn  = flashLengthOn;
    flOff = flashLengthOff;
}

std::string GvsFlashLight :: getFlashPattern ( void ) const {
    return flashPattern;
}

GvsColor GvsFlashLight :: getDirectLight(
        GvsDevice*       ,
        const m4d::vec3& ,
        const double     ,
        const m4d::vec3& localLightDir,
        GvsSurfIntersec* surfIntersec,
        GvsShader&       ,
        const m4d::vec3& ,
        int  ,
        bool    )
{
  //  std::cerr << "GvsFlashLight :: getDirectLight...\n";

  // Schnittpunkt in Koordinaten-Darstellung
  m4d::vec3 intersecPoint  = surfIntersec->point().getAsV3D();

  // Flaechennormale in lokaler Darstellung
  m4d::vec3 intersecNormal = surfIntersec->normNormal();

  double cosNL = intersecNormal | localLightDir;
  if (cosNL <= 0.0) return RgbBlack;

  //double distAtten = lightDistAttenuation( lightDist );
  //if (distAtten < lightAttenThresh) return RgbBlack;

  //GvsColor attenLightColor = lightColor * distAtten;
/*
  bool lightVisible = visAttenuation( device, surfIntersec, lightPosition,
                                      lightDir, lightDist, rayIndex,
                                      globalIllum, attenLightColor );


  if ( lightVisible )
    return (intersecShader.*bdf)( intersecPoint, intersecNormal,
                                  localLightDir, attenLightColor,
                                  outDirection );
  else
    return RgbBlack;
    */
  return RgbBlack;
}


GvsColor GvsFlashLight :: getDirectLight(
        GvsDevice*       ,
        const m4d::vec3& ,
        const double     ,
        const double     ,
        const m4d::vec3& localLightDir,
        GvsSurfIntersec* surfIntersec,
        GvsShader&       ,
        const m4d::vec3& ,
        int  ,
        bool   )
{
  //  std::cerr << "GvsFlashLight :: getDirectLight...time\n";

  // Schnittpunkt in Koordinaten-Darstellung
    m4d::vec3 intersecPoint  = surfIntersec->point().getAsV3D();

  // Flaechennormale in lokaler Darstellung
  m4d::vec3 intersecNormal = surfIntersec->normNormal();

  double cosNL = intersecNormal | localLightDir;
  if (cosNL <= 0.0) return RgbBlack;
/*
  double distAtten = lightDistAttenuation( lightDist );
  if (distAtten < lightAttenThresh) return RgbBlack;

  GvsColor attenLightColor = lightColor * distAtten;
  //  attenLightColor.printRGB(cerr);

  bool lightVisible = visAttenuation( device, surfIntersec, lightPosition,
                                      lightDir, lightDist, rayIndex,
                                      globalIllum, attenLightColor );

  if ( lightVisible )
  {
    // std::cerr << "visible...\n";
    if (testFlash(lightTime))
      return(intersecShader.*bdf)( intersecPoint, intersecNormal,
                                   localLightDir, attenLightColor,
                                   outDirection );
    return RgbBlack;
  }
  else
  {
    // std::cerr << "not visible...\n";
    return RgbBlack;
  }
  */
  return RgbBlack;
}

bool GvsFlashLight :: testFlash ( const double lightTime ) {
    //  std::cerr << "GvsFlashLight::testFlash()...";
    double reltime = (lightTime - flashStartTime);
    double time;
    if (flashType==gvsFTsingleFlash)
        time = reltime;
    else
        time = reltime - floor(reltime/flashPeriod)*flashPeriod;


    int num = flash.size();
    for (int i=0; i<num; i++) {
        if ((time>=flash[i].min) && (time<flash[i].max)) {
      /*
      	if (flash[i].onoff)
                std::cerr << "on" << std::endl;
              else
                std::cerr << "off" << std::endl;
      */
            return flash[i].onoff;
        }
    }
    //  std::cerr << "off" << std::endl;
    return false;
}


void GvsFlashLight::Print( FILE* fptr ) {
    fprintf(fptr,"FlashLight {\n");
    /*
  os << "FlashLight {" << std::endl
  << "\tpos          " << lightPosition << std::endl
  << "\tcolor        " << lightColor << std::endl
  << "\tatten        " << distAttenCoeff() << std::endl
  << "\tflashType    " << GvsFlashTypeName[flashType] << std::endl
  << "\tflashPattern " << flashPattern << std::endl
  << "\tflLengthOn   " << flashLengthOn << std::endl
  << "\tflLengthOff  " << flashLengthOff << std::endl
  << "\tFlStartTime  " << flashStartTime << std::endl
  << "}" << std::endl;
  */
}

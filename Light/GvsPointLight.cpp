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

#include "GvsPointLight.h"
#include <iostream>
#include <Dev/GvsDevice.h>
#include <Ray/GvsSurfIntersec.h>

GvsPointLight :: GvsPointLight () {
     lightColor = GvsColor( 1, 1, 1 );
}


GvsPointLight :: GvsPointLight( const m4d::vec4 pos, const GvsColor col ) {
    lightPosition = pos;
    lightColor = col;
}


GvsColor GvsPointLight :: getDirectLight(
        GvsDevice*       ,
        const m4d::vec3& ,
        const double     ,
        const m4d::vec3& localLightDir,
        GvsSurfIntersec* surfIntersec,
        GvsShader&       ,
        const m4d::vec3& ,
        int              ,
        bool                )
{
  //  std::cerr << "GvsPointLight :: getDirectLight...\n";

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

GvsColor GvsPointLight :: getDirectLight(
        GvsDevice*       device,
        const m4d::vec3& lightDir,
        const double     ,
        const double     lightDist,
        const m4d::vec3& localLightDir,
        GvsSurfIntersec* surfIntersec,
        GvsShader&       intersecShader,
        const m4d::vec3& outDirection,
        int  rayIndex,
        bool globalIllum     )
{
  return getDirectLight (  device, lightDir, lightDist,
                           localLightDir, surfIntersec, intersecShader,
                           outDirection, rayIndex, globalIllum );
}


void GvsPointLight :: Print ( FILE* fptr ) {
    fprintf(fptr,"PointLight {\n");
    fprintf(fptr,"\tposition: "); lightPosition.printS(fptr);
    fprintf(fptr,"\tcolor:    "); lightColor.Print(fptr);
    //fprintf(fptr,"\tattenue.  %f\n",distAttenCoeff());
    fprintf(fptr,"}\n");
}

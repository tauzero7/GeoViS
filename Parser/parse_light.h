/**
 * @file    parse_light.h
 * @author  Thomas Mueller
 *
        (init-light-mgr '(ambient #(double double double) )
                        '(light "light-id1" )
                        '(light "light-id2" )
                        '(shadows #t )
                        '(id    "lmgr" )
        )

        (init-light '(type   "PointLight")
                    '(pos    #(double double double double) )
                    '(color  #(double double double) )
                    '(atten  #(double double double) )
                    '(id "pl1")
        )

 *  This file is part of GeoViS.
 */
#ifndef PARSE_LIGHT_H
#define PARSE_LIGHT_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

class GvsParseScheme;

pointer gvsP_init_lightmgr(scheme* sc, pointer args);

pointer gvsP_init_light(scheme* sc, pointer args);
void gvsP_init_pointlight(GvsParseScheme* gP);
void gvsP_init_flashlight(GvsParseScheme* gP);

#endif

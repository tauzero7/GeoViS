/**
 * @file    parse_device.h
 * @author  Thomas Mueller
 *
        (init-device '(type "standard")
                     '(camera "camID")
                     '(projector "projID")
                     '(metric "metricID")
                     '(light-mgr "lmID")
                    [ '(setparam ("metricID" "mass" 0.3)) ]
        )

 *  This file is part of GeoViS.
 */
#ifndef PARSE_DEVICE_H
#define PARSE_DEVICE_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

pointer gvsP_init_device(scheme* sc, pointer args);

pointer gvsP_set_changeObj(scheme* sc, pointer args);

#endif

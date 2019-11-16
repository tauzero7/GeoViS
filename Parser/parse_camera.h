/**
 * @file    parse_camera.h
 * @author  Thomas Mueller
 *
    Parse parameters for a pinhole camera.
    @verbatim
    (init-camera '(type "PinHoleCam")
                 '(dir  #(double double double) )
                 '(vup  #(double double double) )
                 '(fov  #(double double) )
                 '(res  #(int int) )
                 '(filter "string")
                 '(id "string")
    )@endverbatim
    Direction 'dir' and vertical up-vector 'vup' are with respect to the local reference frame
    of the observer - the projector. 'fov' are given in degrees and image resolution 'res' in
    pixels.


    Parse parameters for a pinhole stereo camera.
    @verbatim
    (init-camera '(type "PinHoleCam")
                 '(dir  #(double double double) )
                 '(vup  #(double double double) )
                 '(fov  #(double double) )
                 '(sep  double)
                 '(res  #(int int) )
                 '(filter "string")
                 '(id "string")
    )@endverbatim
    Direction 'dir' and vertical up-vector 'vup' are with respect to the local reference frame
    of the observer - the projector. 'fov' are given in degrees and image resolution 'res' in
    pixels.


    Parse parameters for a 4pi camera. 'angle' defines the main direction and is to be
    given in degrees.
    @verbatim
    (init-camera '(type "4PICam")
                 '(angle  double)
                 '(res  #(int int)
                 '(filter "string")
                 '(id "string")
    )@endverbatim


    Parse parameters for a 2pi camera. The main viewing direction is defined by
    heading and pitch angles (both given in degrees).
    @verbatim
    (init-camera '(type "2PICam")
                 '(heading  double)
                 '(pitch    double)
                 '(res #(int int))
                 '(filter "string")
                 '(id "string")
    )@endverbatim
    Heading and pitch angles are given in degrees.


    Parse parameters for a panorama camera. The field of view (fov) angles have
    to be given in degrees.
    @verbatim
    (init-camera '(type "PanoramaCam")
                 '(dir  #(double double double) )
                 '(vup  #(double double double) )
                 '(fov  #(double double) )
                 '(res  #(int int) )
                 '(filter "string")
                 '(id "string")
    )@endverbatim
    The parameters are the same as for the pinhole camera.


    Camera filters can be "FilterRGB", "FilterRGBpdz", and "FilterRGBjac".
 *
 *
 *  This file is part of GeoViS.
 */
#ifndef PARSE_CAMERA_H
#define PARSE_CAMERA_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

class GvsParseScheme;

pointer gvsP_init_camera(scheme* sc, pointer args);

void gvsP_init_pinHoleCam(GvsParseScheme* gP);
void gvsP_init_pinHoleStereoCam(GvsParseScheme* gP);
void gvsP_init_panoramaCam(GvsParseScheme* gP);
void gvsP_init_4PICam(GvsParseScheme* gP);
void gvsP_init_2PICam(GvsParseScheme* gP);

#endif

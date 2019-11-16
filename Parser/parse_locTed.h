/**
 * @file    parse_locTed.h
 * @author  Thomas Mueller

    @verbatim
    (local-tetrad '(pos  #(double double double double))
                  '(vel  #(double double double double))
                  '(localvel #(double double double))
                  '(e0  #(double double double double))
                  '(e1  #(double double double double))
                  '(e2  #(double double double double))
                  '(e3  #(double double double double))
                  '(incoords bool)
                  '(localFrame  lftype)
                  '(id "string")
    )@endverbatim
    The current position of the local tetrad is compulsive. 'e0'...'e3' are the four
    basis vectors of the tetrad. 'incoords' determines whether these four basis
    vector are given in coordinates or wrt. natural local tetrad. The 'lftype' defines
    the type of the natural local tetrad. This is important for metrics like "Kerr"
    that have two natural tetrad.

 *  This file is part of GeoViS.
 */
#ifndef PARSE_LOCTED_H
#define PARSE_LOCTED_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

pointer gvsP_local_tetrad(scheme* sc, pointer args);

#endif

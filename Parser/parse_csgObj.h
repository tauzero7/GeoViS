/**
 * @file    parse_csgObject.h
 * @author  Thomas Mueller
 *
 * (csg-object   '(csgtype ,CSGTYPE )
                 '(obj1   "obj1" )
                 '(obj2   "obj2" )
                 '(id    "CSGObjID")
   )
 *
 *
 *
 *  This file is part of GeoViS.
 */
#ifndef PARSE_CSG_OBJ_H
#define PARSE_CSG_OBJ_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

pointer gvsP_init_csg_obj(scheme* sc, pointer args);
#endif

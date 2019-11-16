/**
 * @file    parse_compObj.h
 * @author  Thomas Mueller
 *
 * @brief  Parse compound object- source

@verbatim
    (comp-obj '(obj "objID1")
              '(obj "objID2")
        ...
              `(transform  ...)
              '(id "string")
        )@endverbatim
    Note that the transformation only acts on already included objects. Those objects
    that are added later by 'add-object' will not be transformed!

    @verbatim
    (comp-octree-obj '(lower #(0.0 0.0 0.0))
                     '(upper #(1.0 1.0 1.0))
                     '(subdivs 3)
                     '(obj "objID1")
                     '(obj "objID2")
                                                                          ...
                     '(id "string")
                                                                          )@endverbatim


    @verbatim
    (add-octree-obj '(add-to "compOctObjID")
                    '(obj "objID")
                                                                          )@endverbatim


    @verbatim
    (local-comp-obj '(obj "objID1")
                    '(obj "objID2")
                    '(localtetrad "locTedID")
                    '(motion "motionID")
                    '(id "string")
                                                                          )@endverbatim
    A local compount object is being described either by a motion OR by a
    local tetrad. The latter one can be represented in coordinates as well
    as with respect to a natural local tetrad.


    @verbatim
    (add-object  '(add-to "compObjID")
                 '(obj  "objID")
        )@endverbatim
 *
 *  This file is part of GeoViS.
 */
#ifndef PARSE_COMP_OBJ_H
#define PARSE_COMP_OBJ_H

extern "C" {
#include "scheme-private.h"
#include "scheme.h"
}

pointer gvsP_compound_obj(scheme* sc, pointer args);
pointer gvsP_compound_octree_obj(scheme* sc, pointer args);
pointer gvsP_local_comp_obj(scheme* sc, pointer args);

pointer gvsP_add_object(scheme* sc, pointer args);

#endif

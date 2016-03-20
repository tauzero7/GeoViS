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
/**
    Translation in 2D
    @verbatim
    (translate-obj #(double double))@endverbatim

    Translation in 3D
    @verbatim
    (translate-obj #(double double double))@endverbatim

    Rotation around x-,y-, or z-axis with angle in degree
    @verbatim
    (rotate-obj "x" angle)@endverbatim

    Rotation around axis ... with angle in degree
    @verbatim
    (rotate-obj #(double double double) angle)@endverbatim

    Scale in 2D
    @verbatim
    (scale-obj #(double double))@endverbatim

    Scale in 3D
    @verbatim
    (scale-obj #(double double double))@endverbatim


    Transformation matrices can also be composited. For example, two rotations could read like this:
    @verbatim
    (rotate-obj "z" 20.0 (rotate-obj "x" 10.0))@endverbatim
    This can, in principle, be repeated indefinitely often.
*/

#include "parse_transformObject.h"
#include "parse_helper.h"
#include "Parser/GvsParseScheme.h"

#include <GvsGlobalDefs.h>

#include "m4dGlobalDefs.h"
#include "math/TransfMat.h"

int dim = 0;
double *transformVec;
m4d::vec2 dir2d;
m4d::vec3 dir3d;
std::string achse;

m4d::Matrix<double,2,3> *inMatrix2d;
m4d::Matrix<double,3,4> *inMatrix3d;
m4d::Matrix<double,2,3> *transformMat2d;
m4d::Matrix<double,3,4> *transformMat3d;


/**
 * @brief gvsP_translateObj
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_translateObj (scheme *sc, pointer args) {
    //fprintf(stderr,"translate-obj\n");
    char buf[1024];
    if (args == sc->NIL) scheme_error("translate-obj: no arguments");

    // Determine the dimension of the transformation by means of the translation vector
    dim = (sc->vptr->vector_length)( pair_car(args) );
    if ( !((dim ==2) || (dim == 3)) ) {
        scheme_error("translate-obj: wrong dimension");
    }

    transformVec = new double[dim];

    // parse arguments
    get_double_vec(pair_car(args), dim, transformVec, "translate-obj: read vector");

    pointer matPointer;
    if ( dim == 2 ) {
        // construct transformation matrix
        transformMat2d = new m4d::TranslateMat2D(transformVec[0], transformVec[1]);

        // continue parsing for further transformations
        if (pair_car(pair_cdr(args)) != sc->NIL) {
            inMatrix2d = new m4d::Matrix<double,2,3>();
#ifdef _WIN32
            sprintf_s(buf, "translate-obj: read matrix (%s,%d)", __FILE__, __LINE__);
#else
            sprintf(buf,"translate-obj: read matrix (%s,%d)",__FILE__,__LINE__);
#endif
            get_matrix(pair_car(pair_cdr(args)), inMatrix2d, std::string(buf));

            *inMatrix2d = (*transformMat2d) * (*inMatrix2d);
            mk_matrix( &matPointer, inMatrix2d, "translate-obj: build matrix for scheme");
            delete inMatrix2d;
        }
        else {
            mk_matrix( &matPointer, transformMat2d, "translate-obj: build matrix for scheme");
        }
        delete transformMat2d;
    }
    else {
        // construct transformation matrix
        transformMat3d = new m4d::TranslateMat3D(transformVec[0], transformVec[1], transformVec[2]);

        // continue parsing for further transformations
        if (pair_car(pair_cdr(args)) != sc->NIL) {
            inMatrix3d = new m4d::Matrix<double,3,4>();

            sprintf(buf,"translate-obj: read matrix (%s,%d)",__FILE__,__LINE__);
            get_matrix(pair_car(pair_cdr(args)), inMatrix3d, std::string(buf));

            *inMatrix3d = (*transformMat3d) * (*inMatrix3d);
            mk_matrix( &matPointer, inMatrix3d, "translate-obj: build matrix for scheme");
            delete inMatrix3d;
        }
        else {
            mk_matrix( &matPointer, transformMat3d, "translate-obj: build matrix for scheme");
        }
        delete transformMat3d;
    }

    delete transformVec;
    return matPointer;
}

/**
 * Constructs a rotation matrix from an axis label or an axis vector together with an angle in degrees.
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_rotateObj (scheme *sc, pointer args) {
    //fprintf(stderr,"rotate-obj\n");
    char buf[1024];
    double angle;
    if (args == sc->NIL) scheme_error("rotate-obj: no argument");

    // If there is an axis label, then it is a three-dimensional rotation.
    if (is_string(pair_car(args))) {
        dim = 3;
        transformVec = new double[dim];
        get_string(pair_car(args), achse, "rotate-obj: axis label should be a string!");

        if ( (achse == "x") || (achse == "X") )      dir3d = m4d::vec3(1.0, 0.0, 0.0);
        else if ( (achse == "y") || (achse == "Y") ) dir3d = m4d::vec3(0.0, 1.0, 0.0);
        else if ( (achse == "z") || (achse == "Z") ) dir3d = m4d::vec3(0.0, 0.0, 1.0);
        else scheme_error("rotate-obj: wrong axis label");

        //std::cerr << achse << std::endl;
        get_double(pair_car(pair_cdr(args)), &angle, "rotate-obj: read angle");
    }
    // If the first argument is a vector, the dimension has to be determined.
    // Then, it could be a 2d or 3d rotation.
    else if ( is_vector(pair_car(args)) ) {
        dim = (sc->vptr->vector_length)( pair_car(args) );
        if ( !((dim ==2) || (dim == 3)) ) {
            scheme_error("rotate-obj: wrong dimension");
        }
        transformVec = new double[dim];
        get_double_vec(pair_car(args), dim, transformVec, "rotate-obj: read vector");
        if ( dim == 3 ) {
            dir3d = m4d::vec3(transformVec[0], transformVec[1], transformVec[2]);
        }
        get_double(pair_car(pair_cdr(args)), &angle, "rotate-obj: read angle");
    }
    // If the firest argument is a number, which is the rotation angle, then the
    // transformation must be two dimensional.
    else if ( (is_real(pair_car(args))) || (is_integer(pair_car(args))) )
    {
        dim = 2;
        transformVec = new double[dim];
        transformVec[0] = 0.0;
        transformVec[1] = 0.0;        
        get_double(pair_car(args), &angle, "rotate-obj: read angle");
    }

    pointer matPointer;
    if ( dim == 2) {
        // Construct 2D transformation matrix
        transformMat2d = new m4d::RotateMat2D(transformVec[0], transformVec[1], angle * GVS_deg2rad);

        // Bei drei Argumenten ist es eine geschachtelte Transformation
        if (pair_cdr(pair_cdr(args)) != sc->NIL) {
            inMatrix2d = new m4d::Matrix<double,2,3>();

            sprintf(buf,"rotate-obj: read matrix (%s,%d)",__FILE__,__LINE__);
            get_matrix(pair_car(pair_cdr(pair_cdr(args))), inMatrix2d,std::string(buf));
            *inMatrix2d = (*transformMat2d) * (*inMatrix2d);

            mk_matrix( &matPointer, inMatrix2d, "rotate-obj: build matrix for scheme");
            delete inMatrix2d;
        }
        else {
            mk_matrix( &matPointer, transformMat2d, "rotate-obj: build matrix for scheme");
        }
        //  delete transformMat2d;
    }
    else {
        // Construct 3D transformation matrix
        transformMat3d = new m4d::RotateMat3D(dir3d, angle * GVS_deg2rad);
        //transformMat3d->print();

        // continue parsing
        if (pair_car(pair_cdr(pair_cdr(args))) != sc->NIL) {
            inMatrix3d = new m4d::Matrix<double,3,4>();
#ifdef _WIN32
            sprintf_s(buf,"rotate-obj: read matrix (%s,%d)",__FILE__,__LINE__);
#else
            sprintf(buf, "rotate-obj: read matrix (%s,%d)", __FILE__, __LINE__);
#endif
            get_matrix(pair_car(pair_cdr(pair_cdr(args))), inMatrix3d,std::string(buf));
            *inMatrix3d = (*transformMat3d) * (*inMatrix3d);

            mk_matrix( &matPointer, inMatrix3d, "rotate-obj: build matrix for scheme");
            delete inMatrix3d;
        }
        else {
            mk_matrix( &matPointer, transformMat3d, "rotate-obj: build matrix for scheme");
        }
        delete transformMat3d;
    }

    delete transformVec;
    return matPointer;
}

/**
 * @brief gvsP_scaleObj
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_scaleObj (scheme *sc, pointer args) {
    char buf[1024];
    if (args == sc->NIL) {
        scheme_error("scale-obj: no argument");
    }

    // Determine the dimension of the scaling by means of the scale vector
    dim = (sc->vptr->vector_length)( pair_car(args) );
    if ( !((dim ==2) || (dim == 3)) ) {
        scheme_error("scale-obj: wrong dimension");
    }

    transformVec = new double[dim];
    sprintf(buf,"scale-obj: read vector (%s,%d)",__FILE__,__LINE__);
    get_double_vec(pair_car(args), dim, transformVec, std::string(buf));

    pointer matPointer;
    if ( dim == 2 ) {
        // construct scale matrix
        transformMat2d = new m4d::ScaleMat2D(transformVec[0], transformVec[1]);

        // continue parsing
        if (pair_cdr(args) != sc->NIL) {
            inMatrix2d = new m4d::Matrix<double,2,3>();

            sprintf(buf,"scale-obj: read matrix (%s,%d)",__FILE__,__LINE__);
            get_matrix(pair_car(pair_cdr(args)), inMatrix2d, std::string(buf));
            *inMatrix2d = (*transformMat2d) * (*inMatrix2d);

            mk_matrix( &matPointer, inMatrix2d, "scale-obj: build matrix for scheme");
            delete inMatrix2d;
        }
        else {
            mk_matrix( &matPointer, transformMat2d, "scale-obj: build matrix for scheme");
        }
        delete transformMat2d;
    }
    else {
        // construct transformation matrix
        transformMat3d = new m4d::ScaleMat3D(transformVec[0], transformVec[1], transformVec[2]);

        // continue parsing
        if (pair_cdr(args) != sc->NIL) {
            inMatrix3d = new m4d::Matrix<double,3,4>();
#ifdef _WIN32
            sprintf_s(buf,"scale-obj: read matrix (%s,%d)",__FILE__,__LINE__);
#else
            sprintf(buf, "scale-obj: read matrix (%s,%d)", __FILE__, __LINE__);
#endif
            get_matrix(pair_car(pair_cdr(args)), inMatrix3d, std::string(buf));
            *inMatrix3d = (*transformMat3d) * (*inMatrix3d);

            mk_matrix( &matPointer, inMatrix3d, "scale-obj: build matrix for scheme");
            delete inMatrix3d;
        }
        else {
            mk_matrix( &matPointer, transformMat3d, "scale-obj: build matrix for scheme");
        }
        //delete transformMat3d;
    }

    delete [] transformVec;
    return matPointer;
}

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

#include "parse_transformation.h"
#include "parse_helper.h"

#include <GvsGlobalDefs.h>

#include "Parser/GvsParseScheme.h"

#include "math/TransCoordinates.h"

m4d::TransCoordinates *transformator;
int length;
double *oldPos;
double *newPos;
double *oldDir;
double *newDir;
m4d::vec4 oldPosP4D;
m4d::vec4 newPosP4D;
m4d::vec4 oldDirV4D;
m4d::vec4 newDirV4D;
pointer retVec, tempVec;
bool vectorTrafo = false;

//----------------------------------------------------------------------------
//         readIn
//
// List die Argumente und parst sie
//----------------------------------------------------------------------------
void readIn(scheme *sc, pointer args)
{
    vectorTrafo = false;
    if (args == sc->NIL) scheme_error("Transformation: kein Argument");

    // Initialisierung der Punkte
    transformator = new m4d::TransCoordinates();
    oldPos = new double[4];
    newPos = new double[4];

    // Bei zwei Argumenten ist es eine Vektortransformation
    if (pair_cdr(args) != sc->NIL)
    {
        vectorTrafo = true;
        oldDir = new double[4];
        newDir = new double[4];
    }

    // Liest den Punkt ein
    get_double_vec(pair_car(args), 4, oldPos, "transCartSph Punkt einlesen");

    if (vectorTrafo)
        get_double_vec(pair_car(pair_cdr(args)), 4, oldDir, "transCartSph Vektor einlesen");


    // Umwandlung in P5D-Punkt
    oldPosP4D = m4d::vec4(oldPos[0], oldPos[1], oldPos[2], oldPos[3]);

    // Umwandlung in V4D-Vektor
    if (vectorTrafo)
        oldDirV4D = m4d::vec4(oldDir[0], oldDir[1], oldDir[2], oldDir[3]);
}

//----------------------------------------------------------------------------
//         finish
//
// Umwandlung in Scheme-Format und Speicherfreigabe
//----------------------------------------------------------------------------
void finish(scheme *sc)
{
    // Umwandlung in einen Scheme-Vektor
    if (!vectorTrafo)
    {
        retVec = ((sc->vptr->mk_vector)(sc, 4));
        for (int i = 0; i < 4; i++)
        {
            tempVec = ((sc->vptr->set_vector_elem)(retVec, i, ((sc->vptr->mk_real)(sc, newPosP4D.x(i)))));
        }
    }
    else
    {
        retVec = ((sc->vptr->mk_vector)(sc, 4));
        for (int i = 0; i < 4; i++)
        {
            tempVec = ((sc->vptr->set_vector_elem)(retVec, i, ((sc->vptr->mk_real)(sc, newDirV4D.x(i)))));
        }
    }

    // Speicher freigeben
    delete oldPos;
    delete newPos;
    delete transformator;
    if (vectorTrafo)
    {
        delete oldDir;
        delete newDir;
    }
}

//----------------------------------------------------------------------------
//         gvsP_transCartSph
//----------------------------------------------------------------------------
pointer gvsP_transCartSph (scheme *sc, pointer args)
{
    readIn(sc, args);

    // Transformiert den Punkt auf Kugelkoordinaten
    if (!vectorTrafo)
        transformator->transCoordCartSph(oldPosP4D, newPosP4D);
    else
        transformator->transCoordCartSph(oldPosP4D, oldDirV4D, newPosP4D, newDirV4D);

    finish(sc);

    //Rueckgabe des transformierten Punkts an Scheme
    return retVec;
}

//----------------------------------------------------------------------------
//         gvsP_transSphCart
//----------------------------------------------------------------------------
pointer gvsP_transSphCart (scheme *sc, pointer args)
{
    readIn(sc, args);

    // Transformiert den Punkt auf Kugelkoordinaten
    if (!vectorTrafo)
        transformator->transCoordSphCart(oldPosP4D, newPosP4D);
    else
        transformator->transCoordSphCart(oldPosP4D, oldDirV4D, newPosP4D, newDirV4D);

    finish(sc);
    return retVec;
}

//----------------------------------------------------------------------------
//         gvsP_transCartCyl
//----------------------------------------------------------------------------
pointer gvsP_transCartCyl (scheme *sc, pointer args)
{
    readIn(sc, args);

    // Transformiert den Punkt auf Kugelkoordinaten
    if (!vectorTrafo)
        transformator->transCoordCartCyl(oldPosP4D, newPosP4D);
    else
        transformator->transCoordCartCyl(oldPosP4D, oldDirV4D, newPosP4D, newDirV4D);

    finish(sc);
    return retVec;
}

//----------------------------------------------------------------------------
//         gvsP_transCylCart
//----------------------------------------------------------------------------
pointer gvsP_transCylCart (scheme *sc, pointer args)
{
    readIn(sc, args);

    // Transformiert den Punkt auf Kugelkoordinaten
    if (!vectorTrafo)
        transformator->transCoordCylCart(oldPosP4D, newPosP4D);
    else
        transformator->transCoordCylCart(oldPosP4D, oldDirV4D, newPosP4D, newDirV4D);

    finish(sc);
    return retVec;
}

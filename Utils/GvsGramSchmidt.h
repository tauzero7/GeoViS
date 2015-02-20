#ifndef GVS_GRAMSCHMIDT_H
#define GVS_GRAMSCHMIDT_H

#include "GvsGlobalDefs.h"
#include "metric/m4dMetric.h"
#include <iostream>

class GvsGramSchmidt
{
  public:
    GvsGramSchmidt(m4d::Metric* metric);
    GvsGramSchmidt(m4d::Metric*, const m4d::vec4 pos);
    GvsGramSchmidt(m4d::Metric*, const m4d::vec4 pos,
                   const m4d::vec4 e0, const m4d::vec4 u1, const m4d::vec4 u2, const m4d::vec4 u3);

    ~GvsGramSchmidt();

    void setMetric   ( m4d::Metric* metric);
    void setPosition ( const m4d::vec4 pos );

    void setVectors  ( const m4d::vec4 e0, const m4d::vec4 u1, const m4d::vec4 u2, const m4d::vec4 u3);
    void getVectors  ( m4d::vec4 &e0, m4d::vec4 &e1, m4d::vec4 &e2, m4d::vec4 &e3 );

    m4d::vec4  getVector   ( int i ) const;

    // frage nach, ob die vier Vektoren bereits orthonormiert sind
    bool isOrthonormal ( ) const;

    // frage nach, ob die vier Vektoren ein Rechts-System bilden
    bool isRightHanded ( ) const;

    // normiere Vektor
    void   normalizeVector ( m4d::vec4* vec );

    // Skalarprodukt zwischen den Vektoren v1 und v2 am Ort mPos
    double calcScalarProd  ( const m4d::vec4* v1, const m4d::vec4* v2 );

    // Orthonormiere Tetrade
    void calculateTetrad ( );
    void calculateTetrad ( m4d::vec4 &e0, m4d::vec4 &e1, m4d::vec4 &e2, m4d::vec4 &e3 );

    void Print  ( FILE *fptr = stderr );
    void printS ( ) const;


private:
    m4d::Metric* mMetric;

    double mPos[4];
    m4d::vec4 e[4];

    bool orthonormal;
};


#endif


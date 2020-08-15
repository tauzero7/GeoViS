
#include "GvsGramSchmidt.h"

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>


GvsGramSchmidt :: GvsGramSchmidt( m4d::Metric* metric ) {
    mMetric = metric;
    for (int i=0; i<4; i++) {
        e[i] = m4d::vec4();
    }
    orthonormal = false;
}

GvsGramSchmidt :: GvsGramSchmidt ( m4d::Metric* metric, const m4d::vec4 pos) {
    mMetric = metric;
    setPosition(pos);
    for (int i=0; i<4; i++) {
        e[i] = m4d::vec4();
    }
    orthonormal = false;
}

GvsGramSchmidt :: GvsGramSchmidt ( m4d::Metric* metric, const m4d::vec4 pos,
                                   const m4d::vec4 e0, const m4d::vec4 u1, const m4d::vec4 u2, const m4d::vec4 u3 ) {
    mMetric = metric;
    setPosition(pos);
    setVectors (e0,u1,u2,u3);
    orthonormal = false;
}

GvsGramSchmidt :: ~GvsGramSchmidt() {
    mMetric = NULL;
}


void GvsGramSchmidt :: setMetric ( m4d::Metric* metric) {
    mMetric = metric;
}


void GvsGramSchmidt :: setPosition ( const m4d::vec4 pos ) {
    for (int i = 0; i < 4; i++)     {
        mPos[i] = pos.x(i);
    }
    assert( mMetric != NULL);
    mMetric->calculateMetric(mPos);
}


void GvsGramSchmidt :: setVectors  ( const m4d::vec4 e0, const m4d::vec4 u1, const m4d::vec4 u2, const m4d::vec4 u3 ) {
    e[0] = e0;
    normalizeVector(&e[0]);
    e[1] = u1;
    e[2] = u2;
    e[3] = u3;

    for (int i=0; i<4; i++) {
        if (e[i].isZero()) {
            std::cerr << "GramSchmidt:  Vektor " << i << " ist ungueltig!\n";
            exit(1);
        }
    }

    // test if orthonormal
    double sc;
    orthonormal = false;

    for (int i=0; i<4; i++) {
        for (int j=i+1; j<4; j++) {
            sc = calcScalarProd ( &e[i], &e[j] );
            if (fabs(sc)<GVS_EPS)
                orthonormal |= true;
        }
    }
}


void GvsGramSchmidt :: getVectors  ( m4d::vec4 &e0, m4d::vec4 &e1, m4d::vec4 &e2, m4d::vec4 &e3 ) {
    e0 = e[0];
    e1 = e[1];
    e2 = e[2];
    e3 = e[3];
}

m4d::vec4 GvsGramSchmidt :: getVector ( int i ) const {
    return e[i];
}


bool GvsGramSchmidt :: isOrthonormal ( ) const {
    return orthonormal;
}


bool GvsGramSchmidt :: isRightHanded ( ) const {
    gsl_matrix* m = gsl_matrix_alloc(4,4);

    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            gsl_matrix_set(m,i,j,e[i][j]);

    int signum;
    gsl_permutation *p = gsl_permutation_alloc(4);

    gsl_linalg_LU_decomp ( m, p, &signum );

    double det = gsl_linalg_LU_det(m,signum);

    gsl_permutation_free(p);
    gsl_matrix_free(m);

    return ((det>0) ? true : false);
}


void GvsGramSchmidt :: normalizeVector ( m4d::vec4* vec ) {
    double norm = fabs(calcScalarProd ( vec, vec ));
    //cerr << "GramSchmidt: Normalize Vektor:" << mMetric->getMetricName() << "-" << mMetric->getMetricCoeff(1,1) << "-" << mMetric->getMetricCoeff(2,2) << "-" << mMetric->getMetricCoeff(3,3) <<"\n";
    assert ( norm > 0.0 );

    norm = sqrt(norm);
    double val;
    for ( int i=0; i<4; i++ )     {
        val = (vec->x(i))/norm;
        if (fabs(val)<1.0e-15) val = 0.0;
        vec->setX(i,val);
    }
}


double GvsGramSchmidt :: calcScalarProd ( const m4d::vec4* v1, const m4d::vec4* v2 ) {
    double prod = 0.0;
    for ( int i = 0; i < 4; i++ ) {
        for ( int j = 0; j < 4; j++ ) {
            prod += mMetric->getMetricCoeff(i,j) * v1->x(i) * v2->x(j);
        }
    }
    return prod;
}


void GvsGramSchmidt :: calculateTetrad() {
    mMetric->calculateMetric(mPos);

    m4d::vec4 u1 = e[1];
    m4d::vec4 u2 = e[2];
    m4d::vec4 u3 = e[3];

    // projiziere auf e0

    e[1] = u1 + calcScalarProd(&e[0],&u1)*e[0];
    assert (!(e[1].isZero()));
    normalizeVector(&e[1]);

    e[2] = u2 + calcScalarProd(&e[0],&u2)*e[0] - calcScalarProd(&e[1],&u2)*e[1];
    assert (!(e[2].isZero()));
    normalizeVector(&e[2]);

    e[3] = u3 + calcScalarProd(&e[0],&u3)*e[0] - calcScalarProd(&e[1],&u3)*e[1] - calcScalarProd(&e[2],&u3)*e[2];
    assert (!(e[3].isZero()));
    normalizeVector(&e[3]);

    orthonormal = true;
}


void GvsGramSchmidt :: calculateTetrad ( m4d::vec4 &e0, m4d::vec4 &e1, m4d::vec4 &e2, m4d::vec4 &e3 ) {
    setVectors(e0,e1,e2,e3);
    calculateTetrad();
    getVectors(e0,e1,e2,e3);
}


void GvsGramSchmidt :: Print( FILE* fptr ) {
    fprintf(fptr,"GvsGramSchmidt {\n");
    fprintf(fptr,"\tpos:  %6.3f %6.3f %6.3f %6.3f\n",mPos[0],mPos[1],mPos[2],mPos[3]);
    fprintf(fptr,"\te0:   %6.3f %6.3f %6.3f %6.3f\n",e[0][0],e[0][1],e[0][2],e[0][3]);
    fprintf(fptr,"}\n");
}


void GvsGramSchmidt :: printS () const {
    printf("GvsGramSchmidt {\n");
    printf("\tPosition : ( %9.6f %9.6f %9.6f %9.6f %i )\n",mPos[0],mPos[1],mPos[2],mPos[3],0);
    printf("\te0 : ( %9.6f %9.6f %9.6f %9.6f )\n", e[0].x(0),e[0].x(1),e[0].x(2),e[0].x(3));
    printf("\te1 : ( %9.6f %9.6f %9.6f %9.6f )\n", e[1].x(0),e[1].x(1),e[1].x(2),e[1].x(3));
    printf("\te2 : ( %9.6f %9.6f %9.6f %9.6f )\n", e[2].x(0),e[2].x(1),e[2].x(2),e[2].x(3));
    printf("\te3 : ( %9.6f %9.6f %9.6f %9.6f )\n", e[3].x(0),e[3].x(1),e[3].x(2),e[3].x(3));
    printf("}\n");
    printf("rightRanded: %i\n",(int)isRightHanded());
}

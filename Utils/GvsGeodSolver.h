#ifndef GVS_GEOD_SOLVER_H
#define GVS_GEOD_SOLVER_H

#include <iostream>
#include <cstdio>

#include "Obj/GvsBase.h"
#include "Obj/STMotion/GvsLocalTetrad.h"
#include "motion/m4dGeodesic.h"
#include "motion/m4dMotionList.h"
#include "metric/m4dMetric.h"


class GvsGeodSolver : public GvsBase
{
public:
    GvsGeodSolver( m4d::Metric* metric,
                   m4d::enum_integrator m4dGeodSolver );

    GvsGeodSolver( m4d::Metric* metric, m4d::enum_geodesic_type type, m4d::enum_time_direction tDir,
                   m4d::enum_integrator m4dGeodSolver );

    ~GvsGeodSolver();

    void setMetric( m4d::Metric* metric );
    m4d::Metric* getMetric();

    bool setSolver( m4d::enum_integrator m4dGeodSolver );

    void                     setGeodType( m4d::enum_geodesic_type gType );
    m4d::enum_geodesic_type  getGeodType() const;

    void                      setTimeDir ( m4d::enum_time_direction tDir );
    m4d::enum_time_direction  getTimeDir ( void  ) const;

    void   setEpsilons( double eps_a, double eps_r );
    void   getEpsilons( double &eps_a, double &eps_r );

    void   setStepSizeControl( const bool cn );
    bool   getStepSizeControl();

    void   setStepsize( double step );
    double getStepsize() const;

    void   setMaxStepsize( double step );
    double getMaxStepsize() const;


    int startConditionLocal ( const m4d::vec4* pos, m4d::vec4 &dir );

    int startCondition ( const m4d::vec4* pos, m4d::vec4 &dir );
    int startCondition ( const double pos[], double dir[] );

    double calcNormCondition ( const double pos[], const double dir[] );
    double calcNormCondition ( const m4d::vec4 pos, const m4d::vec4 dir );


    m4d::enum_break_condition calculateGeodesic ( const m4d::vec4& yStart, const m4d::vec4& Dir,
                                                  const double maxNumPoints,
                                                  m4d::vec4 *&points, m4d::vec4 *&dirs, int &numPoints );

    m4d::enum_break_condition calcParTransport ( const m4d::vec4& yStart, const m4d::vec4& yDir, const m4d::vec4 base[4],
                                                 const double maxNumPoints,
                                                 GvsLocalTetrad *&lt, int &numPoints );

    m4d::enum_break_condition calcSachsJacobi ( const m4d::vec4 &startOrig, const m4d::vec4 &startDir, const m4d::vec3 &localDir,
                                                const int maxNumPoints,
                                                const GvsLocalTetrad *lt,
                                                m4d::vec4 *&points, m4d::vec4 *&dirs, double *&lambda,
                                                m4d::vec4 *&sachs1, m4d::vec4 *&sachs2,
                                                m4d::vec5 *&rayJacobi, m4d::vec5 &rayMaxJacobi, int &numPoints );


    void  setBoundingBox  ( const double p1[4], const double p2[4] );
    void  setBoundingBox  ( const m4d::vec4 p1, const m4d::vec4 p2 );
    void  getBoundingBox  ( double p1[4], double p2[4] );
    void  setBoundingTime ( double minTime,  double maxTime );
    void  getBoundingTime ( double &minTime, double &maxTime );

    void errorMessage ( m4d::enum_break_condition brCond ) const;

    void Print ( FILE* fptr = stderr );

protected:
    bool   outsideBoundingBox ( const double* pos );

private:
    m4d::Metric*     mMetric;
    m4d::enum_geodesic_type  mGeodType;
    m4d::enum_time_direction mTimeDir;

    m4d::Geodesic* m4dSolver;
    m4d::enum_integrator m4dGeodSolverType;
    std::string solverName;

    double epsilon_abs;
    double epsilon_rel;

    bool   stepSizeControlled;
    double stepSize;       //!< Initial stepsize for calculateGeodesic
    double maxStepsize;    //!< Maximum stepsize for calculation


    // The bounding box parameters for the four coordinates
    double  boundBoxMin[4];
    double  boundBoxMax[4];

};

#endif


#include "GvsGeodSolver.h"

#include <metric/m4dMetricDatabase.h>
#include <motion/m4dMotionList.h>
#include <motion/m4dMotionDatabase.h>
#include <m4dGlobalDefs.h>


GvsGeodSolver::GvsGeodSolver( m4d::Metric *metric, m4d::enum_integrator m4dGeodSolver )
    : mMetric(metric),
      mGeodType(m4d::enum_geodesic_lightlike),
      mTimeDir(m4d::enum_time_forward)
{
    epsilon_abs = 1.0e-4;
    epsilon_rel = 0.0;

    stepSizeControlled = true;
    stepSize = 0.01;
    maxStepsize = DEF_MAX_STEPSIZE;

    m4dSolver = nullptr;
    setSolver( m4dGeodSolver );

    int solverID = static_cast<int>(m4dGeodSolver);
    if (solverID>=0 && solverID < (int)m4d::NUM_GEOD_SOLVERS) {
        solverName = std::string(m4d::stl_solver_names[static_cast<int>(m4dGeodSolver)]);
    } else {
        solverName = std::string("Unknown");
    }
}

GvsGeodSolver::GvsGeodSolver( m4d::Metric *metric, m4d::enum_geodesic_type gType,
                              m4d::enum_time_direction tDir, m4d::enum_integrator m4dGeodSolver )
    : mMetric(metric),
      mGeodType(gType),
      mTimeDir(tDir)
{
    epsilon_abs = 1.0e-4;
    epsilon_rel = 0.0;

    stepSizeControlled = true;
    stepSize = 0.01;
    maxStepsize = DEF_MAX_STEPSIZE;

    m4dSolver = nullptr;
    setSolver( m4dGeodSolver );

    int solverID = static_cast<int>(m4dGeodSolver);
    if (solverID>=0 && solverID < (int)m4d::NUM_GEOD_SOLVERS) {
        solverName = std::string(m4d::stl_solver_names[static_cast<int>(m4dGeodSolver)]);
    } else {
        solverName = std::string("Unknown");
    }
}


GvsGeodSolver::~GvsGeodSolver() {
    delete m4dSolver;
    mMetric = nullptr;
}


void GvsGeodSolver::setMetric( m4d::Metric* metric) {
    m4dSolver->setMetric(metric);
}

m4d::Metric* GvsGeodSolver::getMetric() {
    return m4dSolver->getMetric();
}

bool GvsGeodSolver::setSolver( m4d::enum_integrator m4dGeodSolver ) {
    if (mMetric == nullptr) {
        return false;
    }

    if (m4dGeodSolver==m4d::gsUnknown) {
        return false;
    }

    if (m4dSolver!=nullptr) {
        delete m4dSolver;
    }
    m4dSolver=nullptr;

    m4d::IntegratorDatabase intDB;
    m4dGeodSolverType = m4dGeodSolver;
    m4dSolver = intDB.getIntegrator(mMetric, m4dGeodSolver);
    solverName = intDB.getIntegratorName(m4dGeodSolver);
    return true;
}

void GvsGeodSolver::setGeodType( m4d::enum_geodesic_type gType ) {
    m4dSolver->setGeodesicType(gType);
    mGeodType = gType;
}

m4d::enum_geodesic_type GvsGeodSolver::getGeodType() const {
    return mGeodType;
}

void GvsGeodSolver::setTimeDir( m4d::enum_time_direction tDir ) {
    mTimeDir = tDir;
}

m4d::enum_time_direction GvsGeodSolver::getTimeDir() const {
    return mTimeDir;
}


void GvsGeodSolver::setEpsilons( double eps_a, double eps_r ) {
    //cerr << "GvsGeodSolverGSLode::setEpsilons" << std::endl;
    epsilon_abs = eps_a;
    epsilon_rel = eps_r;
    m4dSolver->setEpsilons(eps_a, eps_r);
}

void GvsGeodSolver::getEpsilons( double &eps_a, double &eps_r ) {
    eps_a = epsilon_abs;
    eps_r = epsilon_rel;

}

void GvsGeodSolver::setStepSizeControl ( const bool cn ) {
    m4dSolver->setStepSizeControlled(cn);
    stepSizeControlled = cn;
}

bool GvsGeodSolver::getStepSizeControl() {
    return stepSizeControlled;
}

void GvsGeodSolver::setStepsize( double step ) {
    stepSize = step;
}

double GvsGeodSolver::getStepsize() const {
    return stepSize;
}

void GvsGeodSolver::setMaxStepsize( double step ) {
    maxStepsize = step;
}

double GvsGeodSolver::getMaxStepsize() const {
    return maxStepsize;
}


int GvsGeodSolver::startConditionLocal( const m4d::vec4* , m4d::vec4 &dir ) {
    int l;
    double norm,d;
    // zeit- und lichtartige Geodaeten
    if ( (mGeodType != m4d::enum_geodesic_spacelike) ) {

        norm = dir[1]*dir[1] + dir[2]*dir[2] + dir[3]*dir[3];
        if (mGeodType==m4d::enum_geodesic_lightlike)  // --- lichtartige Geodaeten
        {
            if (norm <= 0.0) {
                std::cerr << "Error: no direction!\n";
                return 1;
            }
            norm = sqrt(norm);
            for ( l = 1; l < 4; l++) {
                dir[l] /= norm;
            }
            d = 1.0;
        }
        else // --- zeitartige Geodaeten
        {
            double vq = norm;
            if (vq<1.0) {
                double gamma = 1.0/sqrt(1.0-vq);
                for (int l=1;l<4;l++)
                    dir[l] = gamma*dir[l];
                d = gamma;
            }
            else {
                std::cerr << "GvsGeodSolverBase::startConditionLocal()... Lichtgeschwindigkeit ueberschritten!" << std::endl;
                return 1;
            }
        }

        if (mTimeDir==m4d::enum_time_forward)
            dir[0] = d;
        else
            dir[0] = -d;
    }
    else    // -------------------- mGeodType==spacelike --------------------
    {
        norm = dir[1]*dir[1] + dir[2]*dir[2] + dir[3]*dir[3];
        norm = sqrt(norm);
        for ( l =1; l < 4; l++)
            dir[l] /= norm;

        dir[0] = 0.0;
    }
    return 0;
}


int GvsGeodSolver::startCondition ( const m4d::vec4* pos, m4d::vec4 &dir ) {
    double p[4] = {pos->x(0),pos->x(1),pos->x(2),pos->x(3)};
    double v[4] = {dir.x(0),dir.x(1),dir.x(2),dir.x(3)};
    startCondition(p,v);
    for (int i=0;i<4;i++)
        dir.setX(i,v[i]);

    return 0;
}

int GvsGeodSolver::startCondition(const double pos[], double dir[] ) {
    // The time component of the velocity u is
    // determined by the condition for a geodesic:
    //    g_(mu,nu) * u^mu * u^nu = -kappa
    //
    //    kappa = 1 (timelike geodesics)
    //    kappa = 0 (null geodesics)
    //    (consider [Wald])
    //
    // This yields a quadratic equation for u^0 = u^t:
    //     a * (u^0)^2  +  b * u^0  +  c   =  0,
    // where  a = g_(0,0),
    //        b = 2 * sum_i=1..3 (g_(0,i) * u^i)
    //        c = sum_i,j=1..3 (g_(i,j) * u^i * u^j) + kappa

    // std::cerr << "GvsGeodSolverBase::startCondition()\n";

    int i,j,l;
    double a,b,c,norm;
    double u1,u2,w;

    mMetric->calculateMetric(pos);   // Fills the metric coefficients

    // std::cerr << GvsGeodesicTypeName[mGeodType+1] << std::endl;

    // zeit- und raumartige Geodaeten
    if ( (mGeodType != m4d::enum_geodesic_spacelike) )
    {

        // Normalizes the spatial coordinates to unit length if null geodesic
        if (mGeodType==m4d::enum_geodesic_lightlike)
        {
            norm = dir[1]*dir[1] + dir[2]*dir[2] + dir[3]*dir[3];

            if (norm <= 0.0)
            {
                std::cerr << "Error: no direction!\n";
                return 1;
            }
            norm = sqrt(norm);
        }
        else norm = 1.0;
        //  std::cerr << "dir: " << dir[0] << " " << dir[1] << " " << dir[2] << " " << dir[3] << std::endl;
        for ( l = 1; l < 4; l++)
        {
            dir[l] /= norm;
        }
        //    std::cerr << "dir: " << dir[0] << " " << dir[1] << " " << dir[2] << " " << dir[3] << std::endl;

        a = mMetric->getMetricCoeff(0,0);
        b = 0.0;
        c = 0.0;

        for ( i = 1; i < 4; i++)
        {
            b += mMetric->getMetricCoeff(0,i)*dir[i];
            for ( j = 1; j < 4; j++)
            {
                c += mMetric->getMetricCoeff(i,j)*dir[i]*dir[j];
            }
        }
        b *= 2.0;
        c += mGeodType;

        // The solution of the quadratic equation is:
        //  u^t = (-b +- sqrt(b*b - 4*a*c)) / 2*a

        double  d = b*b - 4.0*a*c;
        if (fabs(d)<GVS_EPS) d=0.0;
        if (d < 0.0)
        {
            std::cerr << "d: " << d << std::endl;
            std::cerr << "dir: " << dir[1] << "  "  << dir[2] << "  " << dir[3] << std::endl;
            //  std::cerr << "Metric:" << std::endl;
            //mMetric->printMetric();
            std::cerr << "Error in GvsGeodSolver::startCondition: cannot find time-like component" << std::endl;
            return 1;
        }

        d = sqrt(d);
        a *= 2.0;
        u1 = (-b + d) / a;
        u2 = (-b - d) / a;

        // Schnitt mit Lichtkegel liegt entweder auf dem Vorwaertslichtkegel: max(u1,u2)
        // oder auf dem Rueckwaertslichtkegel: min(u1,u2)
        // BEACHTE: Beim Raytracing verfolgt man Lichtstrahlen in die Vergangenheit, jedoch
        // laeuft der Strahl selbst vorwaerts in der Zeit!
        //cerr << "u1,u2: " << u1 << " " << u2 << std::endl;


        if (mTimeDir==m4d::enum_time_forward)
            w = GVS_MAX(u1,u2);
        else
            w = GVS_MIN(u1,u2);

        dir[0] = w;

        //    std::cerr << "dir1: " << dir[0] << " " << dir[1] << " " << dir[2] << " " << dir[3] << std::endl << std::endl;


    }
    else    // -------------------- mGeodType==spacelike --------------------
    {
    }

    return 0;
}


double GvsGeodSolver::calcNormCondition ( const m4d::vec4 pos, const m4d::vec4 dir ) {
    double position[4]  = {pos.x(0),pos.x(1),pos.x(2),pos.x(3)};
    double direction[4] = {dir.x(0),dir.x(1),dir.x(2),dir.x(3)};
    return calcNormCondition(position,direction);
}


double GvsGeodSolver::calcNormCondition ( const double pos[], const double dir[] ) {
    int i,j;
    mMetric->calculateMetric(pos);

    double sum = 0.0;
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            sum += mMetric->getMetricCoeff(i,j)*dir[i]*dir[j];
        }
    }
    return sum + (double)mGeodType;
}



m4d::enum_break_condition
GvsGeodSolver::calculateGeodesic( const m4d::vec4& yStart, const m4d::vec4& yDir,
                                    const double maxNumPoints,
                                    m4d::vec4 *&points, m4d::vec4 *&dirs, int &numPoints )
{
    // std::cerr << "Starte CalcGeod tg\n";
    m4dSolver->setMaxAffineParamStep(maxStepsize);
    m4dSolver->setAffineParamStep(stepSize);
    return m4dSolver->calculateGeodesic(yStart, yDir, maxNumPoints, points, dirs, numPoints);
}

m4d::enum_break_condition
GvsGeodSolver::calcSachsJacobi( const m4d::vec4 &startOrig, const m4d::vec4 &startDir, const m4d::vec3 &localDir,
                                  const int maxNumPoints,
                                  const GvsLocalTetrad *lt,
                                  m4d::vec4 *&points, m4d::vec4 *&dirs, double *&lambda, m4d::vec4 *&sachs1, m4d::vec4 *&sachs2,
                                  m4d::vec5 *&rayJacobi, m4d::vec5 &rayMaxJacobi, int &numPoints )
{
    m4dSolver->setMaxAffineParamStep(maxStepsize);
    m4dSolver->setAffineParamStep(stepSize);
    m4d::vec3 locX = m4d::vec3(1.0,0.0,0.0);
    m4d::vec3 locY = m4d::vec3(0.0,1.0,0.0);
    m4d::vec3 locZ = m4d::vec3(0.0,0.0,1.0);
    return m4dSolver->calcSachsJacobi(startOrig, startDir, localDir,locX,locY,locZ,
                                      lt->getE(0),lt->getE(1),lt->getE(2),lt->getE(3),
                                      lt->getLFType(),maxNumPoints,
                                      points,dirs,lambda,sachs1,sachs2,rayJacobi,rayMaxJacobi,numPoints);
}

m4d::enum_break_condition
GvsGeodSolver::calcParTransport( const m4d::vec4& yStart, const m4d::vec4& yDir, const m4d::vec4 base[],
                                   const double maxNumPoints,
                                   GvsLocalTetrad *&lt, int &numPoints )
{
    //return m4d::enum_break_other;
    m4dSolver->setMaxAffineParamStep(maxStepsize);
    m4dSolver->setAffineParamStep(stepSize);
    std::vector<double> lambdas;
    std::vector<m4d::vec4> points;
    std::vector<m4d::vec4> dirs;
    // ToDo Stimmt lt=e ???
    std::vector<m4d::vec4> lt0, lt1, lt2, lt3;

    m4d::enum_break_condition breakCond = m4dSolver->calcParTransport(yStart, yDir, base[0], base[1], base[2], base[3], maxNumPoints, points, dirs, lambdas, lt0, lt1, lt2, lt3);

    //m4d::vec4 acc, olddir;
    //double oldtime = 0;
    numPoints = points.size();
    if (numPoints<2) {
        return m4d::enum_break_other;
    }

    lt = new GvsLocalTetrad[numPoints];
    for (unsigned int i=0; i<points.size(); i++) {
        lt[i].setLocalTime(lambdas[i]);
        lt[i].setMetric(mMetric);
        lt[i].setPosition(points[i]);
        lt[i].setVelocity(dirs[i]);
        //for (int j= 0; j<4; j++){ acc.setX(j,(dirs[i][j]-olddir[j])/(points[i][0]-oldtime));    }
        //oldtime=points[i][0];
        //olddir=dirs[i];
        //lt[i].setAccel(acc); ???
        lt[i].setTetrad(lt0[i],lt1[i],lt2[i],lt3[i]);
    }
    return breakCond;
}


void GvsGeodSolver::setBoundingBox( const double p1[4], const double p2[4] ) {
    m4dSolver->setBoundingBox(p1, p2);
}

void GvsGeodSolver::setBoundingBox( const m4d::vec4 p1, const m4d::vec4 p2 ) {
    m4dSolver->setBoundingBox(p1, p2);
}

void GvsGeodSolver::getBoundingBox( double p1[4], double p2[4] ) {
    m4d::vec4 v1, v2;
    m4dSolver->getBoundingBox(v1, v2);
    int i;
    for (i=0; i<4; i++) {
        p1[i]=v1[i];
        p2[i]=v2[i];
    }
}


void GvsGeodSolver::setBoundingTime(double minTime, double maxTime) {
    m4d::vec4 min, max;
    m4dSolver->getBoundingBox(min, max);
    min[0] = minTime;
    max[0] = maxTime;
    m4dSolver->setBoundingBox(min, max);
}

void GvsGeodSolver::getBoundingTime(double &minTime, double &maxTime) {
    m4d::vec4 min, max;
    m4dSolver->getBoundingBox(min, max);
    minTime = min[0];
    maxTime = max[0];
}


void GvsGeodSolver::errorMessage( m4d::enum_break_condition brCond ) const {
    fprintf(stderr,"GvsGeodSolver::warning: %s\n",m4d::stl_break_condition[static_cast<int>(brCond)]);
}


void GvsGeodSolver::Print( FILE* fptr ) {
    fprintf(fptr,"%s {\n",solverName.c_str());
    fprintf(fptr,"\ttype :    %s\n",m4d::stl_geodesic_type[mGeodType]);
    fprintf(fptr,"\tTimeDir : %s\n",m4d::str_time_dir[((mTimeDir+1)/2)]);
    fprintf(fptr,"\tboxMin  : %6.3e %6.3e %6.3e %6.3e\n",
            boundBoxMin[0],boundBoxMin[1],boundBoxMin[2],boundBoxMin[3]);
    fprintf(fptr,"\tboxMax  : %6.3e %6.3e %6.3e %6.3e\n",
            boundBoxMax[0],boundBoxMax[1],boundBoxMax[2],boundBoxMax[3]);
    fprintf(fptr,"\tstepCtr : %s\n", (stepSizeControlled?"yes":"no"));
    fprintf(fptr,"\teps_abs : %6.3f\n",epsilon_abs);
    fprintf(fptr,"\teps_rel : %6.3f\n",epsilon_rel);
    fprintf(fptr,"}");
}

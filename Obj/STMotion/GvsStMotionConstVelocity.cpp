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

#include "GvsStMotionConstVelocity.h"


GvsStMotionConstVelocity::GvsStMotionConstVelocity()
    : GvsStMotion()
{
    mType      = gvsMotionConstVelocity;
    mMetric = NULL;

    mBoosts.clear();
    mRotations.clear();

    mNrMotions = mNrBoosts = mNrRotations = 0;
}

GvsStMotionConstVelocity::GvsStMotionConstVelocity(m4d::Metric* spacetime)
    : GvsStMotion()
{
    //    std::cerr << "Initialize GvsStMotionConstVelocity..." << std::endl;
    mMetric = spacetime;
    mType      = gvsMotionConstVelocity;

    if (!localTetrad.empty()) deleteAllEntries();

    mBoosts.clear();
    mRotations.clear();

    mNrMotions = mNrBoosts = mNrRotations = 0;
}

GvsStMotionConstVelocity::~GvsStMotionConstVelocity()
{
    mBoosts.clear();
    mRotations.clear();
}


void GvsStMotionConstVelocity::setBoost( m4d::vec3 boost ) {
    mBoosts.push_back(boost);
    mNrBoosts = mBoosts.size();
    // mBoosts[mNrBoosts-1].print(cerr);

    GvsMotionCVTypeNum ct = {gvsMotionCVboost,mNrBoosts-1};
    mMotions.push_back(ct);
    //mMotions.push_back((GvsMotionCVTypeNum){gvsMotionCVboost,mNrBoosts-1});
    mNrMotions++;
}


void GvsStMotionConstVelocity::setRotation( m4d::vec4 rot ) {
    m4d::vec3 axis = (rot.getAsV3D()).getNormalized();
    assert( axis != m4d::vec3());

    m4d::vec4 r = m4d::vec4( rot.x(0), axis.x(0),axis.x(1),axis.x(2) );
    mRotations.push_back(rot);
    mNrRotations = mRotations.size();
    //  mRotations[mNrRotations-1].print(cerr);

    GvsMotionCVTypeNum ct = {gvsMotionCVrot,mNrRotations-1};
    mMotions.push_back(ct);
    //mMotions.push_back((GvsMotionCVTypeNum){gvsMotionCVrot,mNrRotations-1});
    mNrMotions++;
}


m4d::mat4 GvsStMotionConstVelocity::getBoostMatrix( const int nr ) const {
    assert( nr < mNrBoosts );
    m4d::vec3 boost = mBoosts[nr];

    double vel = boost.getNorm();
    double v1  = boost.x(0);
    double v2  = boost.x(1);
    double v3  = boost.x(2);

    double gamma = 1.0/sqrt(1.0-vel*vel);
    double f = gamma*gamma/(1.0+gamma);

    m4d::mat4 boostMatrix;

    boostMatrix.setRow(0, m4d::vec4( gamma,      -gamma*v1,   -gamma*v2,   -gamma*v3 ) );
    boostMatrix.setRow(1, m4d::vec4(-gamma*v1, 1.0+f*v1*v1,     f*v1*v2,     f*v1*v3 ) );
    boostMatrix.setRow(2, m4d::vec4(-gamma*v2,     f*v2*v1, 1.0+f*v2*v2,     f*v2*v3 ) );
    boostMatrix.setRow(3, m4d::vec4(-gamma*v3,     f*v3*v1,     f*v3*v2, 1.0+f*v3*v3 ) );

    return boostMatrix;
}


m4d::mat4 GvsStMotionConstVelocity :: getRotMatrix ( const int nr, const double time ) const {
    std::cerr << "getRotMatrix\n";
    assert( nr < mNrRotations );
    m4d::vec3 axis = mRotations[nr].getAsV3D();
    axis.printC();

    axis.normalize();

    double w = mRotations[nr].x(0);

    m4d::mat4 delta;
    delta.setIdent();// simuliert Kronecker-delta

    m4d::mat4 rotMatrix;
    rotMatrix.setIdent();

    double sum = 0.0;
    if (w!=0.0)
    {
        for (int i=1; i<4; i++)
        {
            double angle = w*time;
            for (int j=1; j<4; j++)
            {
                sum=0.0;
                for (int k=1; k<4; k++)
                {
                    // sum+=gvsEpsSymb(i-1,j-1,k-1)*axis[k-1];  //TODO
                }
                rotMatrix.setElem(i,j,
                                   axis[i-1]*axis[j-1]
                        + (delta.getElem(i,j)-axis[i-1]*axis[j-1]) * cos(angle)
                        + sin(angle)*sum );
                // std::cerr << i << " " << j << " " << rotMatrix.getElem(i,j) << std::endl;
            }
        }
    }

    return rotMatrix;
}


GvsMotionCVType
GvsStMotionConstVelocity :: getMotionCVType ( const int nr ) const
{
    if ((nr<0) || (nr > static_cast<int>(mMotions.size())))
        return gvsMotionCVnone;

    return mMotions[nr].type;
}


bool
GvsStMotionConstVelocity :: getTransformedPolygon ( const int ,
                                                    const m4d::vec4& p0in, const m4d::vec4& p1in,
                                                    m4d::vec4& p0out, m4d::vec4& p1out )
{
    // Lorentz-Transformation des Lichtstrahls
    // siehe Sexl,Urbantke : "Relativitaet, Gruppen, Teilchen"

    int numBoosts = 0;
    int numRot = 0;

    p0out = p0in;
    p1out = p1in;

    for (int i=0; i < mNrMotions; i++) {
        m4d::vec3 p0 = p0out.getAsV3D();
        m4d::vec3 p1 = p1out.getAsV3D();

        switch (mMotions[i].type)
        {
            case gvsMotionCVnone: {
                break;
            }
            case gvsMotionCVboost: {
                //std::cerr << "Boost\n";
                m4d::vec3 mVelocity = mBoosts[numBoosts];

                double vel = mVelocity.getNorm();
                double gamma = 1.0/sqrt(1.0-vel*vel);

                double vx0 = p0|mVelocity;
                double t0  = gamma*(p0out[0]-vx0);
                m4d::vec3 p0new = p0 + (gamma*gamma/(gamma+1.0)*vx0)*mVelocity - (gamma*p0out[0])*mVelocity;
                p0out = m4d::vec4(t0,p0new[0],p0new[1],p0new[2]);

                double vx1 = p1|mVelocity;
                double t1  = gamma*(p1out[0]-vx1);
                m4d::vec3 p1new = p1 + (gamma*gamma/(gamma+1.0)*vx1)*mVelocity - (gamma*p1out[0])*mVelocity;
                p1out = m4d::vec4(t1,p1new[0],p1new[1],p1new[2]);
                numBoosts++;
                break;
            }
            case gvsMotionCVrot: {
                //std::cerr << "Rotation\n";
                double omega = mRotations[numRot].x(0);
                m4d::vec3 alpha = mRotations[numRot].getAsV3D();
                alpha.normalize();

                double a0  = omega * p0out[0];
                double ax0 = alpha|p0;
                m4d::vec3 akx0  = alpha^p0;
                m4d::vec3 p0new = ax0*alpha + (p0-ax0*alpha)*cos(a0) - akx0*sin(a0);
                p0out = m4d::vec4(p0out[0],p0new[0],p0new[1],p0new[2]);

                double a1  = omega * p1out[0];
                double ax1 = alpha|p1;
                m4d::vec3 akx1  = alpha^p1;
                m4d::vec3 p1new = ax1*alpha + (p1-ax1*alpha)*cos(a1) - akx1*sin(a1);
                p1out = m4d::vec4(p1out[0],p1new[0],p1new[1],p1new[2]);
                numRot++;
                break;
            }
        }
    }
    return true;
}


void GvsStMotionConstVelocity :: Print ( FILE* fptr ) {
    fprintf(fptr,"-----------------------------------------\n");
    fprintf(fptr,"              Motion:\n");
    fprintf(fptr,"-----------------------------------------\n");
    fprintf(fptr,"\tMotionType:     %s\n",GvsMotionTypeName[mType].c_str());
    fprintf(fptr,"\t#Motions:       %d\n",mNrMotions);
    for (int i=0; i<mNrMotions; i++) {
        if (mMotions[i].type == gvsMotionCVboost) {
            fprintf(fptr,"\t\t Boost:    \n");
            mBoosts[mMotions[i].nr].printC();
        }
        else if (mMotions[i].type == gvsMotionCVrot) {
            fprintf(fptr,"\t\t Rotation: \n");
            mRotations[mMotions[i].nr].printC();
        }
    }
    fprintf(fptr,"\n");
}

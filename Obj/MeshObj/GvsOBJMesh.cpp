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

#include "GvsOBJMesh.h"
#include "Ray/GvsSurfIntersec.h"
#include "math/TransfMat.h"
#include <fstream>


typedef struct hit_object_struct {
    unsigned int faceID;
    double      r;
    double      s;
} hit_object_t;



GvsOBJMesh::GvsOBJMesh(GvsSurfaceShader* shader) : GvsSurface(shader),
    mObjOffsets(NULL),mNumDrawObjects(0),mNumAllObjVertices(0) {
    mHaveSetParamTransfMat = false;

    AddParam("objfilename",gvsDT_STRING);
}


GvsOBJMesh::GvsOBJMesh(const char* pathname, const char* filename, GvsSurfaceShader* shader, m4d::Metric *metric, GvsObjType objType) :
    GvsSurface(shader), mObjOffsets(NULL),mNumDrawObjects(0),mNumAllObjVertices(0)  {

    mObjType = objType;
    mMetric = metric;

    mHaveSetParamTransfMat = false;
    volTransfMat.setIdent();
    volInvTransfMat.setIdent();
    volParamTransfMat.setIdent();
    volParamInvTransfMat.setIdent();

    mPathname = std::string(pathname);
    mFilename = std::string(filename);    
    ReadObjFile(pathname,filename);

    AddParam("objfilename",gvsDT_STRING);
}


GvsOBJMesh::~GvsOBJMesh() {
    ClearAll();
}


bool GvsOBJMesh::ReadObjFile( const char* pathname, const char* filename ) {
    std::vector<std::vector<std::string> > tokens;

    std::string fn = std::string(pathname) + "/" + std::string(filename);
    if (!tokenizeFile( fn, tokens )) {
        return false;
    }
    ClearAll();
    //fprintf(stderr,"Reed %d lines ...\n",(int)tokens.size());

    obj_tag_t  tag;
    tag.materialID = -1;

    m4d::vec3   v;
    m4d::vec3   n;
    obj_face_t  f;
    m4d::vec2   tc;

    mCenterOfVertices = m4d::vec3();

    for(unsigned int i=0; i<tokens.size(); i++) {
        if (tokens[i].size()==0)
        continue;

        std::string baseString = tokens[i][0];
        if (baseString.compare("mtllib")==0 && tokens[i].size()>1) {
            mMTLname = tokens[i][1];
            ReadMtlFile(pathname,mMTLname.c_str());
        }
        else if (baseString.compare("usemtl")==0 && tokens[i].size()>1) {
            mTags.push_back(tag);
            lowCase(tokens[i][1]);
            mMaterialNamesItr = mMaterialNames.find(tokens[i][1]);
            if (mMaterialNamesItr != mMaterialNames.end()) {
               tag.materialID = mMaterialNamesItr->second;
            }
            if (!tag.vFaceNums.empty()) {
                tag.vFaceNums.clear();
            }
        }
        else if (baseString.compare("v")==0 && tokens[i].size()>1) {
            if (readVertex( tokens[i], v )) {
                mCenterOfVertices += v;
                //fprintf(stderr,"v: %f %f %f %f\n",v.x,v.y,v.z,v.w);
                mVertices.push_back(v);
            }
        }
        else if (baseString.compare("vn")==0 && tokens[i].size()>3) {
            if (readNormal( tokens[i], n )) {
                //fprintf(stderr,"vn: %f %f %f\n",n.x,n.y,n.z);
                mNormals.push_back(n);
            }
        }
        else if (baseString.compare("vt")==0 && tokens[i].size()>2) {
            if (readTexCoord( tokens[i], tc )) {
                //fprintf(stderr,"vt: %f %f\n",tc.x,tc.y);
                mTexCoords.push_back(tc);
            }
        }
        else if (baseString.compare("f")==0 && tokens[i].size()>1) {
            if (readFace( tokens[i], f )) {
                mFaces.push_back(f);
                int numFaces = static_cast<int>(mFaces.size());
                tag.vFaceNums.push_back(numFaces-1);
            }
        }
    }
    mTags.push_back(tag);

    /*
    fprintf(stderr,"\nOBJ file \'%s\' has the following basic entries:\n",fn.c_str());
    fprintf(stderr,"-----------------------------------\n");
    if (mMTLname.compare("")!=0)
        fprintf(stderr,"MTLlib      : %s\n",mMTLname.c_str());
    fprintf(stderr,"# Vertices  : %d\n",(int)mVertices.size());
    fprintf(stderr,"# Normals   : %d\n",(int)mNormals.size());
    fprintf(stderr,"# TexCoords : %d\n",(int)mTexCoords.size());
    fprintf(stderr,"# Faces     : %d\n",(int)mFaces.size());
    fprintf(stderr,"# Tags      : %d\n",(int)mTags.size());
    fprintf(stderr,"-----------------------------------\n");
    //printAllTags();
*/

    /*
    for(unsigned int i=0; i<mFaces.size(); i++) {
        obj_face_t face = mFaces[i];
        fprintf(stderr,"%2d %2d %2d\n",face[0].vID,face[1].vID,face[2].vID);
    }
    */

    /*
    for(unsigned int i=0; i<mVertices.size(); i++) {
        mVertices[i].print();
    }
    */


    float mm = 1.0f/(float)mVertices.size();
    mCenterOfVertices = mCenterOfVertices*mm;
    //fprintf(stderr,"center: %f %f %f\n",mCenterOfVertices.x(0),mCenterOfVertices.x(1),mCenterOfVertices.x(2));
    return true;
}


bool GvsOBJMesh::ReadMtlFile(const char* pathname, const char* filename) {
    fprintf(stderr,"Read mtl file...\n");
    std::vector<std::vector<std::string> >  tokens;

    std::string fn = std::string(pathname) + "/" + std::string(filename);
    if (!tokenizeFile( fn, tokens ))
        return false;

    obj_material_t* mat;
    std::string     matName     = std::string();
    int currMaterialID = -1;
    int currTexID = -1;

    for(unsigned int i=0; i<tokens.size(); i++) {
        if (tokens[i].size()==0)
            continue;

        std::string baseString = tokens[i][0];
        if (baseString.compare("newmtl")==0 && tokens[i].size()>1) {
            matName = tokens[i][1];
            lowCase(matName);
            mat = new obj_material_t;
            mat->Ns = 0.0f;
            mat->Ni = 0.0f;
            mat->d  = 0.0f;
            mat->Ka[0] = mat->Ka[1] = mat->Ka[2] = 0.0f;
            mat->Ks[0] = mat->Ks[1] = mat->Ks[2] = 0.0f;
            mat->Kd[0] = mat->Kd[1] = mat->Kd[2] = 0.0f;
            mat->Ke[0] = mat->Ke[1] = mat->Ke[2] = 0.0f;
            mat->mapID = 0;
            mat->mapTexOffset[0] = mat->mapTexOffset[1] = mat->mapTexOffset[2] = 0.0f;
            mat->mapTexScale[0] = mat->mapTexScale[1] = mat->mapTexScale[2] = 1.0f;
            mMaterial.push_back(mat);
            currMaterialID = static_cast<int>(mMaterial.size())-1;
            mMaterialNames.insert(std::pair<std::string,int>(matName,currMaterialID));
        }
        else if (baseString.compare("Ns")==0 && tokens[i].size()>1 && currMaterialID!=-1) {
            if (currMaterialID < static_cast<int>(mMaterial.size())) {
                mMaterial[currMaterialID]->Ns = static_cast<float>(atof( tokens[i][1].c_str() ));
            }
        }
        else if (baseString.compare("Ni")==0 && tokens[i].size()>1 && currMaterialID!=-1) {
            if (currMaterialID < static_cast<int>(mMaterial.size())) {
                mMaterial[currMaterialID]->Ni = static_cast<float>(atof( tokens[i][1].c_str() ));
            }
        }
        else if (baseString.compare("d")==0 && tokens[i].size()>1 && currMaterialID!=-1) {
            if (currMaterialID < static_cast<int>(mMaterial.size())) {
                mMaterial[currMaterialID]->d = static_cast<float>(atof( tokens[i][1].c_str() ));
            }
        }
        else if (baseString.compare("Ka")==0 && tokens[i].size()>3 && currMaterialID!=-1) {
            if (currMaterialID < static_cast<int>(mMaterial.size())) {
                mMaterial[currMaterialID]->Ka[0] = static_cast<float>(atof( tokens[i][1].c_str() ));
                mMaterial[currMaterialID]->Ka[1] = static_cast<float>(atof( tokens[i][2].c_str() ));
                mMaterial[currMaterialID]->Ka[2] = static_cast<float>(atof( tokens[i][3].c_str() ));
            }
        }
        else if (baseString.compare("Kd")==0 && tokens[i].size()>3 && currMaterialID!=-1) {
           if (currMaterialID < static_cast<int>(mMaterial.size())) {
                mMaterial[currMaterialID]->Kd[0] = static_cast<float>(atof( tokens[i][1].c_str() ));
                mMaterial[currMaterialID]->Kd[1] = static_cast<float>(atof( tokens[i][2].c_str() ));
                mMaterial[currMaterialID]->Kd[2] = static_cast<float>(atof( tokens[i][3].c_str() ));
            }
        }
        else if (baseString.compare("Ks")==0 && tokens[i].size()>3 && currMaterialID!=-1) {
            if (currMaterialID < static_cast<int>(mMaterial.size())) {
                mMaterial[currMaterialID]->Ks[0] = static_cast<float>(atof( tokens[i][1].c_str() ));
                mMaterial[currMaterialID]->Ks[1] = static_cast<float>(atof( tokens[i][2].c_str() ));
                mMaterial[currMaterialID]->Ks[2] = static_cast<float>(atof( tokens[i][3].c_str() ));
            }
        }
        else if (baseString.compare("Ke")==0 && tokens[i].size()>3 && currMaterialID!=-1) {
            if (currMaterialID < static_cast<int>(mMaterial.size())) {
                mMaterial[currMaterialID]->Ke[0] = static_cast<float>(atof( tokens[i][1].c_str() ));
                mMaterial[currMaterialID]->Ke[1] = static_cast<float>(atof( tokens[i][2].c_str() ));
                mMaterial[currMaterialID]->Ke[2] = static_cast<float>(atof( tokens[i][3].c_str() ));
            }
        }
        else if ((baseString.compare("map")==0 || baseString.compare("map_bump")==0)&& currMaterialID!=-1) {
            if (currMaterialID < static_cast<int>(mMaterial.size())) {
                mTexNamesItr = mTexNames.find(tokens[i][1]);
                if (mTexNamesItr==mTexNames.end()) {
                    mTexNames.insert(std::pair<std::string,int>(tokens[i][1],++currTexID));
                } else {
                    currTexID = mTexNamesItr->second;
                }
                if (currTexID>=0) {
                    mMaterial[currMaterialID]->mapID = currTexID;

                    if (tokens[i].size()>5) {
                        if (tokens[i][1]=="-s") {
                            mMaterial[currMaterialID]->mapTexScale[0] = static_cast<float>(atof(tokens[i][2].c_str()));
                            mMaterial[currMaterialID]->mapTexScale[1] = static_cast<float>(atof(tokens[i][2].c_str()));
                            mMaterial[currMaterialID]->mapTexScale[2] = static_cast<float>(atof(tokens[i][2].c_str()));
                        }
                    }
                }
            }
            /*
            mMaterialItr = mMaterial.find(currMatName);
            if (mMaterialItr!=mMaterial.end()) {
                if (tokens[i].size()>5) {
                    if (tokens[i][1]=="-s") {
                        mMaterialItr->second->mapTexScale[0] = static_cast<float>(atof(tokens[i][2].c_str()));
                        mMaterialItr->second->mapTexScale[1] = static_cast<float>(atof(tokens[i][3].c_str()));
                        mMaterialItr->second->mapTexScale[2] = static_cast<float>(atof(tokens[i][4].c_str()));
                    }
                    //mMaterialItr->second->map = tokens[i][5];
                }
                else if (tokens[i].size()>1) {
                    //mMaterialItr->second->map = tokens[i][1];
                }
            }
            */
        }
        else if (baseString.compare("bump_map")==0 && tokens[i].size()>1 && currMaterialID!=-1) {
            if (currMaterialID < static_cast<int>(mMaterial.size())) {

            }
            /*
            mMaterialItr = mMaterial.find(currMatName);
            if (mMaterialItr!=mMaterial.end()) {
                //mMaterialItr->second->bumpMap = tokens[i][1];
            }
            */
        }
    }

#if 0
    for(unsigned int m=0; m<mMaterial.size(); m++) {
        PrintMaterial(m);
    }
#endif
    return true;
}

/**
 * @brief GvsOBJMesh::ClearAll
 */
void GvsOBJMesh::ClearAll() {
    mFilename = std::string();
    mMTLname  = std::string();

    if (!mVertices.empty()) {
        mVertices.clear();
    }
    if (!mNormals.empty()) {
        mNormals.clear();
    }
    if (!mTexCoords.empty()) {
        mTexCoords.clear();
    }

    if (!mFaces.empty())
        mFaces.clear();

    if (!mTags.empty())
        mTags.clear();

    if (!mMaterial.empty()) {
        for(unsigned int i=0; i<mMaterial.size(); i++) {
            if (mMaterial[i]!=NULL) {
                delete mMaterial[i];
                mMaterial[i] = NULL;
            }
        }
        mMaterial.clear();
    }

    /*
    if (!mMaterialNames.empty()) {
        mMaterialNames.clear();
    }
    if (!mTexNames.empty()) {
        mTexNames.clear();
    }
    if (!objList.empty()) {
        objList.clear();
    }
    clearObjPointers();
    */
}

/**
 * @brief GvsOBJMesh::PrintFacePoint
 * @param fp
 * @param fptr
 */
void GvsOBJMesh::PrintFacePoint( obj_face_point_t &fp, FILE* fptr ) {
    fprintf(fptr,"%d/%d/%d\n",fp.vID,fp.texID,fp.nID);
    fprintf(fptr,"%f %f %f\n",mVertices[fp.vID].x(0),mVertices[fp.vID].x(1),mVertices[fp.vID].x(2));
    fprintf(fptr,"%f %f\n",mTexCoords[fp.texID].x(0),mTexCoords[fp.texID].x(1));
}

/**
 * @brief GvsOBJMesh::PrintMaterial
 * @param materialID
 * @param fptr
 */
void GvsOBJMesh::PrintMaterial( int materialID, FILE* fptr ) {
    if (materialID>=0 && materialID < static_cast<int>(mMaterial.size())) {
        obj_material_t* mat = mMaterial[materialID];
        fprintf(fptr,"\nMaterialID: %d\n",materialID);
        fprintf(fptr," Ns: %6.4f\n",mat->Ns);
        fprintf(fptr," Ni: %6.4f\n",mat->Ni);
        fprintf(fptr,"  d: %6.4f\n",mat->d);
        fprintf(fptr," Ka: %6.4f %6.4f %6.4f\n",mat->Ka[0],mat->Ka[1],mat->Ka[2]);
        fprintf(fptr," Kd: %6.4f %6.4f %6.4f\n",mat->Kd[0],mat->Kd[1],mat->Kd[2]);
        fprintf(fptr," Ks: %6.4f %6.4f %6.4f\n",mat->Ks[0],mat->Ks[1],mat->Ks[2]);
        fprintf(fptr," Ke: %6.4f %6.4f %6.4f\n",mat->Ke[0],mat->Ke[1],mat->Ke[2]);
    }
   // fprintf(fptr,"map: %s\n",mat->map.c_str());
}

/**
 * @brief GvsOBJMesh::printAllTags
 * @param fptr
 */
void GvsOBJMesh::PrintAllTags( FILE* fptr ) {
    for(unsigned int i=0; i<mTags.size(); i++) {
        fprintf(fptr,"\nTag #%d:\n",i);
        fprintf(fptr,"MaterialID: %d\n",mTags[i].materialID);
        fprintf(fptr,"Faces: ");
        for(unsigned int j=0; j<mTags[i].vFaceNums.size(); j++)
            fprintf(fptr,"%d ",mTags[i].vFaceNums[j]);
        fprintf(fptr,"\n");
    }
}

/**
 * @brief GvsOBJMesh::clearObjPointers
 */
void GvsOBJMesh::clearObjPointers() {
    if (mObjOffsets!=NULL) {
       delete [] mObjOffsets;
    }
    mObjOffsets = NULL;
}

/**
 * @brief GvsOBJMesh::readVertex
 * @param tokenrow
 * @param v
 * @return
 */
bool GvsOBJMesh::readVertex(std::vector<std::string> &tokenrow, m4d::vec3 &v ) {
    v = m4d::vec3(0,0,0);

    unsigned int num = 0;
    while (num+1<tokenrow.size() && num<4) {
        if (tokenrow[num].compare("")!=0 && tokenrow[num].compare(" ")!=0) {
            v[num] = static_cast<float>(atof(tokenrow[num+1].c_str()));
            num++;
        }
    }
    if (num==0)
        return false;
    return true;
}

/**
 * @brief GvsOBJMesh::readNormal
 * @param tokenrow
 * @param n
 * @return
 */
bool GvsOBJMesh::readNormal( std::vector<std::string> &tokenrow, m4d::vec3 &n ) {
    n = m4d::vec3();

    if (tokenrow.size()<4)
        return false;

    n = m4d::vec3(static_cast<float>(atof(tokenrow[1].c_str())),
                  static_cast<float>(atof(tokenrow[2].c_str())),
                  static_cast<float>(atof(tokenrow[3].c_str())));
    return true;
}

/**
 * @brief GvsOBJMesh::readTexCoord
 * @param tokenrow
 * @param tc
 * @return
 */
bool GvsOBJMesh::readTexCoord( std::vector<std::string> &tokenrow, m4d::vec2 &tc ) {
    tc = m4d::vec2();

    if (tokenrow.size()<2)
        return false;

    tc[0] = static_cast<float>(atof(tokenrow[1].c_str()));
    tc[1] = static_cast<float>(atof(tokenrow[2].c_str()));
    //fprintf(stderr,"tc: %f %f\n",tc[0],tc[1]);
    return true;
}

/**
 * @brief GvsOBJMesh::readFace
 * @param tokenrow
 * @param face
 * @return
 */
bool GvsOBJMesh::readFace( std::vector<std::string> &tokenrow, obj_face_t &face ) {
    if (!face.empty())
        face.clear();

    std::string::size_type fpos,lpos;
    for(unsigned int i=1; i<tokenrow.size(); i++) {
        obj_face_point_t p;

        fpos = tokenrow[i].find_first_of("/");
        if (fpos != std::string::npos) {
            lpos = tokenrow[i].find_last_of("/");

            p.vID   = atoi(tokenrow[i].substr(0,fpos).c_str());
            p.texID = atoi(tokenrow[i].substr(fpos+1,lpos-fpos-1).c_str());
            p.nID   = atoi(tokenrow[i].substr(lpos+1,tokenrow[i].length()-lpos-1).c_str());
        } else {
            p.vID = p.texID = p.nID = atoi(tokenrow[i].c_str());
        }
        //fprintf(stderr,"%d/%d/%d\n",p.vID,p.texID,p.nID);
        face.push_back(p);
    }
    return true;
}

/**
 * @brief GvsOBJMesh::tokenizeFile
 * @param filename
 * @param tokens
 * @param useStandardIgnoreTokens
 * @return
 */
bool GvsOBJMesh::tokenizeFile( const std::string filename, std::vector<std::vector<std::string> > &tokens, bool useStandardIgnoreTokens ) {
    if (filename[0]=='\0') {
        return 0;
    }

    std::ifstream in(filename.c_str());
    if (!in) {
        fprintf(stderr,"Cannot open file \"%s\"\n",filename.c_str());
        return false;
    }

    do {
        std::string line;
        getline(in, line);

        bool ignoreLine = false;
        if (!line.compare(0,1,""))
            ignoreLine = true;

        if (useStandardIgnoreTokens) {
            if (!line.compare(0,1,"#"))
            ignoreLine = true;
        }

        if (!ignoreLine) {
            std::string buf;
            std::stringstream ss(line);
            std::vector<std::string> line_tokens;

            while(ss >> buf)
                line_tokens.push_back(buf);

            tokens.push_back(line_tokens);
        }
    } while (!in.eof());
    in.close();
    return true;
}


void GvsOBJMesh::lowCase( std::string &s ) {
    int l = (int)s.length();
    for(int i=0;i<l;i++) {
        s[i] = tolower(s[i]);
    }
}

GvsBoundBox GvsOBJMesh::boundingBox() const {
    return meshBoundBox;
}

void GvsOBJMesh::calcNormal( GvsSurfIntersec & ) const {
    // intentionally left blank
}


void GvsOBJMesh::scale ( const m4d::vec3 &scaleVec) {
    volTransfMat = m4d::ScaleMat3D ( scaleVec ) * volTransfMat;
    volInvTransfMat *= m4d::ScaleMat3D ( 1.0/scaleVec.x(0), 1.0/scaleVec.x(1), 1.0/scaleVec.x(2) );
    //calcBoundBox();
}


void GvsOBJMesh :: translate ( const m4d::vec3 &transVec) {
    volTransfMat = m4d::TranslateMat3D ( transVec ) * volTransfMat;
    m4d::vec3  minTrans = -transVec;
    volInvTransfMat *= m4d::TranslateMat3D ( minTrans );
    //calcBoundBox();
}


void GvsOBJMesh :: rotate ( const m4d::vec3 &rotAxis, double rotAngle) {
    //volTransfMat = m4d::RotateMat3D ( rotAxis, rotAngle ) * volTransfMat;
    //volInvTransfMat *= m4d::RotateMat3D ( rotAxis, -rotAngle);

    //rotAxis.print();
    std::cerr << "GvsOBJMesh::rotate() ... " << rotAngle << std::endl;

    m4d::Matrix<double,3,4> mat = m4d::TranslateMat3D(mCenterOfVertices) * m4d::RotateMat3D(rotAxis,-rotAngle) * m4d::TranslateMat3D(-mCenterOfVertices);
    m4d::Matrix<double,3,4> invMat = mat;
    invMat.invert();
    //invMat = m4d::TranslateMat3D(-mCenterOfVertices) * m4d::RotateMat3D(rotAxis,rotAngle) * m4d::TranslateMat3D(mCenterOfVertices);

    volTransfMat = mat * volTransfMat;
    volInvTransfMat = volInvTransfMat * invMat;
    //calcBoundBox();
}


void GvsOBJMesh :: transform ( const m4d::Matrix<double,3,4> &mat) {
    //  std::cerr << "GvsSolConvexPrim :: transform()...\n";
    if ( !mat.isIdentMat() ) {
        volTransfMat = mat * volTransfMat;
        m4d::Matrix<double,3,4> invMat = mat;
        invMat.invert();
        volInvTransfMat = volInvTransfMat * invMat;
       // calcBoundBox();
    }
}



bool GvsOBJMesh::testIntersection( GvsRay &ray ) {
    assert( mMetric != NULL );
    //std::cerr << "Error in GvsSolidObj::testIntersection(GvsRay&): not implemented." << std::endl;

    int chart0,chart1;

    m4d::enum_coordinate_type  coords = mMetric->getCoordType();

    int maxSeg   = ray.getNumPoints()-2;
    int startSeg = int(0);
    int endSeg   = maxSeg;

    // --- loop over all segments of the ray
    for (int seg = startSeg; seg < endSeg; seg++) {
        m4d::vec4 p0 = ray.getPoint(seg);
        m4d::vec4 p1 = ray.getPoint(seg+1);

        m4d::vec4 p0trans4D = p0;
        m4d::vec4 p1trans4D = p1;

        chart0 = chart1 = 0;
        if (coords != m4d::enum_coordinate_cartesian)  {
            chart0 = mMetric->transToPseudoCart( p0, p0trans4D );
            chart1 = mMetric->transToPseudoCart( p1, p1trans4D );
        }

        if (chart0!=mChart && chart1!=mChart) {
            continue;
        }

        if (haveMotion) {
            m4d::vec4 p0motTrans4D = p0trans4D;
            m4d::vec4 p1motTrans4D = p1trans4D;
            stMotion->getTransformedPolygon(seg,p0motTrans4D,p1motTrans4D, p0trans4D, p1trans4D);
        }
        m4d::vec4 vtrans4D  = p1trans4D - p0trans4D;

        m4d::vec3 p0trans,p1trans;
        if (mHaveSetParamTransfMat) {
            p0trans = volParamInvTransfMat * p0trans4D.getAsV3D();
            p1trans = volParamInvTransfMat * p1trans4D.getAsV3D();
        }
        else {
            p0trans = volInvTransfMat * p0trans4D.getAsV3D();
            p1trans = volInvTransfMat * p1trans4D.getAsV3D();
        }

        m4d::vec3 vtrans = p1trans - p0trans;

        double tp0 = p0trans4D.x(0);
        double tp1 = p1trans4D.x(0);
        double tHit,alpha;

        m4d::vec3 rayIntersecPt;
        m4d::vec3 rayIntersecNormal;
        m4d::vec2 rayIntersecTexUV;
        if (rayIntersect(p0trans,p1trans,tp0,tp1,alpha,tHit,rayIntersecPt,rayIntersecNormal,rayIntersecTexUV)) {            
            if (GvsRay::isIn(seg,alpha,maxSeg) && ray.isValidSurfIntersec( GvsRay::calcRayDist(seg,alpha))) {

                //std::cerr << "intersec " << alpha << " " << tHit << std::endl;

                GvsSurfIntersec surfIntersec;
                surfIntersec.setDist ( GvsRay::calcRayDist(seg,alpha) );
                surfIntersec.setSurface ( this );

                // global intersection point and direction in proper metric coordinates
                m4d::vec4 point = p0trans4D + alpha * vtrans4D;
                m4d::vec4 dir   = vtrans4D;
                m4d::enum_coordinate_type cType = mMetric->getCoordType();
                m4d::TransCoordinates::coordTransf(m4d::enum_coordinate_cartesian,point,dir,cType,point,dir);

                surfIntersec.setPoint( point );
                surfIntersec.setDirection( dir );

                // local intersection point in standard object system
                //surfIntersec.setLocalPoint( p0trans + alpha * vtrans );
                surfIntersec.setLocalPoint( rayIntersecPt );
                surfIntersec.setLocalDirection( vtrans );

                surfIntersec.setNormal(rayIntersecNormal);
                surfIntersec.setTexUVParam(rayIntersecTexUV.x(0),rayIntersecTexUV.x(1));

                surfIntersec.setRaySegNumber(seg);
                if (ray.store( surfIntersec ) == GvsRayStatus::finished) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool GvsOBJMesh::testLocalIntersection( GvsRay &, const int , const int ,
                                         GvsLocalTetrad* , GvsLocalTetrad* ,
                                         const m4d::vec4 , const m4d::vec4 )
{
    std::cerr << "Error in GvsSolidObj::testLocalIntersection(GvsRay&): not implemented." << std::endl;
    return false;
}


bool GvsOBJMesh :: rayIntersect(const m4d::vec3& p0, const m4d::vec3& p1,
                                 double tp0, double tp1, double &alpha,
                                 double &thit,
                                 m4d::vec3& rayIntersecPnt , m4d::vec3 &rayIntersecNormal, m4d::vec2 &rayIntersecTexUV) const
{

    // TODO: test bounding box!

    double delta_t = tp1-tp0;

    m4d::vec3 P = p0;
    m4d::vec3 d = p1-p0;

    int v1id, v2id, v3id;
    m4d::vec3 A,B,C, u,v,w, dv,wu;
    double t,r,s,hn;

    bool hit = false;
    hit_object_t hitObj;

    alpha=1.0;
    for(unsigned int fid = 0; fid < mFaces.size(); fid++ ) {
        obj_face_t face = mFaces[fid];
        if (face.size()<3) {
            continue;
        }

        v1id = face[0].vID - 1;
        v2id = face[1].vID - 1;
        v3id = face[2].vID - 1;

        A = mVertices[v1id];
        B = mVertices[v2id];
        C = mVertices[v3id];

        u = B - A;
        v = C - A;
        w = P - A;

        dv = d^v;
        wu = w^u;

        if (dv.isZero() || wu.isZero()) {
            continue;
        }

        hn = dv|u;
        if (fabs(hn)<1e-6) {
            continue;
        }

        hn = 1.0/hn;

        t = hn * (wu|v);
        r = hn * (dv|w);
        s = hn * (wu|d);

        if (r>=0 && r<=1 && s>=0 && s<=1 && s+r<=1 && t>=0 && t<=1) {
            // hit
            if (t<alpha) {
                alpha = t;
                {
                    //fprintf(stderr," %f %f %f \n",t,r,s);
                    hit = true;
                    hitObj.faceID = fid;
                    hitObj.r = r;
                    hitObj.s = s;
                }
            }
        }
    }

    if (hit) {
        thit = tp0 + delta_t * alpha;
        rayIntersecPnt = P + alpha*d;
        unsigned int fid = hitObj.faceID;

        m4d::vec3 n1 = mNormals[mFaces[fid][0].nID-1];
        m4d::vec3 n2 = mNormals[mFaces[fid][1].nID-1];
        m4d::vec3 n3 = mNormals[mFaces[fid][2].nID-1];

        m4d::vec2 t1 = mTexCoords[mFaces[fid][0].texID-1];
        m4d::vec2 t2 = mTexCoords[mFaces[fid][1].texID-1];
        m4d::vec2 t3 = mTexCoords[mFaces[fid][2].texID-1];

        rayIntersecNormal = (1-hitObj.r-hitObj.s)*n1 + hitObj.r*n2 + hitObj.s*n3;
        rayIntersecTexUV  = (1-hitObj.r-hitObj.s)*t1 + hitObj.r*t2 + hitObj.s*t3;

        //rayIntersecPnt.print();
        //rayIntersecNormal.print();
        //rayIntersecTexUV.print();
        return true;
    }

    return false;
}


int GvsOBJMesh::SetParam ( std::string pName, std::string objFilename ) {
    int isOkay = GvsBase::SetParam(pName,objFilename);
    if (isOkay >= gvsSetParamNone && getLowCase(pName)=="objfilename") {
        isOkay &= ReadObjFile(mPathname.c_str(), objFilename.c_str());
    }
    return isOkay;
}


int GvsOBJMesh::SetParam( std::string pName, m4d::Matrix<double,3,4> mat ) {
    int isOkay = GvsBase::SetParam(pName,mat);
    if (isOkay >= gvsSetParamNone && getLowCase(pName)=="transform")   {
        volParamTransfMat = mat * volTransfMat;
        volParamInvTransfMat = volParamTransfMat;
        volParamInvTransfMat.invert();
        mHaveSetParamTransfMat = true;
    }
    return isOkay;
}


bool GvsOBJMesh::haveSetParamTransfMat () const {
    return mHaveSetParamTransfMat;
}

void GvsOBJMesh::Print ( FILE* fptr ) {
    PrintAllTags(fptr);
}

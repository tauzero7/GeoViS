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

#ifndef GVS_OBJ_MESH_H
#define GVS_OBJ_MESH_H

#include <Obj/GvsSurface.h>
#include <Ray/GvsRay.h>
#include <iostream>

//! Face point structure for OBJ files.
typedef struct obj_face_point_struct {
    int  vID;    // vertex ID
    int  texID;  // texture ID
    int  nID;    // normal ID
    obj_face_point_struct() {
        vID = -1;
        texID = -1;
        nID = -1;
    }
} obj_face_point_t;

typedef std::vector<obj_face_point_t>  obj_face_t;

//! Tag structure for OBJ files.
typedef struct obj_tag_struct {
    int              materialID;
    std::vector<int> vFaceNums;
    obj_tag_struct() {
        materialID = -1;
    }
} obj_tag_t;

typedef struct obj_draw_struct {
    std::vector<m4d::vec4> vert;
    std::vector<m4d::vec3> norm;
    std::vector<m4d::vec2> tc;
    int  materialID;
    obj_draw_struct() {
        materialID = -1;
    }
} obj_draw_t;

//! Material structure for OBJ files.
typedef struct obj_material_struct {
    float Ns;      //!< Specular component of the Phong shading model ranges between 0 and 1000
    float Ni;      //
    float d;       //
    float Tr;      // alpha transparency
    float Tf[3];
    int   illum;   // the illumination model to be used by the material
                   //  0: no lighting
                   //  1: diffuse lighting only
                   //  2: both diffuse lighting and specular highlights
    float Ka[3];   //!< Ambient color
    float Kd[3];   //!< Diffuse color
    float Ks[3];   //!< Specular color
    float Ke[3];   //!< Emission color

    float mapTexOffset[3];  // "-o u v w" texture offset option
    float mapTexScale[3];   // "-s u v w" texture scaling option

    unsigned int  mapID;
    unsigned int  bumpMapID;

    obj_material_struct() {
        Tr = 0.0f;
        illum = 0;
        Ka[0] = Ka[1] = Ka[2] = 1.0f;
        Kd[0] = Kd[1] = Kd[2] = 1.0f;
        Ks[0] = Ks[1] = Ks[2] = 0.0f;
        Ke[0] = Ke[1] = Ke[2] = 0.0f;
        mapTexOffset[0] = mapTexOffset[1] = mapTexOffset[2] = 0.0f;
        mapTexScale[0] = mapTexScale[1] = mapTexScale[2] = 1.0f;
        mapID = 0;
        bumpMapID = 0;
    }
} obj_material_t;



class GvsLocalTetrad;
class GvsSolObjSpanList;


class GvsOBJMesh : public GvsSurface {
public:
    GvsOBJMesh(GvsSurfaceShader* shader);
    GvsOBJMesh( const char* pathname, const char* filename,
                GvsSurfaceShader* shader, m4d::Metric* spacetime, GvsObjType objType=local);
    virtual ~GvsOBJMesh();

    bool  ReadObjFile( const char* pathname, const char* filename );
    bool  ReadMtlFile( const char* pathname, const char* filename );
    void  ClearAll();

    void   PrintFacePoint ( obj_face_point_t &fp, FILE* fptr = stderr );
    void   PrintMaterial  ( int materialID, FILE* fptr = stderr );
    void   PrintAllTags   ( FILE* fptr = stderr );

    virtual GvsBoundBox    boundingBox() const;

    virtual void calcNormal( GvsSurfIntersec &  intersec ) const;
    virtual void scale     ( const m4d::vec3 &scaleVec);
    virtual void translate ( const m4d::vec3 &transVec);
    virtual void rotate    ( const m4d::vec3 &rotAxis, double rotAngle);
    virtual void transform ( const m4d::Matrix<double,3,4> &mat);

    virtual bool testIntersection      ( GvsRay &ray );
    
    virtual bool testLocalIntersection ( GvsRay &ray, const int startSeg, const int endSeg,
                                         GvsLocalTetrad* lt0, GvsLocalTetrad* lt1,
                                         const m4d::vec4 p0, const m4d::vec4 p1 );


    virtual int  SetParam ( std::string pName, std::string objFilename );
    virtual int  SetParam ( std::string pName, m4d::Matrix<double,3,4> mat );

    virtual bool haveSetParamTransfMat () const;

    virtual void Print ( FILE* fptr = stderr );


protected:
    void clearObjPointers();
    bool readVertex   ( std::vector<std::string> &tokenrow, m4d::vec3 &v );
    bool readNormal   ( std::vector<std::string> &tokenrow, m4d::vec3 &n );
    bool readTexCoord ( std::vector<std::string> &tokenrow, m4d::vec2 &tc );
    bool readFace     ( std::vector<std::string> &tokenrow, obj_face_t   &face );
    bool tokenizeFile ( const std::string filename, std::vector<std::vector<std::string> > &tokens, bool useStandardIgnoreTokens = true );
    void lowCase ( std::string &s );    

    bool rayIntersect ( const m4d::vec3& p0, const m4d::vec3& p1,
                        double tp0, double tp1,
                        double &alpha, double &thit,
                        m4d::vec3& rayIntersecPnt,
                        m4d::vec3& rayIntersecNormal,
                        m4d::vec2& rayIntersecTexUV ) const;



protected:
    std::string mPathname;
    std::string mFilename;
    std::string mMTLname;
    long        mNumVertices;
    long        mNumNormals;
    long        mNumTexCoords;

    std::vector<m4d::vec3>  mVertices;
    std::vector<m4d::vec3>  mNormals;
    std::vector<m4d::vec2>  mTexCoords;
    std::vector<obj_face_t> mFaces;
    std::vector<obj_tag_t>  mTags;

    std::vector<obj_material_t*> mMaterial;
    std::map<std::string,int>            mMaterialNames;
    std::map<std::string,int>::iterator  mMaterialNamesItr;
    std::map<std::string,int>            mTexNames;
    std::map<std::string,int>::iterator  mTexNamesItr;

    m4d::vec3   mCenterOfVertices;

    unsigned int* mObjOffsets;
    unsigned int  mNumDrawObjects;
    unsigned int  mNumAllObjVertices;

    GvsBoundBox  meshBoundBox;

    m4d::Matrix<double,3,4>   volTransfMat;
    m4d::Matrix<double,3,4>   volInvTransfMat;

    m4d::Matrix<double,3,4>   volParamTransfMat;
    m4d::Matrix<double,3,4>   volParamInvTransfMat;
    bool         mHaveSetParamTransfMat;
};


#endif // GVS_OBJ_MESH_H

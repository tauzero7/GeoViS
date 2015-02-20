
#version 330                              

uniform sampler2D texImageID;
uniform sampler2D texDataPos;
uniform sampler2D texDataDir;
uniform sampler2D texDataS;
uniform int channel; 
uniform float timeParam;
uniform vec4 minS;
uniform vec4 maxS;

#define FLUX_REL    9.167e-4
#define FLUX_MAX    0.1

#define s3   1.732050808  //  sqrt(3)
#define s3h  0.866025404  //  sqrt(3/2)
#define PI   3.141592653
    
layout(location = 0) out vec4 fragColor;  
in vec2 texCoords;             

float mass = 2.0;
float angmom = 0.576;
vec3 q = normalize(vec3(0.1,0.0,0.1));


vec4 coordToLocal( vec4 pos, vec4 cdir ) {
    float r     = pos.y;
    float theta = pos.z;

    float sigma = r*r + angmom*angmom*cos(theta)*cos(theta);
    float delta = r*r - 2.0*mass*r + angmom*angmom;
    float A = (r*r+angmom*angmom)*sigma + 2.0*mass*angmom*angmom*r*sin(theta)*sin(theta);
    float pm = 1.0;
    
    if (1==2)  // static
    {
        float w = sqrt(1.0-2.0*mass*r/sigma);
        float ldir0 = w*cdir[0] + 2.0*mass*angmom*r*sin(theta)*sin(theta)/(w*sigma)*cdir[3];
        float ldir1 = sqrt(sigma/delta)*cdir[1];
        float ldir2 = sqrt(sigma)*cdir[2];
        float ldir3 = pm/w*sqrt(delta)*sin(theta)*cdir[3];
        return vec4(ldir0,ldir1,ldir2,ldir3);
    }
    else
    {
        float omega = 2.0*mass*angmom*r/A;
        float ldir0 = sqrt(delta*sigma/A)*cdir[0];
        float ldir1 = sqrt(sigma/delta)*cdir[1];
        float ldir2 = sqrt(sigma)*cdir[2];
        float ldir3 = sqrt(A/sigma)*sin(theta)*(cdir[3]-omega*cdir[0]);
        return vec4(ldir0,ldir1,ldir2,ldir3);
    }
    return vec4(0);
}

void main() {                           
    vec4 color = vec4(vec3(0),1);
    
    vec4 pos = texture2D( texDataPos, texCoords );
    vec4 dir = texture2D( texDataDir, texCoords );
         
    if (texture(texDataS,texCoords).r==2) {
        float freq = 8;
        float omega = 1.0;
          
        float phi = pos.w + omega*timeParam + 0.022*pos.x;
        
        float checkerboard = 0.8+0.2*sign(sin(phi*freq)*sin(pos.z*freq));
        color.rgb = vec3(checkerboard);
        
        
        vec4 ldir = coordToLocal(pos,dir);                
        // simple shading
        color.rgb = vec3(dot(normalize(ldir.yzw),vec3(1,0,0)))*checkerboard;
        
        vec3 d = vec3(sin(pos.z)*cos(phi),sin(pos.z)*sin(phi),cos(pos.z));
        if (max(0,dot(d,q))>0.99 && pos.z>0) color.rgb = vec3(1,0,0)*checkerboard;
        
        float one_plus_z = ldir.x;        
        one_plus_z = (one_plus_z - minS[1]) / (maxS[1] - minS[1]);
        //color.rgb = vec3(pow(one_plus_z,-4.0)*2.0);
        //one_plus_z = sign(sin(one_plus_z*2*PI*10));
        color.rgb = vec3(one_plus_z);
        
    }
    fragColor = color;
}

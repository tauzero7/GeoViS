
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
float rs = 2.0*mass;
vec3 q = normalize(vec3(0.1,0.0,0.1));


vec4 coordToLocal( vec4 pos, vec4 cdir ) {
    float r     = pos.y;
    float theta = pos.z;

    float w = sqrt(1.0-rs/r);
    
    float ldir0 = cdir[0]*w;
    float ldir1 = cdir[1]/w;
    float ldir2 = cdir[2]*r;
    float ldir3 = cdir[3]*r*sin(theta);
    return vec4(ldir0,ldir1,ldir2,ldir3);
}

void main() {                           
    vec4 color = vec4(vec3(0),1);
    
    vec4 pos = texture2D( texDataPos, texCoords );
    vec4 dir = texture2D( texDataDir, texCoords );
         
    if (texture(texDataS,texCoords).r==2) {
        float freq = 8;
        float omega = 1.0;
          
        float checkerboard = 0.8+0.2*sign(sin(pos.w*freq)*sin(pos.z*freq));
        color.rgb = vec3(checkerboard);
        
        
        vec4 ldir = coordToLocal(pos,dir);                
        // simple shading
        color.rgb = vec3(dot(normalize(ldir.yzw),vec3(1,0,0)))*checkerboard;
        
        float one_plus_z = ldir.x;        
        //one_plus_z = (one_plus_z - minS[1]) / (maxS[1] - minS[1]);
        //color.rgb = vec3(pow(one_plus_z,-4.0)*2.0);
        //color.rgb = vec3(one_plus_z);
        
    }
    fragColor = color;
}

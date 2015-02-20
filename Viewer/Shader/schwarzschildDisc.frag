
#version 330                              

uniform sampler2D texDataPos;
uniform sampler2D texDataDir;
uniform sampler2D texDataS;
uniform int channel; 

#define FLUX_REL    9.167e-4
#define FLUX_MAX    0.1

#define s3   1.732050808  //  sqrt(3)
#define s3h  0.866025404  //  sqrt(3/2)
#define PI   3.141592653
    
layout(location = 0) out vec4 frag_color;  
in vec2 texCoords;             

float rs   = 2.0;
float rObs = 10.0;            


float norm( vec4 pos, vec4 v, vec4 w ) {
    float r     = pos.y;
    float theta = pos.z;
    float t4 = -1.0+rs/r;
    float t6 = r*r;
    float t7 = sin(theta);
    float t8 = t7*t7;
    float g_compts[4];
    g_compts[0] = t4;
    g_compts[1] = -1.0/t4;
    g_compts[2] = t6;
    g_compts[3] = t6*t8;
    float sum = 0.0;
    for(int i=0; i<4; i++) 
        sum += g_compts[i]*v[i]*w[i];
    return sum;
}


void main() {                           
    vec4 color = vec4(0);
    
    vec4 pos = texture2D( texDataPos, texCoords );
    vec4 dir = texture2D( texDataDir, texCoords );
            
    float rsdri = rs/rObs;
    
    float v = 1.0/sqrt(2.0*(pos[1]/rs-1.0));
    float gamma = 1.0/sqrt(1.0-v*v);
    vec4 u = gamma*vec4(1.0/sqrt(1.0-rsdri),0,0,v/pos[1]);
    
    if (texture(texDataS,texCoords).r==3) {
        float R  = pos[1]/rs;
        float sR = sqrt(R);
        float flux = FLUX_MAX/FLUX_REL/(R-1.5)*pow(R,-2.5)*(sR - s3 + 0.5*s3h*log((sR+s3h)/(sR-s3h)*(s3-s3h)/(s3+s3h)));
        float opz = vec3(norm(pos,u,-dir));
        switch (channel) {
            default:
            case 1: {
                color.rgb = vec3(flux/(opz*opz*opz*opz));
                break;
            }
            case 2: {
                if (opz-1>0) {
                    color.r = sign(sin((opz-1.0)*2*PI*10))+1.4;
                } else {
                    color.b = sign(sin((opz-1.0)*2*PI*10))+1.4;
                }
                break;
            }
            case 3: {
                color.rgb = vec3(flux*2);
                break;
            }
            case 4: {
                color.rgb = vec3(sign(sin(pos.w*20.0)*sin(pos.y*2*PI*0.2)));
                break;
            }
        }
    } else {
        color.rgb = vec3(0.2);
    }
    frag_color = color;
}


#version 330                              

uniform sampler2D texImageID;
uniform sampler2D texDataPos;
uniform sampler2D texDataDir;
uniform sampler2D texDataS;

uniform int channel; 
uniform float timeParam;
uniform vec4 minS;
uniform vec4 maxS;

#define PI   3.141592653
    
layout(location = 0) out vec4 fragColor;  
in vec2 texCoords;             

float mass   = 1.0;
float angmom = 1.0;

float rmin = 6.0;
float rmax = 16.0;

vec3 colRed1 = vec3(0.8,0.16,0.16);
vec3 colRed2 = vec3(0.9,0.63,0.63);

void main() {                           
    vec4 color = vec4(vec3(0),1);
    
    vec4 pos = texture2D( texDataPos, texCoords );
    vec4 dir = texture2D( texDataDir, texCoords );
    
    int objID = int(texture2D( texDataS, texCoords ).x);
    float t   = pos[0];
    float r   = (pos[1]-rmin)/(rmax-rmin);
    float phi = pos[3];
    
    if (objID==1) {
        float val = sign(sin(12*PI*r))*sign(sin(30*phi + 1*(t-timeParam)));
        //color.rgb = vec3(0.2*val+0.8);
        color.rgb = 0.5*( (1.0-val)*colRed2 + (val+1.0)*colRed1 );
    }
    fragColor = color;
}

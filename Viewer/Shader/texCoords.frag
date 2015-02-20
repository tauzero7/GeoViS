
#version 330                              

uniform sampler2D texImageID;
uniform sampler2D texDataPos;
uniform sampler2D texDataDir;
uniform sampler2D texDataS;

layout(location = 0) out vec4 frag_color;  
in vec2 texCoords;                         

void main() {                           
    frag_color = vec4(texCoords,0,1);
}


#version 330                               

layout(location = 0) in vec2 in_position;  

out vec2 texCoords;                        
uniform mat4 mvp;                          

void main() {                              
    vec4 vert = vec4(in_position,0,1);       
    texCoords = vert.xy;   
    gl_Position = mvp * vert;
}


#version 330                              

uniform sampler2D texImageID;
uniform sampler2D texDataPos;
uniform sampler2D texDataDir;
uniform sampler2D texDataS;

uniform int channel; 
uniform int dataType;

uniform vec4 minPos;
uniform vec4 maxPos;
uniform vec4 minDir;
uniform vec4 maxDir;
uniform vec4 minS;
uniform vec4 maxS;

layout(location = 0) out vec4 frag_color;  
in vec2 texCoords;                         

vec3 hsv2rgb( vec3 hsv )
{
   /*
    * Purpose:
    * Convert HSV values to RGB values
    * All values are in the range [0.0 .. 1.0]
    */
   float S, H, V, F, M, N, K;
   int   I;

   H = hsv.r; //->h;  /* Hue */
   S = hsv.g; //->s;  /* Saturation */
   V = hsv.b; //->v;  /* value or brightness */

   vec3 rgb = vec3(0.0);
   if ( S == 0.0 ) {
      /*
       * Achromatic case, set level of grey
       */
      rgb.r = V;
      rgb.g = V;
      rgb.b = V;
   } else {
      /*
       * Determine levels of primary colours.
       */
      if (H >= 1.0) {
         H = 0.0;
      } else {
         H = H * 6;
      } /* end if */
      I = int(H);   /* should be in the range 0..5 */
      F = H - I;     /* fractional part */

      M = V * (1 - S);
      N = V * (1 - S * F);
      K = V * (1 - S * (1 - F));

      if (I == 0) { rgb.r = V; rgb.g = K; rgb.b = M; }
      if (I == 1) { rgb.r = N; rgb.g = V; rgb.b = M; }
      if (I == 2) { rgb.r = M; rgb.g = V; rgb.b = K; }
      if (I == 3) { rgb.r = M; rgb.g = N; rgb.b = V; }
      if (I == 4) { rgb.r = K; rgb.g = M; rgb.b = V; }
      if (I == 5) { rgb.r = V; rgb.g = M; rgb.b = N; }
   } /* end if */

   return rgb;
}


void main() {                           
    frag_color = vec4(0);
    if (dataType==-1) {
        frag_color = vec4(0);
    }
    else if (dataType==0) {
        frag_color = texture2D( texImageID, vec2(texCoords.x,1-texCoords.y) );
    } 
    else {
        switch (channel) {
            default: {
                //frag_color = vec4(texCoords,0,1);
                break;
            }
            case 1: {
                float val = texture2D( texDataPos, texCoords ).x;
                val = (val-minPos[0])/(maxPos[0]-minPos[0]);
                frag_color = vec4(vec3(val),1);
                break;
            }
            case 2: {
                float val = texture2D( texDataPos, texCoords ).y;
                val = (val-minPos[1])/(maxPos[1]-minPos[1]);
                frag_color = vec4(vec3(val),1);
                break;
            }
            case 3: {
                float val = texture2D( texDataPos, texCoords ).z;
                val = (val-minPos[2])/(maxPos[2]-minPos[2]);
                frag_color = vec4(vec3(val),1);
                break;
            }
            case 4: {
                float val = texture2D( texDataPos, texCoords ).w;
                val = (val-minPos[3])/(maxPos[3]-minPos[3]);
                frag_color = vec4(vec3(val),1);
                break;
            }
            case 5: {
                float val = texture2D( texDataDir, texCoords ).x;
                val = (val-minDir[0])/(maxDir[0]-minDir[0]);
                frag_color = vec4(vec3(val),1);
                break;            
            }
            case 6: {
                float val = texture2D( texDataDir, texCoords ).y;
                val = (val-minDir[1])/(maxDir[1]-minDir[1]);
                frag_color = vec4(vec3(val),1);
                break;            
            }
            case 7: {
                float val = texture2D( texDataDir, texCoords ).z;
                val = (val-minDir[2])/(maxDir[2]-minDir[2]);
                frag_color = vec4(vec3(val),1);
                break;            
            }
            case 8: {
                float val = texture2D( texDataDir, texCoords ).w;
                val = (val-minDir[3])/(maxDir[3]-minDir[3]);
                frag_color = vec4(vec3(val),1);
                break;            
            }
            case 9: { // objID   use different colors
                float val = texture2D( texDataS, texCoords ).x;
                val = (val-minS[0])/(maxS[0]-minS[0]);
                frag_color = vec4(vec3(val),1);
                break;
            }
            case 10: { // freq-shift
                float val = texture2D( texDataS, texCoords ).y;
                val = (val-minS[1])/(maxS[1]-minS[1]);
                frag_color = vec4(vec3(val),1);
                //frag_color = vec4( hsv2rgb(vec3(val,1,1)), 1);
                break;
            }
            case 11: {
                float val = texture2D( texDataS, texCoords ).z;
                val = (val-minS[2])/(maxS[2]-minS[2]);
                frag_color = vec4(vec3(val),1);
                break;
            }
            case 12: {
                float val = texture2D( texDataS, texCoords ).w;
                val = (val-minS[3])/(maxS[3]-minS[3]);
                //val = sign(sin(6.283*val*10));
                frag_color = vec4(vec3(val),1);
                break;
            }
        }
    }
    //frag_color = vec4(vec3(minDir[0]),1);
}

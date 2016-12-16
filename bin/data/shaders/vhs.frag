uniform float    time;
uniform float    chromaAb;
uniform float    waves;
uniform sampler2DRect diffuseTexture;

varying vec2 texCoord;

void main(){
   vec2 st = texCoord;
   vec4 c0 = texture2DRect(diffuseTexture, st);
   
   if(mod(floor(gl_FragCoord.y),waves*5.0)>0.5){
       float l = dot(c0.xyz, vec3(0.2126, 0.7152, 0.0722));
       gl_FragColor = l * c0;
       return;
   }
   
   float t = pow((((1.0 + sin(time*10.0) * 0.5)
                   * 0.8 + sin(time* cos(gl_FragCoord.y)*41415.92653) * 0.0125)
                  * 1.5 + sin(time*7.0) * 0.5), 5.0);
   
   t *= chromaAb;
   
   vec4 c1 = texture2DRect(diffuseTexture, st+vec2(t * 0.2,t * -0.2) );
   vec4 c2 = texture2DRect(diffuseTexture, st+vec2(t * 0.5,0.0) );
   vec4 c3 = texture2DRect(diffuseTexture, st+vec2(t * 0.9,t * 0.2) );
   
   gl_FragColor = vec4(c3.r, c2.g, c1.b, 1.0);
}
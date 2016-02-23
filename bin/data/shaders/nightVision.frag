//uniform sampler2D diffuseTexture;
////uniform sampler2D noiseTex; 
////uniform sampler2D maskTex;

//
//
//varying vec2 texCoord;
//
//void main ()
//{
////    vec4 finalColor;
////    vec2 uv = texCoord;
////    uv.x = 0.4*sin(time*50.0);                                 
////    uv.y = 0.4*cos(time*50.0);                                 
////    //float m = texture2D(maskTex, gl_TexCoord[0].st).r;
////    //vec3 n = texture2D(noiseTex,
////                 //(gl_TexCoord[0].st*3.5) + uv).rgb;
////    vec3 c = texture2DRect(diffuseTexture, uv).rgb;
////                               //+ (n.xy*0.005)).rgb;
////
////    float lum = dot(vec3(0.30, 0.59, 0.11), c);
////    if (lum < luminanceThreshold)
////      c *= colorAmplification; 
////
////    vec3 visionColor = vec3(0.1, 0.95, 0.2);
////    finalColor.rgb = (c /*+ (n*0.2)*/) * visionColor; //* m;
////    gl_FragColor.rgb = finalColor.rgb;
////    gl_FragColor.a = 1.0;
//    gl_FragColor = texture2DRect(diffuseTexture, texCoord);
//}

#version 120

uniform sampler2DRect diffuseTexture;
varying vec2 texCoord;

uniform float luminanceThreshold; // 0.2
uniform float colorAmplification; // 4.0
uniform float effectCoverage; // 0.5
uniform float time;


void main() {
//
    vec4 finalColor;
    vec2 uv = texCoord;
//    uv.x = 0.4*sin(time*50.0);
//    uv.y = 0.4*cos(time*50.0);
//    float m = texture2D(maskTex, gl_TexCoord[0].st).r;
//    //vec3 n = texture2D(noiseTex,
//                 //(gl_TexCoord[0].st*3.5) + uv).rgb;
    vec3 c = texture2DRect(diffuseTexture, uv).rgb;
                               //+ (n.xy*0.005)).rgb;
//
    float lum = dot(vec3(0.30, 0.59, 0.11), c);
    if (lum < luminanceThreshold)
      c *= colorAmplification;
//
    vec3 visionColor = vec3(0.1, 0.95, 0.2);
    finalColor = vec4(c * visionColor, 1.0);
//    finalColor.rgb = (c /*+ (n*0.2)*/) * visionColor; //* m;
    gl_FragColor = finalColor;
    
//    gl_FragColor = texture2DRect(diffuseTexture, texCoord);
}
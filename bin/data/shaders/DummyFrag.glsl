#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform vec2 resolution;
uniform vec2 size;
uniform float time;
uniform vec2 mouse;
uniform sampler2DRect diffuseTexture;
uniform float theMix;
uniform float weirdness;
uniform float amount;

varying vec2 texCoord;

float strength = 9.0; //  effect strength

vec4 sharp( sampler2DRect sampler, vec2 uv )
{
    vec4 c0 = texture2DRect(sampler , uv);
    vec4 c1 = texture2DRect(sampler , (uv-vec2(1.0,.0)));
    vec4 c2 = texture2DRect(sampler, (uv+vec2(1.0,.0)));
    vec4 c3 = texture2DRect(sampler, (uv-vec2(.0,1.0)));
    vec4 c4 = texture2DRect(sampler, (uv+vec2(.0,1.0)));
    vec4 c5 = c0+c1+c2+c3+c4;
    c5*=0.2;
    vec4 mi = min(c0,c1); mi = min(mi,c2); mi = min(mi,c3); mi = min(mi,c4);
    vec4 ma = max(c0,c1); ma = max(ma,c2); ma = max(ma,c3); ma = max(ma,c4);
    return clamp(mi,(strength+1.0)*c0-c5*strength,ma);
}

void main() {
    vec2 pos = gl_TexCoord[0].xy;
    
    vec4 col0 = sharp(diffuseTexture, texCoord);
    vec4 col1 = texture2DRect(diffuseTexture, texCoord );
    
    vec4 weird = abs( col0 - col1 ) * weirdness * 100.0;
    //weird = vec4(1.0) - weird;
    weird.a = 1.0;

    gl_FragColor = mix( col1, mix( weird, col0, theMix ), amount );
}
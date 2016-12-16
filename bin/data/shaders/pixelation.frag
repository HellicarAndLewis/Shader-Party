#version 120

uniform sampler2DRect diffuseTexture;
varying vec2 texCoord;
uniform vec2 diffuseTextureSize;
uniform float pixel_w; // 15.0
uniform float pixel_h; // 10.0

void main() {
    vec2 uv = texCoord;
//
    float dx = pixel_w;//*(1./diffuseTextureSize.x);
    float dy = pixel_h;//*(1./diffuseTextureSize.y);
    vec2 coord = vec2(dx*floor(uv.x/dx), dy*floor(uv.y/dy));
    vec3 tc = texture2DRect(diffuseTexture, coord).rgb;
    gl_FragColor = vec4(tc, 1.0);
}
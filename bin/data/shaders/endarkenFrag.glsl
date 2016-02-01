#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect diffuseTexture;
uniform vec2 diffuseTextureSize;
uniform float darkness;

void main()
{
    vec2 pos = gl_TexCoord[0].xy;
    vec2 diffuseTextureSize;
    float height = pos.y / 720;
    float invDarkness = 1 - darkness;
    vec4 color = texture2DRect(diffuseTexture, pos);
    if(height < 0.5 - invDarkness || height > 0.5 + invDarkness) color = vec4(0.0, 0.0, 0.0, 1.0);
    
    gl_FragColor = color;
}
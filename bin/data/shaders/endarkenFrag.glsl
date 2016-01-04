#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect diffuseTexture;
uniform float darkness;

void main()
{
    vec2 pos = gl_TexCoord[0].xy;
    
    vec4 color = texture2DRect(diffuseTexture, pos) * (1-darkness);

    gl_FragColor = color;
}
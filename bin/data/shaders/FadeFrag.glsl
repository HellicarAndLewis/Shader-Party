#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect texIn;
uniform sampler2DRect texOut;
uniform float fadeAmnt;

void main()
{
    vec2 pos = gl_TexCoord[0].xy;
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

    color += (1 - fadeAmnt) * texture2DRect(texOut, pos);
    color += fadeAmnt * texture2DRect(texIn, pos);

    gl_FragColor = color;
}
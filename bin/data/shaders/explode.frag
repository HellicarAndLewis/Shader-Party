#version 120

uniform sampler2DRect diffuseTexture;
varying vec2 texCoord;

void main() {
    gl_FragColor = texture2DRect(diffuseTexture, texCoord);
}
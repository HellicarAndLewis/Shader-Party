#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

#define MAX_SEEDS 400

uniform sampler2DRect diffuseTexture;
uniform float numActiveSeeds;
uniform vec2 locs[MAX_SEEDS];

float distance(vec2 loc1, vec2 loc2);

void main() {
    //Getting coordinates of the current pixel in texture
    vec2 pos = gl_TexCoord[0].xy;
    float minDistance = distance(pos, locs[0]);
    int nearestSeed = 0;
    
    for(int i=1; i<numActiveSeeds; i++) {
        float dist = distance(pos, locs[i]);
        if(dist < minDistance) {
            minDistance = dist;
            nearestSeed = i;
        }
    }
        
    vec4 color = texture2DRect(diffuseTexture, locs[nearestSeed]);//vec4(cols[nearestCol].r, cols[nearestCol].g, cols[nearestCol].b, 0.5);
    gl_FragColor = color;
}

float distance(vec2 loc1, vec2 loc2) {
    float xsqrd = (loc2.x - loc1.x) * (loc2.x - loc1.x);
    float ysqrd = (loc2.y - loc1.y) * (loc2.y - loc1.y);
    return sqrt(xsqrd + ysqrd);
}


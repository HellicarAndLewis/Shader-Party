#version 120

#define MAX_SEEDS 100

varying vec2 texCoord;
uniform float time;
uniform sampler2DRect diffuseTexture;
uniform float centerX;
uniform float centerY;
uniform float outCenterX;
uniform float outCenterY;
uniform float sections;
uniform float val;

const float PI = 3.141592658;
const float TAU = 2.0 * PI;

float map(float inValue, float inMin, float inMax, float outMin, float outMax) {
    float percent = (inValue - inMin) / (inMax - inMin);
    return percent * (outMax - outMin) + outMin;
}

void main() {
	vec2 pos = texCoord;
	vec2 center = vec2(centerX, centerY);

	vec2 outCenter = vec2(outCenterX, outCenterY);
	
	vec2 shiftedPos = center - pos;
	float rad = length(shiftedPos);
	float angle = atan(shiftedPos.y, shiftedPos.x);

	float ma = mod(angle + val, TAU/sections);
	ma = abs(ma - PI/sections);

	float x = cos(ma) * rad;
	float y = sin(ma) * rad;

	x += outCenter.x;
	y += outCenter.y;
    
    gl_FragColor = texture2DRect(diffuseTexture, vec2(x, y)); //vec4(angleScaled, 0, 0, 1);
}

// const int numPoints = 12;
// const bool showFolds = true;

// uniform vec2 locs[MAX_SEEDS];
// uniform vec2 normals[MAX_SEEDS];


// float rand( vec2 n ) {
// 	return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
// }

// struct Ray
// {
// 	vec2 point;
// 	vec2 direction;
// };

// float noise(vec2 n) {
// 	const vec2 d = vec2(0.0, 1.0);
// 	vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
// 	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
// 	// return 1.;
// }

// vec2 noise2(vec2 n)
// {
// 	// return vec2(noise(vec2(n.x+0.2, n.y-0.6)), noise(vec2(n.y+3., n.x-4.)));
// 	// return vec2(1., 1.);
// }

// Ray GetRay(float i)
// {
// 	// vec2 position = vec2(1, 1);
// 	// vec2 position = noise2(vec2(i*6.12+1*0.1, i*4.43+1*0.1));
// 	// vec2 direction = normalize(noise2(vec2(i*7.+1*0.05, i*6.))*2.0-1.0);
// 	// return Ray(
// 	// 	position,
// 	// 	direction
// 	// 	);
// 	// return Ray(
// 	// 	position,
// 	// 	normalize(noise2(vec2(i*7.+1*0.05, i*6.))*2.0-1.0));	
// }

// void main()
// {
// 	vec2 pos = texCoord;

// 	for(int i=0;i<numPoints;i++)
// 	{
// 		Ray ray = Ray(locs[i], normals[i]);
			
// 		if(length(ray.point-pos)<10.0)
// 		{
// 			gl_FragColor = vec4(1,1,1,1);
// 			return;
// 		}
// 		else if (length(pos-(ray.point+ray.direction*0.1))<10.0)
// 		{
// 			gl_FragColor = vec4(1,0,0,1);
// 			return;
// 		}
// 		else
// 		{
// 			float offset=dot(pos-ray.point, ray.direction);
// 			if(abs(offset)<1.0 && showFolds)
// 			{
// 				gl_FragColor = vec4(0,0,1,1);
// 				return;
// 			}
// 			if(offset<0.)
// 			{
// 				pos -= ray.direction*offset*2.0;
// 			}									
// 		}
// 	}									

// 	gl_FragColor = texture2DRect( diffuseTexture, pos );	
// }
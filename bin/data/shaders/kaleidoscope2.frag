const float tau = 6.28318530718;

varying vec2 texCoord;
uniform float time;
uniform sampler2DRect diffuseTexture;
uniform float slide1X;
uniform float slide1Y;
uniform float slide2X;
uniform float slide2Y;
uniform float shiftX;
uniform float shiftY;
uniform float sides;
uniform float rotation;
uniform float size;
uniform float angle;

vec2 pattern() {
	vec2 slide1 = vec2(slide1X, slide1Y);
	vec2 slide2 = vec2(slide2X, slide2Y);
	vec2 shift = vec2(shiftX, shiftY);

	float s = sin(tau * rotation);
	float c = cos(tau * rotation);
	vec2 tex = texCoord;
	float scale = 1.0 / max(size,0.001);
	vec2 point = vec2( c * tex.x - s * tex.y, s * tex.x + c * tex.y ) * scale;
	point = (point - shift) / vec2(1280., 720.);
	//	do this to repeat
	point = mod(point,1.0);
	if (point.x < 0.5)	{
		point.y = mod(point.y + slide1.y / 720., 1.0);
	}
	else	{
		point.y = mod(point.y + slide2.y / 720., 1.0);
	}
	if (point.y < 0.5)	{
		point.x = mod(point.x + slide1.x / 1280., 1.0);
	}
	else	{
		point.x = mod(point.x + slide2.x / 1280., 1.0);
	}
	//	do this for relections
	point = 1.0-abs(1.0-2.0*point);
	
	//	Now let's do a squish based on angle
	//	convert to polar coordinates
	vec2 center = vec2(0.5,0.5);
	float r = distance(center, point);
	float a = atan ((point.y-center.y),(point.x-center.x));
	
	// now do the kaleidoscope
	a = mod(a, tau/sides);
	a = abs(a - tau/sides/2.);
	
	s = sin(a + tau * angle);
	c = cos(a + tau * angle);
	
	float zoom = 1280. / 720.;
	
	point.x = (r * c)/zoom + 0.5;
	point.y = (r * s)/zoom + 0.5;
	
	return point;
}


void main() {

	vec2 pat = pattern();

	// gl_FragColor = vec4(pat.x, pat.y, 0, 1);
	gl_FragColor = texture2DRect(diffuseTexture, vec2(pat.x * 1280., pat.y*720.));
}
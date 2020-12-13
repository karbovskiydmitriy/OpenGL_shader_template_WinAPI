uniform float time;
uniform vec2 screenSize;

void main(void)
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	float t = time / 100.0;
	float f = cos(uv.y * 100.0 - t);
	f = f + sin(uv.x * 100.0 - t);
	f = 1.0 - f;
	vec3 col = vec3(0.0, 0.0, 0.5) * pow(f, 0.2);
	gl_FragColor = vec4(col, 1.0);
}
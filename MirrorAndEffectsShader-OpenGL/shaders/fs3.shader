uniform sampler2D tex;
void main() {
	// filmic tone mapping  

	vec4 objectColor = texture(tex, gl_TexCoord[0].st);

	objectColor = max(vec4(0.), objectColor - vec4(0.004));
	objectColor = (objectColor * (6.2 * objectColor + .5)) / (objectColor * (6.2 * objectColor + 1.7) + 0.06);

	gl_FragColor = objectColor;
}
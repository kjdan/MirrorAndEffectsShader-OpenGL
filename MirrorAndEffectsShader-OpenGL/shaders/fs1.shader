uniform sampler2D tex;
void main() {
	//rombindahouseToneMapping ???

	vec4 objectColor = texture(tex, gl_TexCoord[0].st);
	float gamma = 1.75;

	objectColor = exp(-1.0 / (2.72 * objectColor + 0.15));
	objectColor = pow(objectColor, vec4(1. / gamma));

	gl_FragColor = objectColor;
}
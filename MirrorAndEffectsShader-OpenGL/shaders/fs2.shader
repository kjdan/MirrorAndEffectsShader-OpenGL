uniform sampler2D tex;
void main() {
	//simpleReinhardToneMapping

	vec4 objectColor = texture(tex, gl_TexCoord[0].st);

	float gamma = 1.75;
	float exposure = 1.75;
	objectColor *= exposure / (1. + objectColor / exposure);
	objectColor = pow(objectColor, vec4(1. / gamma));

	gl_FragColor = objectColor;
}
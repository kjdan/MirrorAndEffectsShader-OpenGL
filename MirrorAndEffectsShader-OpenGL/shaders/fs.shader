uniform sampler2D tex;
void main() {
	// lumaBasedReinhardToneMapping

	vec4 objectColor = texture(tex, gl_TexCoord[0].st);
	float gamma = 1.9;

	float luma = dot(objectColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma / (1. + luma);
	objectColor *= toneMappedLuma / luma;
	objectColor = pow(objectColor, vec4(1. / gamma));
	gl_FragColor = objectColor;
} 
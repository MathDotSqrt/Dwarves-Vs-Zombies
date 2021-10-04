const float gamma = 2.2;

vec3 to_gamma(vec3 linear_color) {
	return pow(linear_color, vec3(1 / gamma));
}

vec3 to_linear(vec3 gamma_color){
	return pow(gamma_color, vec3(gamma));
}
#version 330

in vec2 f_texcoord;

out vec4 out_color;

uniform float u_time;

uniform sampler2D diffuse;

void main(){
	vec4 tex_color = texture(diffuse, f_texcoord);
	out_color = mix(tex_color, vec4(f_texcoord, 0, 1), .5);
}
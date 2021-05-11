#version 330

in vec2 f_texcoord;

out vec4 out_color;

void main(){
	out_color = vec4(f_texcoord, 1, 1);
}
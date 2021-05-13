#version 330

in vec2 f_texcoord;

out vec4 out_color;

uniform sampler2D diffuse;

void main(){
	vec4 tex_color = texture(diffuse, f_texcoord);
	out_color = tex_color;
}

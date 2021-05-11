#version 330

#include "glsl/preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec2 v_pos;

out vec2 f_texcoord;

void main(){
	gl_Position = vec4(v_pos, 0, 1);
	f_texcoord = v_pos; 
}
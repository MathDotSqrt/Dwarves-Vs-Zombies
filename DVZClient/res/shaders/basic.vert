#version 330

#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=TEXCOORD_ATTRIB_LOCATION) in vec2 v_texcoord;

out vec2 f_texcoord;

uniform mat4 MP;

void main(){
	gl_Position = MP * vec4(v_pos, 1);

	f_texcoord = v_texcoord; 
}
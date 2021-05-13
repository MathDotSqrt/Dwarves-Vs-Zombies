#version 330

#include "glsl/preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=TEXCOORD_ATTRIB_LOCATION) in vec2 v_texcoord;

uniform mat4 MP;

out vec2 f_texcoord;

void main(){
	gl_Position = MP * vec4(v_pos, 1);
	f_texcoord = v_texcoord;
}

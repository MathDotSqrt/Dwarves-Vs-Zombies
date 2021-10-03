#version 330 core
#include "glsl/preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION) in vec4 vert_pos;
layout(location = NORMAL_ATTRIB_LOCATION) 	in vec4 vert_norm;
layout(location = COLOR_ATTRIB_LOCATION) 	in vec4 vert_col;
layout(location = TEXCOORD_ATTRIB_LOCATION) in vec4 vert_uv;

out vec3 frag_normal;
out vec3 frag_color;
out vec3 frag_uv;

uniform vec3 u_pos;
uniform mat4 VP;

void main(){
    //TODO: investigate if I can just do this on CPU
    mat4 M = mat4(1);
    M[3][0] = u_pos.x;
    M[3][1] = u_pos.y;
    M[3][2] = u_pos.z; 

    frag_normal = vert_norm.xyz;
    frag_color = vert_col.rgb;
    frag_uv = vert_uv.xyz;

    gl_Position = VP * M * vec4(vert_pos.xyz, 1);
}
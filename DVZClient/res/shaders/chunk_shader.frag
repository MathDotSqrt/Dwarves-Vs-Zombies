#version 330

#include "glsl/color_correction.glsl"

in vec3 frag_normal;
in vec3 frag_color;
in vec3 frag_uv;

out vec4 final_color;

uniform sampler2DArray u_tex_atlas;

void main(){

    vec3 dir = -vec3(.1, -.5, .4);

    vec4 tex_color = texture(u_tex_atlas, frag_uv);
    tex_color.rgb = to_linear(tex_color.rgb);

    final_color = tex_color * max(dot(dir, frag_normal), .3);
    final_color.xyz = to_gamma(final_color.xyz);
}

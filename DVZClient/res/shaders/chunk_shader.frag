#version 330

in vec3 frag_normal;
in vec3 frag_color;
in vec3 frag_uv;

out vec4 final_color;

//uniform sampler2D u_diffuse;
uniform sampler2DArray u_tex_atlas;

void main(){

    vec3 dir = -vec3(.1, -.5, .4);

    vec4 tex_color = texture(u_tex_atlas, vec3(frag_uv.xy, 0));
    final_color =  tex_color * max(dot(dir, frag_normal), .3);
}

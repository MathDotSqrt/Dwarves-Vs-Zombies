#version 330

in vec3 frag_normal;
in vec3 frag_color;
in vec3 frag_uv;

out vec4 final_color;

uniform sampler2DArray tex_atlas;

void main(){

    vec3 dir = -vec3(.1, -.5, .4);

    vec4 tex_color = texture(tex_atlas, frag_uv);
    final_color = tex_color + vec4(frag_color, 1) * max(dot(dir, frag_normal), .3);
}

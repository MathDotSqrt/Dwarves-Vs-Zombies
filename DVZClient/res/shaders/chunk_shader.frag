#version 330

in vec3 frag_normal;
in vec3 frag_color;

out vec4 final_color;

void main(){

    vec3 dir = -vec3(.1, -.5, .4);



    final_color = vec4(frag_color, 1) * max(dot(dir, frag_normal), .3);
}

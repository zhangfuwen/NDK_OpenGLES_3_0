#version 300 es

// attributes
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_tex_coord;
layout(location = 2) in vec3 a_normal;

// uniforms
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_light_pos;

// varyings
out vec2 v_tex_coord;
out vec3 v_normal;
out vec3 v_frag_pos;

// main
void main()
{
    v_tex_coord = a_tex_coord;
    v_normal = a_normal;
    v_frag_pos = vec3(u_model * a_position);
    gl_Position = u_projection * u_view * vec4(v_frag_pos,1.0f);
}
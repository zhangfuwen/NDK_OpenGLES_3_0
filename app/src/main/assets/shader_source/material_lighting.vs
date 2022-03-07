#version 300 es

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_light_pos;
out vec2 v_tex_coord;
out vec3 v_normal;
out vec3 v_frag_pos;
out highp float zDepth;
out mediump vec2 texCoord;
void main() {
	v_tex_coord = a_TexCoord;
	v_normal = a_Normal;
	v_frag_pos = vec3(u_model * vec4(a_Position, 1.0f));
//	gl_Position=u_projection * u_view * vec4( v_frag_pos, 1.0f);
	gl_Position=u_projection * u_view * u_model * vec4(a_Position, 1.0f);
	zDepth = gl_Position.z/gl_Position.w;
   texCoord = vec2(a_TexCoord.x, 1.0f - a_TexCoord.y);
}
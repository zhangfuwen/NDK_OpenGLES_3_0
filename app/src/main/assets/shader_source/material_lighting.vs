#version 300 es
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
uniform mat4 u_MVPMatrix;
out highp float zDepth;
out mediump vec2 texCoord;
void main() {
	gl_Position=u_MVPMatrix * vec4(a_Position, 1.0f);
	zDepth = gl_Position.z/gl_Position.w;
   texCoord = vec2(a_TexCoord.x, 1.0f - a_TexCoord.y);
}
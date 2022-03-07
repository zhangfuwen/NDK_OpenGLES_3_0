#version 300 es
precision mediump float;
layout(location = 0) uniform sampler2D color_sampler;
in highp float zDepth;
in mediump vec2 texCoord;
layout(location = 0) out vec4 outColor;

// varyings
in vec2 v_tex_coord;
in vec3 v_normal;
in vec3 v_frag_pos;
void main(){
    vec4 color = texture(color_sampler, texCoord);
    outColor = vec4(vec3(color), 1.0f);
}
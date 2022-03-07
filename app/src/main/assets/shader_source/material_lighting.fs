#version 300 es
precision mediump float;
layout(location = 0) uniform sampler2D color_sampler;
in highp float zDepth;
in mediump vec2 texCoord;
layout(location = 0) out vec4 outColor;
struct Light {
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

struct Material {
    vec3 ambient_ratio;
    vec3 diffuse_ratio;
    vec3 specular_ratio;
    float shininess;
};

// uniforms
uniform vec3 u_light_pos;
uniform vec3 u_view_pos;
uniform Light u_light;
uniform Material u_material;

// varyings
in vec2 v_tex_coord;
in vec3 v_normal;
in vec3 v_frag_pos;
void main(){
    vec3 ambient = u_light.ambient_color * u_material.ambient_ratio;

    // diffuse
    vec3 norm = normalize(v_normal);
    vec3 light_dir = normalize(u_light_pos - v_frag_pos);
    float diff = max(dot(norm, light_dir), 0.0f);
    vec3 diffuse = u_light.diffuse_color * ( diff * u_material.diffuse_ratio);

    // specular
    vec3 view_dir = normalize(u_view_pos - v_frag_pos);
    vec3 relect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, relect_dir), 0.0f), u_material.shininess);
    vec3 specular = u_light.specular_color * (spec * u_material.specular_ratio);

    vec3 result = ambient + diffuse + specular;

    ///vec4 color = texture(color_sampler, v_tex_coord);
//    vec4 color = texture(color_sampler, texCoord);
    outColor = vec4(result, 1.0f);
}
#version 300 es
precision mediump float;


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
uniform sampler2D u_color_sampler;
uniform Light u_light;
uniform Material u_material;

// varyings
in mediump vec2 v_tex_coord;
out vec3 v_normal;
out vec3 v_frag_pos;

layout(location = 0) out vec4 outColor;

void main() {
    // ambient
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

    vec4 color = texture(u_color_sampler, v_tex_coord);

    outColor = vec4(vec3(color) + result, 1.0f);
}
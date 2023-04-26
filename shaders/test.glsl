#VS
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec3 in_norm;
layout (location = 3) in vec4 in_color;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

out vec4 frag_color;
out vec2 frag_uv;
out vec3 frag_norm;
out vec3 frag_pos;

void main() {
    
    gl_Position = proj*view*model*vec4(in_pos, 1.0f);
    
    frag_color = in_color;
    frag_uv = in_uv;
    frag_norm = in_norm;
    frag_pos = in_pos;
}

#FS
#version 330 core

in vec4 frag_color;
in vec2 frag_uv;
in vec3 frag_norm;
in vec3 frag_pos;

struct light_t {
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D diffuse_map;
uniform vec4 mat_diffuse;
uniform sampler2D specular_map;
uniform vec4 mat_specular;
uniform light_t light;
uniform vec3 view_pos;

out vec4 out_color;

void main() {
    vec3 ambient = light.ambient * texture(diffuse_map, frag_uv).rgb;
    
    vec3 norm = normalize(frag_norm);
    vec3 light_dir = normalize(-light.dir);
    float diff = max(dot(norm, light_dir), 0.0);
    
    vec3 diffuse = light.diffuse * diff * texture(diffuse_map, frag_uv).rgb;
    
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 1);
    vec3 specular = light.specular * spec * texture(specular_map, frag_uv).rgb;
    
    vec3 result = ambient + diffuse + specular;
    out_color = mat_diffuse;
    
}

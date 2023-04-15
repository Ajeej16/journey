#VS
#version 330 core
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_color;

uniform mat4 view;
uniform mat4 proj;

out vec4 frag_color;
out vec2 frag_uv;

void main() {
    
    gl_Position = proj*view*vec4(in_pos, 0.0f, 1.0f);
    
    frag_color = in_color;
    frag_uv = in_uv;
    
}

#FS
#version 330 core

in vec4 frag_color;
in vec2 frag_uv;

out vec4 out_color;

void main() {
    
    out_color = frag_color;
    
}

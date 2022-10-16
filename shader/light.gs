#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 200) out;

in VS_OUT {
    vec3 color;
    vec4 position;
} gs_in[];

out GS_OUT {
    vec3 color;
} fs_in;


void main() {
    
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        fs_in.color = gs_in[i].color;
        EmitVertex();
    }
    EndPrimitive();
}
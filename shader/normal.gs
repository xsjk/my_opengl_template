#version 450
out vec4 FragColor;

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    vec3 pos;
} gs_in[];

out GS_OUT {
    vec3 normal;
    vec3 pos;
    vec3 color;
} gs_out;

uniform mat4 camera;
uniform vec3 color;

// display normals

void main() {
    for (int i = 0; i < 3; i++) {
        gs_out.pos = gs_in[i].pos;
        gs_out.normal = gs_in[i].normal;
        gs_out.color = vec3(1, 1, 0);
        gl_Position = camera * vec4(gs_out.pos, 1.0);
        EmitVertex();
        gs_out.pos = gs_in[i].pos + gs_in[i].normal/30;
        gs_out.normal = gs_in[i].normal;
        gs_out.color = vec3(1);
        gl_Position = camera * vec4(gs_out.pos, 1.0);
        EmitVertex();
        EndPrimitive();

    }
    
}
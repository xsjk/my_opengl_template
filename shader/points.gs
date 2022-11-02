#version 450
out vec4 FragColor;

layout (triangles) in;
layout (points, max_vertices = 3) out;

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

void main() {
    for (int i = 0; i < 3; i++) {
        gl_PointSize = 10.0;

        gs_out.normal = gs_in[i].normal;
        gs_out.pos = gs_in[i].pos;
        gs_out.color = color;
        gl_Position = camera * vec4(gs_out.pos,1.0);
        EmitVertex();
        EndPrimitive();
        
    }
}
#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 200) out;


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

uniform float time;

const float k = 0.2;

void produce_vertex(vec3 pos, vec3 normal) {
    gs_out.pos = pos + normal*k*(1+sin(1.5*time))*clamp((.4-pos.y), 0, 1)*clamp(pos.y, 0, 1);
    gs_out.normal = normal;
    gs_out.color = color;
    gl_Position = camera * vec4(gs_out.pos, 1.0);    
    EmitVertex();
}

void main() {
    produce_vertex(gs_in[0].pos, gs_in[0].normal);
    produce_vertex(gs_in[1].pos, gs_in[1].normal);
    produce_vertex(gs_in[2].pos, gs_in[2].normal);

}

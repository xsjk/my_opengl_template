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

uniform float time;
uniform float t;

uniform vec3 color;


const float v = 0.5;
const float a = 1.1;

vec3 calc_pos(vec3 pos, vec3 normal, float t) {
    pos += normal*v*t - a*t*t*vec3(0, 1, 0);
    return pos;
}

float get_t() {
    return clamp(0.5-cos(1.5*time),0,2);
}

// explosion
void produce_vertex(vec3 pos, vec3 normal, vec3 center) {
    float t = get_t();
    gs_out.pos = calc_pos(pos, gs_in[0].normal, t);
    if (gs_out.pos.y<0) {
        gs_out.pos.y = 0.001;
        // gs_out.normal = normal*t/10+vec3(0,1,0);
    } else {
        // gs_out.normal = normal;
    }
    gs_out.normal = normal;
    gs_out.color = color;
    gl_Position = camera * vec4(gs_out.pos, 1.0);    
    EmitVertex();
}

void main() {

    vec3 center = (gs_in[0].pos+gs_in[1].pos+gs_in[2].pos)/3;
    produce_vertex(gs_in[0].pos, gs_in[0].normal, center);
    produce_vertex(gs_in[1].pos, gs_in[1].normal, center);
    produce_vertex(gs_in[2].pos, gs_in[2].normal, center);

}

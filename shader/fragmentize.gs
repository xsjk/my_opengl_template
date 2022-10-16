#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 200) out;

struct Light {
    // for point light and spot light
    vec4 position;
    float constant;
    float linear;
    float quadratic; 
    
    // for directional light and spot light
    vec3 direction;

    // for spot light
    float cutOff;
    float outerCutOff;

    // for all lights
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
};

const int MAX_LIGHTS = 10;

uniform Light light[MAX_LIGHTS];

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

const float v = 0.1;

float intense(Light light, vec3 pos) {
    vec3 lightDir = normalize(light.position.xyz - pos);
    float cutOff = cos(radians(light.cutOff));
    float outerCutOff = cos(radians(light.outerCutOff));
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = outerCutOff - cutOff;
    float intensity = clamp((outerCutOff - theta) / epsilon, 0.0, 1.0);
    return intensity;
}

vec3 calc_pos(vec3 pos, vec3 normal, float t) {
    pos += normal*v*intense(light[0],pos);
    return pos;
}

float get_t() {
    return 1-cos(1.5*time);
}

// explosion
void produce_vertex(vec3 pos, vec3 normal, vec3 center) {
    float t = get_t();
    gs_out.pos = calc_pos(pos, gs_in[0].normal, t);
    if (gs_out.pos.y<0) 
        gs_out.pos.y = 0.001;
    else {
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

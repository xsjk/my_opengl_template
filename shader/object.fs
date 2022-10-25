#version 450
out vec4 FragColor;

const int MAX_LIGHTS = 10;

const bool blinn = true;

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

uniform Light light[MAX_LIGHTS];
uniform vec3 viewPos;
uniform mat4 model;
uniform float opacity;

in VS_OUT {
    vec3 normal;
    vec3 pos;
    vec3 color;
} fs_in;


vec4 calc_point_light(Light light, vec3 normal, vec3 fragPos, vec3 viewPos) {

    // diffuse
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec;
    if (blinn) {
        spec = pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 32);
    } else {
        spec = pow(max(dot(normal, normalize(lightDir + viewDir)), 0.0), 32);
    }
    vec3 specular = spec * light.specular;

    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + (light.linear + light.quadratic * distance) * distance);
    diffuse *= attenuation;
    specular *= attenuation;

    return vec4((diffuse + specular)*light.color, 1.0);

}

vec4 calc_directional_light(Light light, vec3 normal, vec3 fragPos, vec3 viewPos) {

    // diffuse
    vec3 lightDir = normalize(-light.direction.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec;
    if (blinn) {
        spec = pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 32);
    } else {
        spec = pow(max(dot(normal, normalize(lightDir + viewDir)), 0.0), 32);
    }
    vec3 specular = spec * light.specular;

    return vec4((diffuse + specular)*light.color, 1.0);
}

vec4 calc_spot_light(Light light, vec3 normal, vec3 fragPos, vec3 viewPos) {
    vec3 lightDir = normalize(light.position.xyz - fragPos);

    float cutOff = cos(radians(light.cutOff));
    float outerCutOff = cos(radians(light.outerCutOff));
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = outerCutOff - cutOff;
    float intensity = clamp((outerCutOff - theta) / epsilon, 0.0, 1.0);

    return intensity * calc_point_light(light, normal, fragPos, viewPos);
}

vec4 calc_light(Light light, vec3 normal, vec3 fragPos, vec3 viewPos) {
    vec4 result = vec4(light.ambient * light.color, 1.0);
    if (light.position.w == 0.0) {
        result += calc_directional_light(light, normal, fragPos, viewPos);
    } else if (light.direction == vec3(0.0)) {
        result += calc_point_light(light, normal, fragPos, viewPos);
    } else {
        result += calc_spot_light(light, normal, fragPos, viewPos);
    }
    return result;
}

void main() {
    if (fs_in.normal == vec3(0.0)) {
        FragColor = vec4(1);
        return;
    }
    vec4 lightColor = vec4(0.0);
    for (int i = 0; i < MAX_LIGHTS; i++)
        lightColor += calc_light(light[i], fs_in.normal, fs_in.pos, viewPos);
    FragColor = lightColor * vec4(fs_in.color, opacity);

    // gamma correction
    float gamma = 2.2;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
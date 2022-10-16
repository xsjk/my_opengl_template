#version 450

in GS_OUT {
    vec3 color;
} gs_in;


out vec4 FragColor;
void main() {
    FragColor = vec4(gs_in.color, 1);
}
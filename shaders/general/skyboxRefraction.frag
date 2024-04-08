#version 330 core
out vec4 FragColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;

uniform vec3 cameraPosition;
uniform samplerCube skybox;

const float RI_air      = 1.00;
const float RI_water    = 1.33;
const float RI_ice      = 1.309;
const float RI_glass    = 1.52;
const float RI_diamond  = 2.42;

void main()
{
    float ratio = RI_air / RI_glass;
    vec3 I = normalize(FragmentPosition - cameraPosition);
    vec3 R = refract(I, normalize(VertexNormal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
} 
#version 330 core
out vec4 FragmentColor;

in vec2 TextureCoordinates;
uniform sampler2D diffuse;
uniform vec2 resolution;
uniform float time;

vec2 uvs = TextureCoordinates;

float inverseLerp(float v, float minValue, float maxValue)
{
    return (v - minValue) / (maxValue - minValue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(v, inMin, inMax);
    return mix(outMin, outMax, t);
}

void main()
{
    vec3 color = vec3(0.75);

    FragmentColor = vec4(color, 1.0);
} 
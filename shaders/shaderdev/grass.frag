#version 330 core
out vec4 FragmentColor;

uniform float time;
uniform vec2 resolution;

float inverseLerp(float v, float minValue, float maxValue)
{
    return (v - minValue) / (maxValue - minValue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(v, inMin, inMax);
    return mix(outMin, outMax, t);
}

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

void main()
{
    vec3 color = vec3(0.5);

    FragmentColor = vec4(color, 1.0);
}
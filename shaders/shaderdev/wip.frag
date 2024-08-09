#version 330 core
out vec4 FragmentColor;

in vec2 TextureCoordinates;
vec2 uvs = TextureCoordinates;

uniform vec2 resolution;
uniform float time;

float inverseLerp(float v, float minValue, float maxValue)
{
    return (v - minValue) / (maxValue - minValue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(v, inMin, inMax);
    return mix(outMin, outMax, t);
}

float random(vec2 p)
{
    p = 50.0 * fract(p * 0.3183099 + vec2(0.71, 0.113));
    return -1.0 + 2.0 * fract(p.x * p.y * (p.x + p.y));
}

vec4 noise(vec2 coords)
{
    vec2 texSize = vec2(2.0);
    vec2 pc = coords * texSize - 0.5;
    vec2 base = floor(pc) + 0.5;

    float s1 = random((base + vec2(0.0, 0.0)) / texSize);
    float s2 = random((base + vec2(1.0, 0.0)) / texSize);
    float s3 = random((base + vec2(0.0, 1.0)) / texSize);
    float s4 = random((base + vec2(1.0, 1.0)) / texSize);

    vec2 f = smoothstep(0.0, 1.0, fract(pc));

    float px1 = mix(s1, s2, f.x);
    float px2 = mix(s3, s4, f.x);
    float result = mix(px1, px2, f.y);

    return vec4(result);
}

void main()
{
    vec4 color = noise(uvs * 10.0);

    FragmentColor = color;
}
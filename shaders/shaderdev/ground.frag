#version 330 core
out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;
in vec2 TextureCoordinates;
vec2 uvs = TextureCoordinates;

uniform sampler2D diffuseTexture;

float inverseLerp(float v, float minValue, float maxValue)
{
    return (v - minValue) / (maxValue - minValue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(v, inMin, inMax);
    return mix(outMin, outMax, t);
}

float hash(vec2 p)
{
    p = 50.0 * fract(p * 0.3183099 + vec2(0.71, 0.113));
    return -1.0 + 2.0 * fract(p.x * p.y * (p.x + p.y));
}

void main()
{
    // Grid
    float grid1 = texture(diffuseTexture, FragmentPosition.xz * 0.1).x;
    float grid2 = texture(diffuseTexture, FragmentPosition.xz).x;

    float gridHash = hash(floor(FragmentPosition.xz));

    vec3 gridColor = mix(vec3(0.5 + remap(gridHash, -1.0, 1.0, -0.2, 0.2)), vec3(0.0625), grid2);
    gridColor = mix(gridColor, vec3(0.00625), grid1);

    vec3 color = vec3(gridColor);

    FragmentColor = vec4(color, 1.0);
}
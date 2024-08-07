#version 330 core
out vec4 FragmentColor;

in vec3 VertexNormal;
in vec2 TextureCoordinates;
in vec3 FragmentPosition;

uniform samplerCube specMap;

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
    vec3 baseColor = vec3(0.5);
    vec3 lighting = vec3(0.0);
    vec3 normal = normalize(VertexNormal);

    // Ambient
    vec3 ambient = vec3(0.1);

    // Diffuse lighting
    vec3 lightDir = normalize(vec3(0.8, 1.2, 1.0));
    vec3 lightColor = vec3(1.0, 1.0, 0.9);
    float dp = max(0.0, dot(lightDir, normal));

    vec3 diffuse = dp * lightColor;

    lighting = ambient + diffuse;

    vec3 color = baseColor * lighting;

    FragmentColor = vec4(color, 1.0);
}
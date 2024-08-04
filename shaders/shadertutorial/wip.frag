#version 330 core
out vec4 FragmentColor;

in vec3 VertexNormal;
in vec2 TextureCoordinates;
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
    vec3 baseColor = vec3(0.5);
    vec3 lighting = vec3(0.0);
    vec3 normal = normalize(VertexNormal);

    // Ambient
    vec3 ambient = vec3(0.5);

    // Hemi light
    vec3 skyColor = vec3(0.0, 0.3, 0.6);
    vec3 groundColor = vec3(0.6, 0.3, 0.1);

    float hemiMix = remap(normal.y, -1.0, 1.0, 0.0, 1.0);
    vec3 hemi = mix(groundColor, skyColor, hemiMix);

    lighting = ambient * 0.0 + hemi;

    vec3 color = baseColor * lighting;

    FragmentColor = vec4(color, 1.0);
} 
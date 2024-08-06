#version 330 core
out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FresnelNormal;
in vec2 TextureCoordinates;
in vec3 FragmentPosition;

uniform float time;
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
    vec3 viewDir = normalize(-FragmentPosition);

    // Hemi light
    vec3 skyColor = vec3(0.6, 0.8, 0.9);
    vec3 groundColor = vec3(0.0, 0.05, 0.2);

    float hemiMix = remap(normal.y, -1.0, 1.0, 0.0, 1.0);
    vec3 hemi = mix(groundColor, skyColor, hemiMix);

    // Diffuse lighting
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 lightColor = vec3(1.0, 1.0, 0.9);
    float dp = max(0.0, dot(lightDir, normal));

    // Colorbanding
    dp = mix(0.5, 1.0, step(0.65, dp)) * step(0.5, dp);

    vec3 diffuse = dp * lightColor;

    // Phong specular
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float phongValue = max(0.0, dot(viewDir, reflectDir));
    phongValue = pow(phongValue, 32.0);

    // Fresnel
    float fresnel = 1.0 - max(0.0, dot(viewDir, FresnelNormal));
    fresnel = pow(fresnel, 2.2);
    fresnel *= step(0.7, fresnel);

    vec3 specular = vec3(phongValue);
    specular = smoothstep(0.5, 0.52, specular);

    lighting = hemi * (fresnel + 0.3) + diffuse;

    vec3 color = baseColor * lighting + specular;

    FragmentColor = vec4(color, 1.0);
}
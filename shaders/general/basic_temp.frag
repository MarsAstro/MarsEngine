#version 330 core
struct PointLight {
    vec4 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float constant;
    float linear;
    float quadratic;
    float PADDING;
};

#define MAX_POINT_LIGHTS 64
layout (std140) uniform PointLights
{
    PointLight pointLights[MAX_POINT_LIGHTS];
    int numPointLights;
};

out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDirection);

void main()
{
    vec3 normal = normalize(VertexNormal);
    vec3 viewDirection = normalize(-FragmentPosition);

    vec3 result = vec3(0.0);

    int lightCount = min(numPointLights, MAX_POINT_LIGHTS);
    for(int i = 0; i < lightCount; i++)
    result += CalculatePointLight(pointLights[i], normal, viewDirection);

    FragmentColor = vec4(result, 1.0);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection     = normalize(light.position.xyz - FragmentPosition);
    vec3 halfwayDirection   = normalize(lightDirection + viewDirection);

    float diffuseAmount = max(dot(normal, lightDirection), 0.0);
    float specularAmount = pow(max(dot(normal, halfwayDirection), 0.0), 64.0);

    vec3 objColor   = vec3(0.5, 0.5, 0.5);
    vec3 ambient    = light.ambient.xyz * objColor;
    vec3 diffuse    = light.diffuse.xyz * diffuseAmount * objColor;
    vec3 specular   = light.specular.xyz * specularAmount * objColor;

    float distance = length(light.position.xyz - FragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (ambient + diffuse + specular) * attenuation;
}
#version 330 core
struct Material {
    vec3 ambientColor;

    vec3 diffuseColor;
    sampler2D diffuseMap;
    bool hasDiffuseMap;

    vec3 specularColor;
    sampler2D specularMap;
    bool hasSpecularMap;

    vec3 emissiveColor;
    sampler2D emissiveMap;
    bool hasEmissiveMap;

    float shininess;
};
#define MAX_MATERIALS 64
uniform Material materials[MAX_MATERIALS];

struct Surface {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

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
in vec2 TextureCoordinates;
flat in int MaterialIndex;

vec3 CalculatePointLight(PointLight light, Surface surface, vec3 normal, vec3 viewDirection);
Surface CalculateSurface();

void main()
{
    Surface surface = CalculateSurface();
    vec3 normal = normalize(VertexNormal);
    vec3 viewDirection = normalize(-FragmentPosition);

    vec3 result = vec3(0.0);

    int lightCount = min(numPointLights, MAX_POINT_LIGHTS);
    for(int i = 0; i < lightCount; i++)
    result += CalculatePointLight(pointLights[i], surface, normal, viewDirection);

    FragmentColor = vec4(result, 1.0);
}

vec3 CalculatePointLight(PointLight light, Surface surface, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection     = normalize(light.position.xyz - FragmentPosition);
    vec3 halfwayDirection   = normalize(lightDirection + viewDirection);

    float diffuseAmount = max(dot(normal, lightDirection), 0.0);
    float specularAmount = pow(max(dot(normal, halfwayDirection), 0.0), surface.shininess);

    vec3 ambient    = light.ambient.xyz * surface.ambient;
    vec3 diffuse    = light.diffuse.xyz * diffuseAmount * surface.diffuse;
    vec3 specular   = light.specular.xyz * specularAmount * surface.specular;

    float distance = length(light.position.xyz - FragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (ambient + diffuse + specular) * attenuation;
}

Surface CalculateSurface()
{
    Surface surface;

    surface.ambient = vec3(1.0);
    surface.diffuse = vec3(0.5);
    surface.specular = vec3(0.5);
    surface.shininess = 250.0;

    if (MaterialIndex >= 0)
    {
        surface.ambient = materials[MaterialIndex].ambientColor;

        if (materials[MaterialIndex].hasDiffuseMap)
            surface.diffuse = vec3(texture(materials[MaterialIndex].diffuseMap, TextureCoordinates));
        else
            surface.diffuse = materials[MaterialIndex].diffuseColor;

        if (materials[MaterialIndex].hasSpecularMap)
            surface.specular = vec3(texture(materials[MaterialIndex].specularMap, TextureCoordinates));
        else
            surface.specular = materials[MaterialIndex].specularColor;

        surface.shininess = materials[MaterialIndex].shininess;
    }

    surface.ambient = surface.ambient * surface.diffuse;

    return surface;
}
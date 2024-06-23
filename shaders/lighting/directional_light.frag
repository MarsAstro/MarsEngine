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

struct DirectionalLight {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform DirectionalLight directionalLight;

out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;
in vec2 TextureCoordinates;
flat in int MaterialIndex;

Surface CalculateSurface();

void main()
{
    Surface surface = CalculateSurface();
    vec3 normal = normalize(VertexNormal);
    vec3 viewDirection = normalize(-FragmentPosition);

    vec3 lightDirection     = normalize(-directionalLight.direction.xyz);
    vec3 halfwayDirection   = normalize(lightDirection + viewDirection);

    float diffuseAmount     = max(dot(normal, lightDirection), 0.0);
    float specularAmount    = pow(max(dot(viewDirection, halfwayDirection), 0.0), surface.shininess);

    vec3 ambient    = directionalLight.ambient.xyz * surface.ambient;
    vec3 diffuse    = directionalLight.diffuse.xyz * diffuseAmount * surface.diffuse;
    vec3 specular   = directionalLight.specular.xyz * specularAmount * surface.specular;

    FragmentColor = vec4(ambient + diffuse + specular, 1.0);
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
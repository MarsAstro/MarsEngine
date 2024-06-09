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
#define MAX_MATERIALS 12
uniform Material materials[MAX_MATERIALS];

out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;
in vec2 TextureCoordinates;
flat in int MaterialIndex;

vec3 GetDiffuse();

void main()
{
    FragmentColor = vec4(GetDiffuse(), 1.0);
}

vec3 GetDiffuse()
{
    vec3 result = vec3(0.5);

    if (MaterialIndex >= 0)
    {
        if (materials[MaterialIndex].hasDiffuseMap)
            result = vec3(texture(materials[MaterialIndex].diffuseMap, TextureCoordinates));
        else
            result = materials[MaterialIndex].diffuseColor;
    }

    return result;
}
#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
    float emissiveness;
};

uniform Material material;

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;
in vec2 TextureCoordinates;

void main()
{
    vec3 normal = normalize(VertexNormal);
    vec3 lightDirection = normalize(-light.direction);
    float diffuseAmount = max(dot(normal, lightDirection), 0.0);

    vec3 viewDirection = normalize(-FragmentPosition);
    vec3 lightReflectDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, lightReflectDirection), 0.0), material.shininess);
    vec3 specularMapValue = vec3(texture(material.specular, TextureCoordinates));

    vec3 ambient    = light.ambient * vec3(texture(material.diffuse, TextureCoordinates));
    vec3 diffuse    = light.diffuse * diffuseAmount * vec3(texture(material.diffuse, TextureCoordinates));
    vec3 specular   = light.specular * specularAmount * specularMapValue;
    vec3 emission   = vec3(texture(material.emission, TextureCoordinates)) * (vec3(1.0) - ceil(specularMapValue)) * material.emissiveness;

    FragmentColor = vec4(ambient + diffuse + specular + emission, 1.0);
}
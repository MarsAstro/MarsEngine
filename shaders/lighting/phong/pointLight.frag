#version 330 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

uniform Material material;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLight;

out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;
in vec2 TextureCoordinates;

void main()
{
    vec3 normal = normalize(VertexNormal);
    vec3 lightDirection = normalize(pointLight.position - FragmentPosition);
    float diffuseAmount = max(dot(normal, lightDirection), 0.0);

    vec3 viewDirection = normalize(-FragmentPosition);
    vec3 lightReflectDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, lightReflectDirection), 0.0), material.shininess);

    vec3 ambient    = pointLight.ambient * vec3(texture(material.texture_diffuse1, TextureCoordinates));
    vec3 diffuse    = pointLight.diffuse * diffuseAmount * vec3(texture(material.texture_diffuse1, TextureCoordinates));
    vec3 specular   = pointLight.specular * specularAmount * vec3(texture(material.texture_specular1, TextureCoordinates));

    float distance = length(pointLight.position - FragmentPosition);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    FragmentColor = vec4((ambient + diffuse + specular) * attenuation, 1.0);
}
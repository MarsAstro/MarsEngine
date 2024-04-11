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

#define NR_POINT_LIGHTS 128
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int numPointLights;

out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;
in vec2 TextureCoordinates;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDirection);

void main()
{
    vec3 normal = normalize(VertexNormal);
    vec3 viewDirection = normalize(-FragmentPosition);

    vec3 result = vec3(0.0);

    int lightCount = min(numPointLights, NR_POINT_LIGHTS);
    for(int i = 0; i < lightCount; i++)
        result += CalculatePointLight(pointLights[i], normal, viewDirection);

    FragmentColor = vec4(result, 1.0);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(light.position - FragmentPosition);
    float diffuseAmount = max(dot(normal, lightDirection), 0.0);

    vec3 lightReflectDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, lightReflectDirection), 0.0), material.shininess);

    vec3 ambient    = light.ambient * vec3(texture(material.texture_diffuse1, TextureCoordinates));
    vec3 diffuse    = light.diffuse * diffuseAmount * vec3(texture(material.texture_diffuse1, TextureCoordinates));
    vec3 specular   = light.specular * specularAmount * vec3(texture(material.texture_specular1, TextureCoordinates));

    float distance = length(light.position - FragmentPosition); 
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

    return (ambient + diffuse + specular) * attenuation; 
}
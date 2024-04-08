#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
    float emissiveness;
};

uniform Material material;

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight directionalLight;

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 diffuse;
    vec3 specular;

    float cutoff;
    float outerCutoff;
};

uniform SpotLight spotLight;

struct PointLight {
    vec3 position;

    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;
in vec2 TextureCoordinates;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDirection);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDirection);

void main()
{
    vec3 normal = normalize(VertexNormal);
    vec3 viewDirection = normalize(-FragmentPosition);

    vec3 result = CalculateDirectionalLight(directionalLight, normal, viewDirection);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalculatePointLight(pointLights[i], normal, viewDirection);

    result += CalculateSpotLight(spotLight, normal, viewDirection);

    FragmentColor = vec4(result, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-light.direction);
    float diffuseAmount = max(dot(normal, lightDirection), 0.0);

    vec3 lightReflectDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, lightReflectDirection), 0.0), material.shininess);

    vec3 ambient    = light.ambient     * vec3(texture(material.diffuse,    TextureCoordinates)); 
    vec3 diffuse    = light.diffuse     * vec3(texture(material.diffuse,    TextureCoordinates)) * diffuseAmount; 
    vec3 specular   = light.specular    * vec3(texture(material.specular,   TextureCoordinates)) * specularAmount; 

    return vec3(ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(light.position - FragmentPosition);
    float theta     = dot(lightDirection, normalize(-light.direction));

    if (theta > light.outerCutoff)
    {
        float epsilon   = light.cutoff - light.outerCutoff;
        float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

        vec3 normal = normalize(VertexNormal);
        float diffuseAmount = max(dot(normal, lightDirection), 0.0);

        vec3 lightReflectDirection = reflect(-lightDirection, normal);
        float specularAmount = pow(max(dot(viewDirection, lightReflectDirection), 0.0), material.shininess);

        vec3 diffuse    = light.diffuse     * vec3(texture(material.diffuse,    TextureCoordinates)) * diffuseAmount; 
        vec3 specular   = light.specular    * vec3(texture(material.specular,   TextureCoordinates)) * specularAmount;

        return (diffuse + specular) * intensity;
    }
    
    return vec3(0.0);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(light.position - FragmentPosition);
    float diffuseAmount = max(dot(normal, lightDirection), 0.0);

    vec3 lightReflectDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, lightReflectDirection), 0.0), material.shininess);

    float distance = length(light.position - FragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuse    = light.diffuse     * vec3(texture(material.diffuse,    TextureCoordinates)) * diffuseAmount;
    vec3 specular   = light.specular    * vec3(texture(material.specular,   TextureCoordinates)) * specularAmount;

    return (diffuse + specular) * attenuation;
}
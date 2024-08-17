#version 330 core
out vec4 FragmentColor;

uniform float time;
uniform vec2 resolution;
uniform vec3 cameraPosition;

in vec3 VertexColor;
in vec3 VertexNormal;
in vec3 FragmentPosition;
in vec4 GrassData;

float inverseLerp(float v, float minValue, float maxValue)
{
    return (v - minValue) / (maxValue - minValue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(v, inMin, inMax);
    return mix(outMin, outMax, t);
}

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

vec3 lambertLight(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 lightColor)
{
    float wrap = 0.5;
    float dotNL = saturate(dot(normal, lightDir) + wrap) / (1.0 + wrap);
    vec3 lighting = vec3(dotNL);

    float backlight = saturate((dot(viewDir, -lightDir) + wrap) / (1.0 + wrap));
    vec3 scatter = vec3(pow(backlight, 2.0));

    lighting += scatter;

    return lighting * lightColor;
}

vec3 hemiLight(vec3 normal, vec3 groundColor, vec3 skyColor)
{
    return mix(groundColor, skyColor, 0.5 * normal.y + 0.5);
}

void main()
{
    vec3 baseColor = mix(VertexColor * 0.75, VertexColor, smoothstep(0.125, 0.0, abs(GrassData.x)));
    vec3 normal = normalize(VertexNormal);
    vec3 viewDir = normalize(cameraPosition - FragmentPosition);

    // Hemi
    vec3 c1 = vec3(1.0, 1.0, 0.75);
    vec3 c2 = vec3(0.05, 0.05, 0.25);

    vec3 ambientLighting = hemiLight(normal, c2, c1);

    // Diffuse
    vec3 lightDir = normalize(vec3(-1.0, 0.5, 1.0));
    vec3 lightColor = vec3(1.0);
    vec3 diffuse = lambertLight(normal, viewDir, lightDir, lightColor);

    vec3 lighting = ambientLighting * 0.5 + diffuse * 0.5;

    vec3 color = baseColor * lighting;

    FragmentColor = vec4(color, 1.0);
}
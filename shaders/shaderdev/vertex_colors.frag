#version 330 core
out vec4 FragmentColor;

in vec3 VertexNormal;
in vec2 TextureCoordinates;
in vec3 FragmentPosition;
in vec3 LocalPosition;
in vec4 VertexColor;

uniform float time;

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
    vec3 baseColor = VertexColor.xyz;

    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 yellow = vec3(1.0, 1.0, 0.0);
    vec3 blue = vec3(0.0, 0.0, 1.0);

    float value1 = VertexColor.w;
    float line = smoothstep(
            0.003, 0.004, abs(LocalPosition.y - mix(-1.0, 0.0, value1)));
    baseColor = mix(yellow, baseColor, line);

    if (LocalPosition.y > 0.0)
    {
        float t = remap(LocalPosition.x, -1.0, 1.0, 0.0, 1.0);
        float t2 = pow(t, 2.0);

        t = mix(t, t2, remap(sin(time), -1.0, 1.0, 0.0, 1.0));

        baseColor = vec4(mix(red, blue, t), t).xyz;

        float value2 = t;
        float line2 = smoothstep(
            0.003, 0.004, abs(LocalPosition.y - mix(0.0, 1.0, value2)));
        baseColor = mix(yellow, baseColor, line2);
    }

    // Dividing line
    float middleLine = smoothstep(0.008, 0.01, abs(LocalPosition.y));
    baseColor = mix(vec3(0), baseColor, middleLine);

    vec3 lighting = vec3(0.0);
    vec3 normal = normalize(VertexNormal);

    // Ambient
    vec3 ambient = vec3(0.1);

    // Diffuse lighting
    vec3 lightDir = normalize(vec3(0.8, 1.2, 1.0));
    vec3 lightColor = vec3(1.0, 1.0, 0.9);
    float dp = max(0.0, dot(lightDir, normal));

    vec3 diffuse = dp * lightColor;

    lighting = ambient + diffuse;

    vec3 color = baseColor * lighting;

    FragmentColor = vec4(color, 1.0);
}
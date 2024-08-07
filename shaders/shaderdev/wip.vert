#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinates;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};
uniform mat4 model;

out vec2 TextureCoordinates;
out vec3 VertexNormal;
out vec3 FragmentPosition;
out vec3 LocalPosition;
out vec4 VertexColor;

float inverseLerp(float v, float minValue, float maxValue)
{
    return (v - minValue) / (maxValue - minValue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(v, inMin, inMax);
    return mix(outMin, outMax, t);
}

mat3 rotateX(float radians)
{
    float sn = sin(radians);
    float cs = cos(radians);

    return mat3(
        1.0, 0.0, 0.0,
        0.0, cs,  -sn,
        0.0, sn,  cs
    );
}

mat3 rotateY(float radians)
{
    float sn = sin(radians);
    float cs = cos(radians);

    return mat3(
        cs,  0.0, sn,
        0.0, 1.0, 0.0,
        -sn, 0.0, cs
    );
}

mat3 rotateZ(float radians)
{
    float sn = sin(radians);
    float cs = cos(radians);

    return mat3(
        cs,  -sn, 0.0,
        sn,  cs,  0.0,
        0.0, 0.0, 1.0
    );
}

void main()
{
    vec3 localSpacePosition = position;

    gl_Position = projection * view * model * vec4(localSpacePosition, 1.0);
    VertexNormal = mat3(transpose(inverse(model))) * normal;
    LocalPosition = position;
    FragmentPosition = vec3(view * model * vec4(position, 1.0));
    TextureCoordinates = textureCoordinates;

    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 blue = vec3(0.0, 0.0, 1.0);

    float t = remap(position.x, -1.0, 1.0, 0.0, 1.0);
    t = pow(t, 2.0);

    VertexColor = vec4(mix(red, blue, t), t);
}
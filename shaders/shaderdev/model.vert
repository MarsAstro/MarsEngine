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
out vec3 FresnelNormal;
out vec3 FragmentPosition;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    VertexNormal = mat3(transpose(inverse(model))) * normal;
    FresnelNormal = mat3(transpose(inverse(view * model))) * normal;
    FragmentPosition = vec3(view * model * vec4(position, 1.0));
    TextureCoordinates = textureCoordinates;
}
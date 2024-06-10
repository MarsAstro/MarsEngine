#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinates;
layout (location = 3) in int materialIndex;
layout (location = 4) in mat4 instanceMatrix;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

out vec3 VertexNormal;
out vec3 FragmentPosition;
out vec2 TextureCoordinates;
flat out int MaterialIndex;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(position, 1.0);
    FragmentPosition = vec3(view * instanceMatrix * vec4(position, 1.0));
    VertexNormal = mat3(transpose(inverse(view * instanceMatrix))) * normal;
    TextureCoordinates = textureCoordinates;
    MaterialIndex = materialIndex;
}
#version 330 core
layout (location = 0) in vec3 position;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

out vec3 TextureCoordinates;

void main()
{
    TextureCoordinates = position;

    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}
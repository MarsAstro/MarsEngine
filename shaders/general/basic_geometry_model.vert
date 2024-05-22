#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 textureCoordinates;

out VS_OUT {
    vec2 textureCoordinates;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};
uniform mat4 model;

void main()
{
    vs_out.textureCoordinates = textureCoordinates;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TextureCoordinates;

void main()
{
    gl_Position = vec4(position, 1.0);
    TextureCoordinates = textureCoordinates;
}
#version 330 core
out vec4 FragColor;

in vec2 TextureCoordinates;

uniform sampler2D screenTexture;

const float offset = 1.0 / 400.0;

void main()
{
    FragColor = texture(screenTexture, TextureCoordinates);
}
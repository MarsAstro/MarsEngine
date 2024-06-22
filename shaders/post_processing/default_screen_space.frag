#version 330 core
out vec4 FragColor;

in vec2 TextureCoordinates;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, TextureCoordinates);

    float gamma = 2.0;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
} 
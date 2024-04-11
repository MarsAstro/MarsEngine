#version 330 core
out vec4 FragColor;

uniform sampler2D texture1;

in vec2 TextureCoordinates;

void main()
{
    vec4 texColor = texture(texture1, TextureCoordinates);

    if(texColor.a < 0.0001)
        discard;

    FragColor = texColor;
} 
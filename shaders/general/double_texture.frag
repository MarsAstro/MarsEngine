#version 330 core
out vec4 FragmentColor;

in vec2 vertexUVs;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragmentColor = mix(texture(texture1, vertexUVs), texture(texture2, vertexUVs), 0.2);
} 